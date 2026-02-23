#include "cubic_curve_fitter.h"
#include <iomanip>

CubicCurveFitter::CubicCurveFitter(double x0, double y0, double yaw0,
                                     double x1, double y1, double yaw1)
    : x0_(x0), y0_(y0), yaw0_(yaw0)
{
    double L = std::hypot(x1 - x0, y1 - y0);

    Vec2 T0{std::cos(yaw0)*L, std::sin(yaw0)*L};
    Vec2 T1{std::cos(yaw1)*L, std::sin(yaw1)*L};

    d_ = {x0, y0};
    c_ = T0;
    b_ = {-3*x0 - 2*T0[0] + 3*x1 - T1[0], -3*y0 - 2*T0[1] + 3*y1 - T1[1]};
    a_ = {2*x0 + T0[0] - 2*x1 + T1[0], 2*y0 + T0[1] - 2*y1 + T1[1]};
}

CubicCurveFitter::Vec2 CubicCurveFitter::evaluate(double t) const {
    Vec2 p;
    for(int i=0;i<2;i++)
        p[i] = a_[i]*t*t*t + b_[i]*t*t + c_[i]*t + d_[i];
    return p;
}

void CubicCurveFitter::reparameterizeArcLength(std::vector<double>& s_vals,
                                                std::vector<Vec2>& uv_vals,
                                                double& total_length,
                                                int n) const
{
    s_vals.resize(n);
    uv_vals.resize(n);
    std::vector<double> speed(n);

    // 采样 t
    for(int i=0;i<n;i++){
        double t = double(i)/(n-1);
        auto p = evaluate(t);
        uv_vals[i] = p;

        // 速度
        double dx = 3*a_[0]*t*t + 2*b_[0]*t + c_[0];
        double dy = 3*a_[1]*t*t + 2*b_[1]*t + c_[1];
        speed[i] = std::sqrt(dx*dx + dy*dy);
    }

    // 累加弧长
    s_vals[0] = 0.0;
    for(int i=1;i<n;i++){
        double ds = 0.5*(speed[i-1]+speed[i]) * (1.0/(n-1));
        s_vals[i] = s_vals[i-1] + ds;
    }

    total_length = s_vals.back();

    // 转局部坐标 U,V
    for(int i=0;i<n;i++){
        uv_vals[i] = worldToLocal(uv_vals[i][0], uv_vals[i][1], x0_, y0_, yaw0_);
    }
}

CubicCurveFitter::Vec2 CubicCurveFitter::worldToLocal(double x, double y,
                                                        double x0, double y0,
                                                        double hdg)
{
    double dx = x - x0;
    double dy = y - y0;
    double U = std::cos(hdg)*dx + std::sin(hdg)*dy;
    double V = -std::sin(hdg)*dx + std::cos(hdg)*dy;
    return {U,V};
}

