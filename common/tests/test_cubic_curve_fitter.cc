#include "common/cubic_curve_fitter.h"
#include "common/odr_geometry.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>

int main(){
    // 输入参数 (与 Python 代码一致)
    double x0 = 100.0, y0 = 0.0, yaw0 = 0.0;
    double x1 = 320.0, y1 = 3.7, yaw1 = 0.0;

    // 创建三次曲线拟合器
    CubicCurveFitter curve(x0, y0, yaw0, x1, y1, yaw1);

    // 弧长重参数化
    std::vector<double> s_vals;
    std::vector<CubicCurveFitter::Vec2> uv_vals;
    double total_length;

    curve.reparameterizeArcLength(s_vals, uv_vals, total_length);

    // 提取 U 和 V 分量
    auto u_vals = CubicCurveFitter::extractU(uv_vals);
    auto v_vals = CubicCurveFitter::extractV(uv_vals);

    // 直接使用 s_vals (不归一化, 与 Python 行为一致)
    // 拟合三次多项式
    auto coef_U = CubicCurveFitter::fitCubic(s_vals, u_vals);
    auto coef_V = CubicCurveFitter::fitCubic(s_vals, v_vals);

    // 输出 OpenDRIVE 格式
    curve.printOpenDRIVE(std::cout, coef_U, coef_V, total_length);

    // 验证: 打印拟合结果
    std::cout << "\n===== 拟合系数验证 =====\n";
    std::cout << std::scientific << std::setprecision(12);
    std::cout << "coef_U (au, bu, cu, du): ";
    for (int i = 3; i >= 0; --i) std::cout << coef_U[i] << " ";
    std::cout << "\n";
    std::cout << "coef_V (av, bv, cv, dv): ";
    for (int i = 3; i >= 0; --i) std::cout << coef_V[i] << " ";
    std::cout << "\n";

    // 验证: 在某些采样点比较原曲线和拟合曲线
    std::cout << "\n===== 验证拟合精度 =====\n";
    std::cout << "采样点对比 (s, 原U, 拟合U, 原V, 拟合V):\n";
    for (size_t i = 0; i < s_vals.size(); i += 400) {
        double s = s_vals[i];
        double t = s;  // 使用原始 s 值 (与 Python 一致)

        // 原值
        double orig_U = u_vals[i];
        double orig_V = v_vals[i];

        // 拟合值: a*s^3 + b*s^2 + c*s + d = coef[3]*t^3 + coef[2]*t^2 + coef[1]*t + coef[0]
        double fit_U = coef_U[3]*t*t*t + coef_U[2]*t*t + coef_U[1]*t + coef_U[0];
        double fit_V = coef_V[3]*t*t*t + coef_V[2]*t*t + coef_V[1]*t + coef_V[0];

        std::cout << "s=" << s << " | U: " << orig_U << " vs " << fit_U
                  << " | V: " << orig_V << " vs " << fit_V << "\n";
    }

    // 测试 OdrGeometry
    std::cout << "\n===== OdrGeometry 测试 =====\n";
    std::vector<common::Pose2d> poses = {
        common::Pose2d{100.0, 0.0, 0.0},
        common::Pose2d{320.0, 3.7, 0.0}
    };
    auto segments = common::OdrGeometry::GenerateFromPoses(poses);
    std::cout << "生成了 " << segments.size() << " 段曲线\n";

    return 0;
}
