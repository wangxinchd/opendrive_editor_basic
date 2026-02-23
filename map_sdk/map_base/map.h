#ifndef MAP_BASE_MAP_HPP
#define MAP_BASE_MAP_HPP

#include "intersection.h"
#include "lane.h"
#include "road.h"
#include <memory>
#include <string>
#include <vector>

namespace map_base {

class Map {

public:
  struct Metadata {
    double center_x = 0.0;
    double center_y = 0.0;
    double center_z = 0.0;

    double x_bias = 0.0;
    double y_bias = 0.0;
    double z_bias = 0.0;

    double min_x = 0.0;
    double max_x = 0.0;
    double min_y = 0.0;
    double max_y = 0.0;
    double min_z = 0.0;
    double max_z = 0.0;

    // init metadata
    Metadata() {
      center_x = 0.0;
      center_y = 0.0;
      center_z = 0.0;
      x_bias = 0.0;
      y_bias = 0.0;
      z_bias = 0.0;

      // 最大浮点数
      min_x = std::numeric_limits<double>::max();
      max_x = std::numeric_limits<double>::min();
      min_y = std::numeric_limits<double>::max();
      max_y = std::numeric_limits<double>::min();
      min_z = std::numeric_limits<double>::max();
      max_z = std::numeric_limits<double>::min();
    }
  };

public:
  Map() = default;
  explicit Map(std::string name) : name_(std::move(name)) {}

  const std::string &name() const { return name_; }
  void setName(const std::string &name) { name_ = name; }

  const Metadata &metadata() const { return metadata_; }
  void setMetadata(const Metadata &metadata) { metadata_ = metadata; }

  const std::vector<std::shared_ptr<Road>> &roads() const { return roads_; }
  void addRoad(const std::shared_ptr<Road> &road) { roads_.push_back(road); }

  const std::vector<std::shared_ptr<Intersection>> &intersections() const {
    return intersections_;
  }
  void addIntersection(const std::shared_ptr<Intersection> &intersection) {
    intersections_.push_back(intersection);
  }

  const std::vector<std::shared_ptr<Lane>> &lanes() const { return lanes_; }
  void addLane(const std::shared_ptr<Lane> &lane) { lanes_.push_back(lane); }

  void clear() {
    roads_.clear();
    intersections_.clear();
    lanes_.clear();
  }

private:
  std::string name_;
  Metadata metadata_;
  std::vector<std::shared_ptr<Road>> roads_;
  std::vector<std::shared_ptr<Intersection>> intersections_;
  std::vector<std::shared_ptr<Lane>> lanes_;
};

} // namespace map_base

#endif // MAP_BASE_MAP_HPP
