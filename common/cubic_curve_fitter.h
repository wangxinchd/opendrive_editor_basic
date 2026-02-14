#pragma once
#include <array>
#include <vector>
#include <cmath>
#include <ostream>

/**
 * @brief 三次曲线拟合器
 * 
 * 根据两个控制点（含方向角）拟合三次 Hermite 插值曲线
 */
class CubicCurveFitter {
public:
    using Vec2 = std::array<double,2>;

    CubicCurveFitter(double x0, double y0, double yaw0,
                    double x1, double y1, double yaw1);

    // world cubic coefficients
    Vec2 a() const { return a_; }
    Vec2 b() const { return b_; }
    Vec2 c() const { return c_; }
    Vec2 d() const { return d_; }

    // evaluate curve at t=0~1
    Vec2 evaluate(double t) const;

    // 重参数化得到 arcLength 与局部 U,V
    void reparameterizeArcLength(std::vector<double>& s_vals,
                                 std::vector<Vec2>& uv_vals,
                                 double& total_length,
                                 int n=2000) const;

    // 转局部坐标系 (U,V)
    static Vec2 worldToLocal(double x, double y,
                             double x0, double y0,
                             double hdg);

    // cubic 拟合 paramPoly3 (返回系数 [a,b,c,d] 对应 a*t^3 + b*t^2 + c*t + d)
    // s_norm: 归一化后的弧长参数 [0,1]
    static std::array<double,4> fitCubic(const std::vector<double>& s_norm,
                                         const std::vector<double>& vals);

    // 从 uv_vals 中提取 U 分量
    static std::vector<double> extractU(const std::vector<Vec2>& uv_vals);

    // 从 uv_vals 中提取 V 分量
    static std::vector<double> extractV(const std::vector<Vec2>& uv_vals);

    // 输出 OpenDRIVE paramPoly3 格式
    void printOpenDRIVE(std::ostream& os,
                        const std::array<double,4>& coef_U,
                        const std::array<double,4>& coef_V,
                        double total_length) const;

private:
    Vec2 a_, b_, c_, d_;
    double x0_, y0_;
    double yaw0_;
};
