#ifndef MAP_BASE_ROAD_BOUNDARY_HPP
#define MAP_BASE_ROAD_BOUNDARY_HPP

#include "common/point.h"

namespace map_base {

class RoadBoundary {
public:
  using Pose2d = common::Pose2d;

  struct RoadBoundaryId {
    std::int64_t road_id;
    std::int64_t road_boundary_id;

    RoadBoundaryId(std::int64_t road_id, std::int64_t road_boundary_id)
        : road_id(road_id), road_boundary_id(road_boundary_id) {}
    bool operator==(const RoadBoundaryId &other) const {
      return road_id == other.road_id &&
             road_boundary_id == other.road_boundary_id;
    }
    bool operator!=(const RoadBoundaryId &other) const {
      return !(*this == other);
    }
  };

  RoadBoundary() = default;

  const RoadBoundaryId &id() const { return id_; }
  void setId(const RoadBoundaryId &id) { id_ = id; }

  const std::vector<Pose2d> &poses() const { return poses_; }
  void setPoses(const std::vector<Pose2d> &poses) { poses_ = poses; }
  void addPose(const Pose2d &pose) { poses_.push_back(pose); }
  void clearPoses() { poses_.clear(); }

  // 前驱roadboundarys
  const std::vector<std::shared_ptr<RoadBoundary>>
      &predecessor_road_boundaries() const {
    return predecessor_road_boundaries_;
  }
  void addPredecessorRoadBoundary(
      const std::shared_ptr<RoadBoundary> &road_boundary) {
    predecessor_road_boundaries_.push_back(road_boundary);
  }
  void clearPredecessorRoadBoundaries() {
    predecessor_road_boundaries_.clear();
  }
  // 后继roadboundarys
  const std::vector<std::shared_ptr<RoadBoundary>> &successor_road_boundaries()
      const {
    return successor_road_boundaries_;
  }
  void addSuccessorRoadBoundary(
      const std::shared_ptr<RoadBoundary> &road_boundary) {
    successor_road_boundaries_.push_back(road_boundary);
  }
  void clearSuccessorRoadBoundaries() { successor_road_boundaries_.clear(); }

private:
  std::vector<Pose2d> poses_;
  RoadBoundaryId id_;
  std::vector<std::shared_ptr<RoadBoundary>> predecessor_road_boundaries_;
  std::vector<std::shared_ptr<RoadBoundary>> successor_road_boundaries_;
};
} // namespace map_base

#endif // MAP_BASE_ROAD_BOUNDARY_HPP