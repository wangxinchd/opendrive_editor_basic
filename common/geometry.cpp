#include "common/geometry.hpp"

#include <cmath>
#include <numbers>

namespace common::geometry {

/* =========================
 * Angle utility
 * ========================= */

double NormalizeAngle(double angle,
                      double lower,
                      double upper) {
    if (!(upper > lower)) {
        return angle;  // 防御：区间非法，原样返回
    }

    const double range = upper - lower;

    double a = std::fmod(angle - lower, range);
    if (a < 0.0) {
        a += range;
    }
    return a + lower;
}

double NormalizeAngle(double angle) {
    constexpr double kTwoPi = 2.0 * std::numbers::pi;
    return NormalizeAngle(angle, 0.0, kTwoPi);
}

/* =========================
 * Internal helpers
 * ========================= */

namespace {

struct Rotation2d {
    double c;
    double s;
};

inline Rotation2d MakeRotation(double yaw) {
    return { std::cos(yaw), std::sin(yaw) };
}

}  // anonymous namespace

/* =========================
 * Point transform
 * ========================= */

Point WorldToLocal(const Point& world, const Pose& ref) {
    const auto r = MakeRotation(ref.yaw);
    const double dx = world.x - ref.x;
    const double dy = world.y - ref.y;

    return {
        dx * r.c + dy * r.s,
       -dx * r.s + dy * r.c
    };
}

Point LocalToWorld(const Point& local, const Pose& ref) {
    const auto r = MakeRotation(ref.yaw);
    return {
        ref.x + local.x * r.c - local.y * r.s,
        ref.y + local.x * r.s + local.y * r.c
    };
}

void WorldToLocal(const std::vector<Point>& world_pts,
                  const Pose& ref,
                  std::vector<Point>* local_pts) {
    local_pts->clear();
    local_pts->reserve(world_pts.size());

    const auto r = MakeRotation(ref.yaw);
    for (const auto& w : world_pts) {
        const double dx = w.x - ref.x;
        const double dy = w.y - ref.y;
        local_pts->push_back({
            dx * r.c + dy * r.s,
           -dx * r.s + dy * r.c
        });
    }
}

void LocalToWorld(const std::vector<Point>& local_pts,
                  const Pose& ref,
                  std::vector<Point>* world_pts) {
    world_pts->clear();
    world_pts->reserve(local_pts.size());

    const auto r = MakeRotation(ref.yaw);
    for (const auto& l : local_pts) {
        world_pts->push_back({
            ref.x + l.x * r.c - l.y * r.s,
            ref.y + l.x * r.s + l.y * r.c
        });
    }
}

/* =========================
 * Pose transform
 * ========================= */

Pose WorldToLocal(const Pose& world, const Pose& ref) {
    const auto r = MakeRotation(ref.yaw);
    const double dx = world.x - ref.x;
    const double dy = world.y - ref.y;

    Pose out;
    out.x   = dx * r.c + dy * r.s;
    out.y   = -dx * r.s + dy * r.c;
    out.yaw = NormalizeAngle(world.yaw - ref.yaw);  // 默认 [0, 2pi)
    return out;
}

Pose LocalToWorld(const Pose& local, const Pose& ref) {
    const auto r = MakeRotation(ref.yaw);

    Pose out;
    out.x   = ref.x + local.x * r.c - local.y * r.s;
    out.y   = ref.y + local.x * r.s + local.y * r.c;
    out.yaw = NormalizeAngle(local.yaw + ref.yaw);
    return out;
}

}  // namespace common::geometry
