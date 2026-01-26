#ifndef MAP_BASE_LANE_HPP
#define MAP_BASE_LANE_HPP

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace map_base {

class Lane {
public:
  using Point = std::pair<double, double>;

  enum class Type { DRIVING, STOP, BIKING, SIDEWALK, UNKNOWN };

  Lane() = default;
  Lane(std::int64_t id, std::string name = "", double width = 0.0, Type type = Type::UNKNOWN)
      : id_(id), name_(std::move(name)), width_(width), type_(type) {}

  // Accessors
  std::int64_t id() const { return id_; }
  const std::string &name() const { return name_; }
  double width() const { return width_; }
  const std::vector<Point> &centerline() const { return centerline_; }
  Type type() const { return type_; }

  // Mutators
  void setId(std::int64_t id) { id_ = id; }
  void setName(const std::string &name) { name_ = name; }
  void setWidth(double width) { width_ = width; }
  void setType(Type t) { type_ = t; }
  void addCenterlinePoint(const Point &p) { centerline_.push_back(p); }
  void clearCenterline() { centerline_.clear(); }

private:
  std::int64_t id_{0};
  std::string name_;
  double width_{0.0};
  std::vector<Point> centerline_;
  Type type_{Type::UNKNOWN};
};

} // namespace map_base

#endif // MAP_BASE_LANE_HPP
