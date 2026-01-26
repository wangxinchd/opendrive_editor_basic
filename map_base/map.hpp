#ifndef MAP_BASE_MAP_HPP
#define MAP_BASE_MAP_HPP

#include "road.hpp"

#include <memory>
#include <string>
#include <vector>

namespace map_base {

class Map {
public:
  Map() = default;
  explicit Map(std::string name) : name_(std::move(name)) {}

  const std::string &name() const { return name_; }
  void setName(const std::string &name) { name_ = name; }

  const std::vector<std::shared_ptr<Road>> &roads() const { return roads_; }
  void addRoad(const std::shared_ptr<Road> &road) { roads_.push_back(road); }

private:
  std::string name_;
  std::vector<std::shared_ptr<Road>> roads_;
};

} // namespace map_base

#endif // MAP_BASE_MAP_HPP
