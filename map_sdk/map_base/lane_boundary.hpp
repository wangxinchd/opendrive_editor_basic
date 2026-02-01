#ifndef MAP_BASE_LANE_BOUNDARY_HPP
#define MAP_BASE_LANE_BOUNDARY_HPP

#include "common/point.hpp"
#include <memory>
#include <vector>

namespace map_base {

class LaneBoundary {
public:
  LaneBoundary() = default;
  LaneBoundary(const std::vector<Point2d> &points) : points_(points) {}

  const std::vector<Point2d> &points() const { return points_; }
  void setPoints(const std::vector<Point2d> &points) { points_ = points; }

  // 前驱laneboundarys
  const std::vector<std::shared_ptr<LaneBoundary>>
      &predecessor_lane_boundaries() const {
    return predecessor_lane_boundaries_;
  }
  void addPredecessorLaneBoundary(
      const std::shared_ptr<LaneBoundary> &lane_boundary) {
    predecessor_lane_boundaries_.push_back(lane_boundary);
  }
  void clearPredecessorLaneBoundaries() {
    predecessor_lane_boundaries_.clear();
  }
  // 后继laneboundarys
  const std::vector<std::shared_ptr<LaneBoundary>> &successor_lane_boundaries()
      const {
    return successor_lane_boundaries_;
  }
  void addSuccessorLaneBoundary(
      const std::shared_ptr<LaneBoundary> &lane_boundary) {
    successor_lane_boundaries_.push_back(lane_boundary);
  }
  void clearSuccessorLaneBoundaries() { successor_lane_boundaries_.clear(); }

private:
  std::vector<Point2d> points_;

  // 前驱laneboundarys
  std::vector<std::shared_ptr<LaneBoundary>> predecessor_lane_boundaries_;
  // 后继laneboundarys
  std::vector<std::shared_ptr<LaneBoundary>> successor_lane_boundaries_;
};
} // namespace map_base

#endif // MAP_BASE_LANE_BOUNDARY_HPP