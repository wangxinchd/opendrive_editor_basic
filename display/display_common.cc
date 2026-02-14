#include "common/point.h"
#include "display.h"
#include <type_traits>

using Point2d = common::Point2d;
using Point3d = common::Point3d;
using Pose2d = common::Pose2d;
using Pose3d = common::Pose3d;

namespace {
// 辅助函数：从不同类型中提取 x 坐标
template <typename T> double getXValue(const T &value) {
  if constexpr (std::is_same_v<T, double>) {
    return value;
  } else {
    return value.x;
  }
}

// 辅助函数：从不同类型中提取 y 坐标
// 对于 double 类型
template <typename T>
auto getYValue(const T &value)
    -> std::enable_if_t<std::is_same_v<T, double>, double> {
  return value;
}

// 对于有 pos.y 成员的类型（使用 SFINAE 检测）
template <typename T>
auto getYValue(const T &value) -> decltype(value.pos.y, double()) {
  return value.pos.y;
}

// 对于直接有 y 成员的类型（如 Point2d, Point3d, Pose2d, Pose3d）
template <typename T>
auto getYValue(const T &value) -> decltype(value.y, double()) {
  return value.y;
}
} // namespace

template <typename T>
double Display::convertXCoordinateUsingMapCenterPoint(const T &value) {
  const double x = getXValue(value);
  return (x - MapCenterX_ + WinXBias_) * Scale_ + WinWidth_ / 2.0;
}

// 显式实例化，确保链接时能找到这些特化版本
template double Display::convertXCoordinateUsingMapCenterPoint<Point2d>(
    const Point2d &);
template double Display::convertXCoordinateUsingMapCenterPoint<Point3d>(
    const Point3d &);
template double Display::convertXCoordinateUsingMapCenterPoint<double>(
    const double &);

template <typename T>
double Display::convertYCoordinateUsingMapCenterPoint(const T &value) {
  const double y = getYValue(value);
  return (-1.0 * (y - MapCenterY_) + WinYBias_) * Scale_ + WinHeight_ / 2.0;
}

// 显式实例化，确保链接时能找到这些特化版本
// 注意：这里使用 Point2d, Point3d, Pose2d, Pose3d 和 double
// 如果原来有 pose_t 和 position_t 类型，需要根据实际情况调整
template double Display::convertYCoordinateUsingMapCenterPoint<Point2d>(
    const Point2d &);
template double Display::convertYCoordinateUsingMapCenterPoint<Point3d>(
    const Point3d &);
template double Display::convertYCoordinateUsingMapCenterPoint<Pose2d>(
    const Pose2d &);
template double Display::convertYCoordinateUsingMapCenterPoint<Pose3d>(
    const Pose3d &);
template double Display::convertYCoordinateUsingMapCenterPoint<double>(
    const double &);

double
Display::ConvertScreenXToXodrPoint(const int32_t x)
{
    return (x - WinWidth_ / 2.0) / Scale_ - WinXBias_ + MapCenterX_;
}

double
Display::ConvertScreenYToXodrPoint(const int32_t y)
{
    return ((y - WinHeight_ / 2.0) / Scale_ - WinYBias_) / -1.0 +
    MapCenterY_;
}

// void Display::convertPointsCoordinateUsingMapCenterPoint(position_t
// *new_points,
//                                                          int32_t
//                                                          *new_point_num,
//                                                          const position_t
//                                                          *points, int32_t
//                                                          point_num) {
//   for (int32_t i = 0; i < point_num; i++) {
//     new_points[i].x = convertXCoordinateUsingMapCenterPoint(points[i]);
//     new_points[i].y = convertYCoordinateUsingMapCenterPoint(points[i]);
//   }
//   *new_point_num = point_num;
// }