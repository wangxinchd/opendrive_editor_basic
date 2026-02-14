#include "odr_geometry.h"
#include <cmath>
#include <algorithm>
#include "cubic_curve_fitter.h"

namespace common {

std::vector<OpenDriveParamPoly3> OdrGeometry::GenerateFromPoses(
    const std::vector<Pose2d>& poses
) {
  std::vector<OpenDriveParamPoly3> result;

  if (poses.size() < 2) {
    return result;  // 需要至少2个姿态点
  }

  // 分段拟合：每对相邻姿态点之间用三次 Hermite 插值
  for (size_t i = 0; i < poses.size() - 1; i++) {
    const auto& p1 = poses[i];
    const auto& p2 = poses[i + 1];

    // 创建三次曲线拟合器
    CubicCurveFitter curve(p1.x, p1.y, p1.yaw, p2.x, p2.y, p2.yaw);

    // 弧长重参数化
    std::vector<double> s_vals;
    std::vector<CubicCurveFitter::Vec2> uv_vals;
    double total_length;

    curve.reparameterizeArcLength(s_vals, uv_vals, total_length);

    // 提取 U 和 V 分量
    auto u_vals = CubicCurveFitter::extractU(uv_vals);
    auto v_vals = CubicCurveFitter::extractV(uv_vals);

    // 拟合三次多项式
    auto coef_U = CubicCurveFitter::fitCubic(s_vals, u_vals);
    auto coef_V = CubicCurveFitter::fitCubic(s_vals, v_vals);

    // 设置 OpenDRIVE ParamPoly3 系数
    OpenDriveParamPoly3 param_poly;
    param_poly.aU = coef_U[3];  // a (p^3)
    param_poly.bU = coef_U[2];  // b (p^2)
    param_poly.cU = coef_U[1];  // c (p^1)
    param_poly.dU = coef_U[0];  // d (p^0)

    param_poly.aV = coef_V[3];  // a (p^3)
    param_poly.bV = coef_V[2];  // b (p^2)
    param_poly.cV = coef_V[1];  // c (p^1)
    param_poly.dV = coef_V[0];  // d (p^0)

    result.push_back(param_poly);
  }

  return result;
}

} // namespace common
