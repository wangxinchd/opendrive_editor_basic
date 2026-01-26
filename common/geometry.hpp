#pragma once

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
    double x   = 0.0;
    double y   = 0.0;
    double yaw = 0.0;  // rad
};

/* =========================
 * Angle utility
 * ========================= */

/**
 * @brief Normalize angle into [lower, upper)
 *
 * @param angle  Input angle (rad)
 * @param lower  Lower bound (inclusive)
 * @param upper  Upper bound (exclusive), must be > lower
 */
double NormalizeAngle(double angle,
                      double lower,
                      double upper);

/**
 * @brief Normalize angle into [0, 2*pi)
 */
double NormalizeAngle(double angle);

/* =========================
 * Coordinate transform
 * ========================= */

// Point
Point WorldToLocal(const Point& world, const Pose& ref);
Point LocalToWorld(const Point& local, const Pose& ref);

void WorldToLocal(const std::vector<Point>& world_pts,
                  const Pose& ref,
                  std::vector<Point>* local_pts);

void LocalToWorld(const std::vector<Point>& local_pts,
                  const Pose& ref,
                  std::vector<Point>* world_pts);

// Pose
Pose WorldToLocal(const Pose& world, const Pose& ref);
Pose LocalToWorld(const Pose& local, const Pose& ref);

}  // namespace common::geometry
