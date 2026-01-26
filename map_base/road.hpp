#ifndef MAP_BASE_ROAD_HPP
#define MAP_BASE_ROAD_HPP

#include "lane.hpp"

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

namespace map_base {

class Road {
public:
  using Point = Lane::Point;

  Road() = default;
  Road(std::int64_t id, std::string name = "") : id_(id), name_(std::move(name)) {}

  // Accessors
  std::int64_t id() const { return id_; }
  const std::string &name() const { return name_; }
  const std::vector<Lane> &lanes() const { return lanes_; }
  const std::vector<Point> &referenceLine() const { return reference_line_; }

  // Mutators
  void setId(std::int64_t id) { id_ = id; }
  void setName(const std::string &name) { name_ = name; }
  void addLane(const Lane &lane) { lanes_.push_back(lane); }
  bool removeLaneById(std::int64_t lane_id) {
    auto it = std::remove_if(lanes_.begin(), lanes_.end(),
                             [lane_id](const Lane &l) { return l.id() == lane_id; });
    if (it == lanes_.end())
      return false;
    lanes_.erase(it, lanes_.end());
    return true;
  }
  Lane *findLaneById(std::int64_t lane_id) {
    for (auto &l : lanes_) {
      if (l.id() == lane_id)
        return &l;
    }
    return nullptr;
  }
  void addReferencePoint(const Point &p) { reference_line_.push_back(p); }
  void clearReferenceLine() { reference_line_.clear(); }

private:
  std::int64_t id_{0};
  std::string name_;
  std::vector<Lane> lanes_;
  std::vector<Point> reference_line_;
};

} // namespace map_base

#endif // MAP_BASE_ROAD_HPP
