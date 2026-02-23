#ifndef MAP_BASE_LANE_HPP
#define MAP_BASE_LANE_HPP

#include "common/point.h"
#include "lane_boundary.h"
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace map_base {

class Lane {
public:
  struct LaneId {
    std::int64_t road_id = 0;
    std::int64_t section_id = 0;
    std::int64_t lane_id = 0;

    LaneId() = default;
    LaneId(std::int64_t road_id, std::int64_t section_id, std::int64_t lane_id)
        : road_id(road_id), section_id(section_id), lane_id(lane_id) {}
    bool operator==(const LaneId &other) const {
      return road_id == other.road_id && section_id == other.section_id &&
             lane_id == other.lane_id;
    }
    bool operator!=(const LaneId &other) const { return !(*this == other); }
  };

public:
  using Pose2d = common::Pose2d;

  Lane() = default;
  explicit Lane(LaneId lane_id) : lane_id_(lane_id) {}

  const LaneId &id() const { return lane_id_; }
  void setId(const LaneId &lane_id) { lane_id_ = lane_id; }

  // 沿着车道长度s方向的宽度
  const std::vector<double> &width() const { return width_; }
  void setWidth(const std::vector<double> &width) { width_ = width; }
  void addWidth(double width) { width_.push_back(width); }
  void clearWidth() { width_.clear(); }
  void setLaneLength(double lane_length) { lane_length_ = lane_length; }
  double laneLength() const { return lane_length_; }

  // 沿着车道长度s方向的中心线
  const std::vector<Pose2d> &centerline() const { return centerline_; }
  void setCenterline(const std::vector<Pose2d> &centerline) {
    centerline_ = centerline;
  }
  void addCenterlinePoint(const Pose2d &p) { centerline_.push_back(p); }
  void clearCenterline() { centerline_.clear(); }

  // 左边界
  void setLeftLaneBoundary(
      const std::shared_ptr<LaneBoundary> &left_lane_boundary) {
    left_lane_boundary_ = left_lane_boundary;
  }
  const std::shared_ptr<LaneBoundary> &leftLaneBoundary() const {
    return left_lane_boundary_;
  }
  void clearLeftLaneBoundary() { left_lane_boundary_.reset(); }
  // 右边界
  const std::shared_ptr<LaneBoundary> &rightLaneBoundary() const {
    return right_lane_boundary_;
  }
  void setRightLaneBoundary(
      const std::shared_ptr<LaneBoundary> &right_lane_boundary) {
    right_lane_boundary_ = right_lane_boundary;
  }
  void clearRightLaneBoundary() { right_lane_boundary_.reset(); }

  // 前驱lanes
  const std::vector<std::shared_ptr<Lane>> &predecessor_lanes() const {
    return predecessor_lanes_;
  }
  void addPredecessorLane(const std::shared_ptr<Lane> &lane) {
    predecessor_lanes_.push_back(lane);
  }
  void clearPredecessorLanes() { predecessor_lanes_.clear(); }
  // 后继lanes
  const std::vector<std::shared_ptr<Lane>> &successor_lanes() const {
    return successor_lanes_;
  }
  void addSuccessorLane(const std::shared_ptr<Lane> &lane) {
    successor_lanes_.push_back(lane);
  }
  void clearSuccessorLanes() { successor_lanes_.clear(); }

private:
  LaneId lane_id_;

  // 沿着车道长度s方向的宽度
  std::vector<double> width_;
  double lane_length_;

  std::vector<Pose2d> centerline_;
  // 左边界
  std::shared_ptr<LaneBoundary> left_lane_boundary_;
  // 右边界
  std::shared_ptr<LaneBoundary> right_lane_boundary_;

  // 前驱lanes
  std::vector<std::shared_ptr<Lane>> predecessor_lanes_;
  // 后继lanes
  std::vector<std::shared_ptr<Lane>> successor_lanes_;
};

} // namespace map_base

#endif // MAP_BASE_LANE_HPP
