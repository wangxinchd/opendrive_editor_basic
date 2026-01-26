#pragma once

#include <cmath>
#include <vector>
namespace common::geometry {

/* =========================
 * Basic types
 * ========================= */

struct Point {
  double x = 0.0;
  double y = 0.0;
};

struct Pose {
  double x = 0.0;
  double y = 0.0;
  double yaw = 0.0; // rad
};

/**
 * @brief Normalize angle into [lower, upper)
 *
 * The range size must be exactly 2π. If upper - lower != 2π,
 * the function returns the original angle unchanged.
 *
 * @param angle  Input angle (rad)
 * @param lower  Lower bound (inclusive)
 * @param upper  Upper bound (exclusive), must satisfy upper - lower = 2π
 */
double NormalizeAngle(double angle, double lower = 0, double upper = 2.0 * M_PI);

/* =========================
 * Coordinate transform
 * ========================= */

// Point
Point WorldToLocal(const Point &world, const Pose &ref);
Point LocalToWorld(const Point &local, const Pose &ref);

void WorldToLocal(const std::vector<Point> &world_pts, const Pose &ref,
                  std::vector<Point> *local_pts);

void LocalToWorld(const std::vector<Point> &local_pts, const Pose &ref,
                  std::vector<Point> *world_pts);

// Pose
Pose WorldToLocal(const Pose &world, const Pose &ref);
Pose LocalToWorld(const Pose &local, const Pose &ref);

} // namespace common::geometry
