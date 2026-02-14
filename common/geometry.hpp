#pragma once

#include <cmath>
#include <vector>

#include "point.hpp"

namespace common::geometry {

using Point2d = common::Point2d;
using Pose2d = common::Pose2d;

/* =========================
 * Basic types
 * ========================= */

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
double NormalizeAngle(double angle, double lower = 0,
                      double upper = 2.0 * M_PI);

/* =========================
 * Coordinate transform
 * ========================= */

// Point2d
Point2d WorldToLocal(const Point2d &world, const Pose2d &ref);
Point2d LocalToWorld(const Point2d &local, const Pose2d &ref);

void WorldToLocal(const std::vector<Point2d> &world_pts, const Pose2d &ref,
                  std::vector<Point2d> *local_pts);

void LocalToWorld(const std::vector<Point2d> &local_pts, const Pose2d &ref,
                  std::vector<Point2d> *world_pts);

// Pose2d
Pose2d WorldToLocal(const Pose2d &world, const Pose2d &ref);
Pose2d LocalToWorld(const Pose2d &local, const Pose2d &ref);

} // namespace common::geometry
