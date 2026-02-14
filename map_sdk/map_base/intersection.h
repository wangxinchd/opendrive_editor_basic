#ifndef MAP_BASE_INTERSECTION_HPP
#define MAP_BASE_INTERSECTION_HPP

#include "lane.h"
#include "road.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace map_base {

// Intersection describes an intersection area and the roads/lanes that enter,
// exit and exist inside the intersection. The polygon defines the intersection
// boundary.
class Intersection {
public:
  using Point2d = common::Point2d;

  Intersection() = default;
  Intersection(std::int64_t id, std::string name = "")
      : id_(id), name_(std::move(name)) {}

  std::int64_t id() const { return id_; }
  const std::string &name() const { return name_; }
  void setName(const std::string &name) { name_ = name; }

  const std::vector<std::shared_ptr<Road>> &incoming() const {
    return incoming_roads_;
  }
  const std::vector<std::shared_ptr<Road>> &outgoing() const {
    return outgoing_roads_;
  }
  const std::vector<std::shared_ptr<Road>> &internal() const {
    return internal_roads_;
  }

  const std::vector<std::shared_ptr<Lane>> &incoming_lanes() const {
    return incoming_lanes_;
  }
  const std::vector<std::shared_ptr<Lane>> &outgoing_lanes() const {
    return outgoing_lanes_;
  }
  const std::vector<std::shared_ptr<Lane>> &internal_lanes() const {
    return internal_lanes_;
  }

  void addIncoming(const std::shared_ptr<Road> &road,
                   const std::shared_ptr<Lane> &lane) {
    incoming_roads_.push_back(road);
    incoming_lanes_.push_back(lane);
  }
  void addOutgoing(const std::shared_ptr<Road> &road,
                   const std::shared_ptr<Lane> &lane) {
    outgoing_roads_.push_back(road);
    outgoing_lanes_.push_back(lane);
  }
  void addInternal(const std::shared_ptr<Road> &road,
                   const std::shared_ptr<Lane> &lane) {
    internal_roads_.push_back(road);
    internal_lanes_.push_back(lane);
  }

  // Polygon defining the intersection boundary (sequence of (x,y) points).
  const std::vector<Point2d> &polygon() const { return polygon_; }
  void setPolygon(const std::vector<Point2d> &poly) { polygon_ = poly; }
  void addPolygonPoint(const Point2d &p) { polygon_.push_back(p); }
  void clearPolygon() { polygon_.clear(); }

private:
  std::int64_t id_{0};
  std::string name_;
  std::vector<std::shared_ptr<Road>> incoming_roads_;
  std::vector<std::shared_ptr<Road>> outgoing_roads_;
  std::vector<std::shared_ptr<Road>> internal_roads_;
  std::vector<Point2d> polygon_;
  std::vector<std::shared_ptr<Lane>> incoming_lanes_;
  std::vector<std::shared_ptr<Lane>> outgoing_lanes_;
  std::vector<std::shared_ptr<Lane>> internal_lanes_;
};

} // namespace map_base

#endif // MAP_BASE_INTERSECTION_HPP
