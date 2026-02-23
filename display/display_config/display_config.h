#ifndef DISPLAY_CONFIG_H
#define DISPLAY_CONFIG_H

#include <QColor>
#include <QPen>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QJsonArray>

struct DisplayConfig {
  // 显示元素开关
  struct ElementVisibility {
    bool show_lane_centerline = true;      // 车道中心线
    bool show_lane_left_boundary = true;   // 左边界
    bool show_lane_right_boundary = true;   // 右边界
    bool show_road = false;                // 道路
    bool show_intersection = false;        // 交叉口
    bool show_lane_id = false;             // 车道ID标签
    bool show_road_id = false;             // 道路ID标签
  } visibility;

  // 颜色配置
  struct ColorConfig {
    QColor lane_centerline_color = Qt::darkGray;
    QColor lane_left_boundary_color = Qt::black;
    QColor lane_right_boundary_color = Qt::black;
    QColor road_color = Qt::blue;
    QColor intersection_color = Qt::red;
    QColor background_color = Qt::white;
  } colors;

  // 线宽配置
  struct LineWidthConfig {
    double lane_centerline_width = 3.0;
    double lane_boundary_width = 4.0;
    double road_width = 2.0;
    double intersection_width = 2.0;
  } line_widths;

  // 线型配置
  struct LineStyleConfig {
    Qt::PenStyle lane_centerline_style = Qt::DotLine;
    Qt::PenStyle lane_boundary_style = Qt::SolidLine;
    Qt::PenStyle road_style = Qt::SolidLine;
  } line_styles;

  // 其他配置
  bool antialiasing = true;                // 抗锯齿

  // 序列化到JSON
  QJsonObject toJson() const;
  
  // 从JSON反序列化
  void fromJson(const QJsonObject& json);
  
  // 保存到文件
  bool saveToFile(const QString& filename) const;
  
  // 从文件加载
  bool loadFromFile(const QString& filename);
  
  // 获取默认配置
  static DisplayConfig defaultConfig();
};

#endif // DISPLAY_CONFIG_H

