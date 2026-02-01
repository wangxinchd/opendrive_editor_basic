#ifndef MAP_BASE_LANE_HPP
#define MAP_BASE_LANE_HPP

#include "common/point.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace map_base {

class Lane {
public:
  struct LaneId {
    std::int64_t road_id;
    std::int64_t section_id;
    std::int64_t lane_id;
    LaneId(std::int64_t road_id, std::int64_t section_id, std::int64_t lane_id)
        : road_id(road_id), section_id(section_id), lane_id(lane_id) {}
    bool operator==(const LaneId &other) const {
      return road_id == other.road_id && section_id == other.section_id &&
             lane_id == other.lane_id;
    }
    bool operator!=(const LaneId &other) const { return !(*this == other); }
  };

public:
  using Point2d = common::Point2d;

  Lane() = default;
  Lane(LaneId lane_id) : lane_id_(lane_id) {}

  const LaneId &id() const { return lane_id_; }
  void setId(const LaneId &lane_id) { lane_id_ = lane_id; }

  // 沿着车道长度s方向的宽度
  const std::vector<double> &width() const { return width_; }
  void setWidth(const std::vector<double> &width) { width_ = width; }
  void addWidth(double width) { width_.push_back(width); }
  void clearWidth() { width_.clear(); }

  // 沿着车道长度s方向的中心线
  const std::vector<Point2d> &centerline() const { return centerline_; }
  void setCenterline(const std::vector<Point2d> &centerline) {
    centerline_ = centerline;
  }
  void addCenterlinePoint(const Point2d &p) { centerline_.push_back(p); }
  void clearCenterline() { centerline_.clear(); }

  // 左边界
  const std::vector<Point2d> &left_boundary() const { return left_boundary_; }
  void setLeftBoundary(const std::vector<Point2d> &left_boundary) {
    left_boundary_ = left_boundary;
  }
  void addLeftBoundaryPoint(const Point2d &p) { left_boundary_.push_back(p); }

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

  std::vector<Point2d> centerline_;
  // 左边界
  std::vector<Point2d> left_boundary_;
  // 右边界
  std::vector<Point2d> right_boundary_;

  // 前驱lanes
  std::vector<std::shared_ptr<Lane>> predecessor_lanes_;
  // 后继lanes
  std::vector<std::shared_ptr<Lane>> successor_lanes_;
};

} // namespace map_base

#endif // MAP_BASE_LANE_HPP
