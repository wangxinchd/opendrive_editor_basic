#pragma once

#include <cmath>

namespace common {

struct Point2d {
  double x = 0.0;
  double y = 0.0;
};

struct Point3d {
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;
};

struct Pose2d {
  double x = 0.0;
  double y = 0.0;
  double yaw = 0.0; // rad

  Pose2d() = default;
  Pose2d(double x, double y, double yaw) : x(x), y(y), yaw(yaw) {}
};

struct Pose3d {
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;
  double yaw = 0.0;   // rad
  double pitch = 0.0; // rad
  double roll = 0.0;  // rad
};

} // namespace common
