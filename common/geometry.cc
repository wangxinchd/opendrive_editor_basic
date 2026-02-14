#include "geometry.h"
#include "point.h"

namespace common::geometry {

/* =========================
 * Angle utility
 * ========================= */

double NormalizeAngle(double angle, double lower, double upper) {
  constexpr double kTwoPi = 2.0 * M_PI;
  const double range = upper - lower;

  // 确保范围大小正好是 2π
  if (std::abs(range - kTwoPi) > 1e-10) {
    return angle; // 范围大小不是 2π，原样返回
  }

  double a = std::fmod(angle - lower, kTwoPi);
  if (a < 0.0) {
    a += kTwoPi;
  }
  return a + lower;
}

/* =========================
 * Point2d transform
 * ========================= */

Point2d WorldToLocal(const Point2d &world, const Pose2d &ref) {
  const double cos_yaw = std::cos(ref.yaw);
  const double sin_yaw = std::sin(ref.yaw);
  const double dx = world.x - ref.x;
  const double dy = world.y - ref.y;

  return {dx * cos_yaw + dy * sin_yaw, -dx * sin_yaw + dy * cos_yaw};
}

Point2d LocalToWorld(const Point2d &local, const Pose2d &ref) {
  const double cos_yaw = std::cos(ref.yaw);
  const double sin_yaw = std::sin(ref.yaw);
  return {ref.x + local.x * cos_yaw - local.y * sin_yaw,
          ref.y + local.x * sin_yaw + local.y * cos_yaw};
}

void WorldToLocal(const std::vector<Point2d> &world_pts, const Pose2d &ref,
                  std::vector<Point2d> *local_pts) {
  local_pts->clear();
  local_pts->reserve(world_pts.size());

  const double cos_yaw = std::cos(ref.yaw);
  const double sin_yaw = std::sin(ref.yaw);
  for (const auto &w : world_pts) {
    const double dx = w.x - ref.x;
    const double dy = w.y - ref.y;
    local_pts->push_back(
        {dx * cos_yaw + dy * sin_yaw, -dx * sin_yaw + dy * cos_yaw});
  }
}

void LocalToWorld(const std::vector<Point2d> &local_pts, const Pose2d &ref,
                  std::vector<Point2d> *world_pts) {
  world_pts->clear();
  world_pts->reserve(local_pts.size());

  const double cos_yaw = std::cos(ref.yaw);
  const double sin_yaw = std::sin(ref.yaw);
  for (const auto &l : local_pts) {
    world_pts->push_back({ref.x + l.x * cos_yaw - l.y * sin_yaw,
                          ref.y + l.x * sin_yaw + l.y * cos_yaw});
  }
}

/* =========================
 * Pose2d transform
 * ========================= */

Pose2d WorldToLocal(const Pose2d &world, const Pose2d &ref) {
  const double cos_yaw = std::cos(ref.yaw);
  const double sin_yaw = std::sin(ref.yaw);
  const double dx = world.x - ref.x;
  const double dy = world.y - ref.y;

  Pose2d out;
  out.x = dx * cos_yaw + dy * sin_yaw;
  out.y = -dx * sin_yaw + dy * cos_yaw;
  out.yaw = NormalizeAngle(world.yaw - ref.yaw); // 默认 [0, 2pi)
  return out;
}

Pose2d LocalToWorld(const Pose2d &local, const Pose2d &ref) {
  const double cos_yaw = std::cos(ref.yaw);
  const double sin_yaw = std::sin(ref.yaw);

  Pose2d out;
  out.x = ref.x + local.x * cos_yaw - local.y * sin_yaw;
  out.y = ref.y + local.x * sin_yaw + local.y * cos_yaw;
  out.yaw = NormalizeAngle(local.yaw + ref.yaw);
  return out;
}

} // namespace common::geometry
