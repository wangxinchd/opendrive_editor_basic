#pragma once

#include <vector>
#include "geometry.h"
#include "point.h"

namespace common {

/**
 * @brief OpenDRIVE paramPoly3 representation
 *
 * u(p) = aU + bU*p + cU*p^2 + dU*p^3
 * v(p) = aV + bV*p + cV*p^2 + dV*p^3
 * p in [0, 1]
 */
struct OpenDriveParamPoly3 {
  double aU = 0.0;  // constant term
  double bU = 0.0;  // linear term
  double cU = 0.0;  // quadratic term
  double dU = 0.0;  // cubic term

  double aV = 0.0;  // constant term
  double bV = 0.0;  // linear term
  double cV = 0.0;  // quadratic term
  double dV = 0.0;  // cubic term
};

/**
 * @brief OpenDRIVE 几何线形生成器
 * 
 * 根据控制点生成分段的三次 Hermite 插值曲线（OpenDRIVE ParamPoly3）
 */
class OdrGeometry {
public:
  /**
   * @brief 使用分段三次 Hermite 插值生成几何线形
   * @param poses 姿态序列 (x, y, yaw)
   * @return 各段的 OpenDriveParamPoly3 系数
   * 
   * n 个姿态 → n-1 段曲线
   */
  static std::vector<OpenDriveParamPoly3> GenerateFromPoses(
      const std::vector<Pose2d>& poses
  );

};

} // namespace common