// 拟合三次多项式 (最小二乘 polyfit)
// 使用法方程求解: (X^T * X) * coeffs = X^T * y
// s_norm: 归一化到 [0,1] 的弧长参数
// 返回系数 [a,b,c,d] 对应 a*t^3 + b*t^2 + c*t + d
std::array<double,4> CubicCurveFitter::fitCubic(const std::vector<double>& s_norm,
                                                 const std::vector<double>& vals)
{
    int n = static_cast<int>(s_norm.size());
    if (n < 4) {
        return {0, 0, 0, 0};  // 需要至少4个点来拟合3次多项式
    }

    // 构建法方程矩阵 A = X^T * X (4x4)
    // 和右端向量 b = X^T * y
    double A[4][4] = {{0}};
    double b[4] = {0};

    for (int i = 0; i < n; ++i) {
        double t = s_norm[i];  // 已经归一化到 [0,1]
        double t2 = t * t;
        double t3 = t2 * t;
        double t4 = t3 * t;
        double t5 = t4 * t;
        double t6 = t5 * t;

        // X^T * X 的元素
        A[0][0] += 1;
        A[0][1] += t;
        A[0][2] += t2;
        A[0][3] += t3;
        A[1][1] += t2;
        A[1][2] += t3;
        A[1][3] += t4;
        A[2][2] += t4;
        A[2][3] += t5;
        A[3][3] += t6;

        // X^T * y 的元素
        double y = vals[i];
        b[0] += y;
        b[1] += y * t;
        b[2] += y * t2;
        b[3] += y * t3;
    }

    // 对称填充
    A[1][0] = A[0][1];
    A[2][0] = A[0][2];
    A[2][1] = A[1][2];
    A[3][0] = A[0][3];
    A[3][1] = A[1][3];
    A[3][2] = A[2][3];

    // 高斯消元求解 4x4 线性方程组
    for (int col = 0; col < 4; ++col) {
        // 选主元
        int max_row = col;
        for (int row = col + 1; row < 4; ++row) {
            if (std::abs(A[row][col]) > std::abs(A[max_row][col])) {
                max_row = row;
            }
        }
        // 交换行
        if (max_row != col) {
            std::swap(A[col], A[max_row]);
            std::swap(b[col], b[max_row]);
        }

        // 消元
        for (int row = col + 1; row < 4; ++row) {
            if (std::abs(A[col][col]) < 1e-12) continue;  // 奇异矩阵
            double factor = A[row][col] / A[col][col];
            for (int k = col; k < 4; ++k) {
                A[row][k] -= factor * A[col][k];
            }
            b[row] -= factor * b[col];
        }
    }

    // 回代
    double x[4] = {0};
    for (int i = 3; i >= 0; --i) {
        x[i] = b[i];
        for (int j = i + 1; j < 4; ++j) {
            x[i] -= A[i][j] * x[j];
        }
        if (std::abs(A[i][i]) > 1e-12) {
            x[i] /= A[i][i];
        }
    }

    // x[0]=d, x[1]=c, x[2]=b, x[3]=a (对应 a*t^3 + b*t^2 + c*t + d)
    return {x[3], x[2], x[1], x[0]};
}

std::vector<double> CubicCurveFitter::extractU(const std::vector<Vec2>& uv_vals) {
    std::vector<double> u_vals(uv_vals.size());
    for (size_t i = 0; i < uv_vals.size(); ++i) {
        u_vals[i] = uv_vals[i][0];
    }
    return u_vals;
}

std::vector<double> CubicCurveFitter::extractV(const std::vector<Vec2>& uv_vals) {
    std::vector<double> v_vals(uv_vals.size());
    for (size_t i = 0; i < uv_vals.size(); ++i) {
        v_vals[i] = uv_vals[i][1];
    }
    return v_vals;
}

void CubicCurveFitter::printOpenDRIVE(std::ostream& os,
                                        const std::array<double,4>& coef_U,
                                        const std::array<double,4>& coef_V,
                                        double total_length) const
{
    os << "\n===== OpenDRIVE planView =====\n";
    os << "<planView>\n";
    os << std::fixed << std::setprecision(6);
    os << "    <geometry s=\"0.0\" "
       << "x=\"" << x0_ << "\" "
       << "y=\"" << y0_ << "\" "
       << "hdg=\"" << std::scientific << std::setprecision(12) << yaw0_ << "\" "
       << "length=\"" << total_length << "\">\n";

    // coef_U = {a, b, c, d} 对应 a*t^3 + b*t^2 + c*t + d
    // OpenDRIVE paramPoly3: aU=常数项, bU=一次项, cU=二次项, dU=三次项
    // 所以需要反转: aU=d, bU=c, cU=b, dU=a
    os << "        <paramPoly3 "
       << "aU=\"" << coef_U[3] << "\" "
       << "bU=\"" << coef_U[2] << "\" "
       << "cU=\"" << coef_U[1] << "\" "
       << "dU=\"" << coef_U[0] << "\" "
       << "aV=\"" << coef_V[3] << "\" "
       << "bV=\"" << coef_V[2] << "\" "
       << "cV=\"" << coef_V[1] << "\" "
       << "dV=\"" << coef_V[0] << "\" "
       << "pRange=\"arcLength\"/>\n";

    os << "    </geometry>\n";
    os << "</planView>\n";
}
