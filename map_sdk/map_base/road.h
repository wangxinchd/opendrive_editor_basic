#ifndef MAP_BASE_ROAD_HPP
#define MAP_BASE_ROAD_HPP

#include "lane.h"

#include "common/point.h"
#include "road_boundary.h"
#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

namespace map_base {

class Road {
public:
  using Point2d = common::Point2d;

  Road() = default;
  Road(std::int64_t id, std::string name = "")
      : id_(id), name_(std::move(name)) {}

  // Accessors
  std::int64_t id() const { return id_; }
  const std::string &name() const { return name_; }
  const std::vector<std::shared_ptr<Lane>> &lanes() const { return lane_ptrs_; }
  const std::vector<Point2d> &referenceLine() const { return reference_line_; }

  // Mutators
  void setId(std::int64_t id) { id_ = id; }
  void setName(const std::string &name) { name_ = name; }
  void addLane(const std::shared_ptr<Lane> &lane) {
    lane_ptrs_.push_back(lane);
  }

  // 道路边界
  const std::vector<std::shared_ptr<RoadBoundary>> &leftRoadBoundaries() const {
    return left_road_boundary_ptrs_;
  }
  const std::vector<std::shared_ptr<RoadBoundary>> &rightRoadBoundaries()
      const {
    return right_road_boundary_ptrs_;
  }
  void addLeftRoadBoundary(const std::shared_ptr<RoadBoundary> &road_boundary) {
    left_road_boundary_ptrs_.push_back(road_boundary);
  }
  void addRightRoadBoundary(
      const std::shared_ptr<RoadBoundary> &road_boundary) {
    right_road_boundary_ptrs_.push_back(road_boundary);
  }

  // 前驱道路
  const std::vector<std::shared_ptr<Road>> &predecessorRoads() const {
    return predecessor_roads_;
  }
  void addPredecessorRoad(const std::shared_ptr<Road> &road) {
    predecessor_roads_.push_back(road);
  }
  // 后继道路
  const std::vector<std::shared_ptr<Road>> &successorRoads() const {
    return successor_roads_;
  }
  void addSuccessorRoad(const std::shared_ptr<Road> &road) {
    successor_roads_.push_back(road);
  }

private:
  std::int64_t id_{0};
  std::string name_;
  std::vector<std::shared_ptr<Lane>> lane_ptrs_;

  std::vector<Point2d> reference_line_;
  // 道路边界
  // 左侧边界
  std::vector<std::shared_ptr<RoadBoundary>> left_road_boundary_ptrs_;
  // 右侧边界
  std::vector<std::shared_ptr<RoadBoundary>> right_road_boundary_ptrs_;

  // 前驱roads
  std::vector<std::shared_ptr<Road>> predecessor_roads_;
  // 后继roads
  std::vector<std::shared_ptr<Road>> successor_roads_;
};

} // namespace map_base

#endif // MAP_BASE_ROAD_HPP
