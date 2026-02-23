#include "display_config.h"
#include <QDebug>

QJsonObject DisplayConfig::toJson() const {
  QJsonObject json;
  
  // 显示元素开关
  QJsonObject visibilityObj;
  visibilityObj["show_lane_centerline"] = visibility.show_lane_centerline;
  visibilityObj["show_lane_left_boundary"] = visibility.show_lane_left_boundary;
  visibilityObj["show_lane_right_boundary"] = visibility.show_lane_right_boundary;
  visibilityObj["show_road"] = visibility.show_road;
  visibilityObj["show_intersection"] = visibility.show_intersection;
  visibilityObj["show_lane_id"] = visibility.show_lane_id;
  visibilityObj["show_road_id"] = visibility.show_road_id;
  json["visibility"] = visibilityObj;
  
  // 颜色配置
  QJsonObject colorsObj;
  colorsObj["lane_centerline_color"] = colors.lane_centerline_color.name();
  colorsObj["lane_left_boundary_color"] = colors.lane_left_boundary_color.name();
  colorsObj["lane_right_boundary_color"] = colors.lane_right_boundary_color.name();
  colorsObj["road_color"] = colors.road_color.name();
  colorsObj["intersection_color"] = colors.intersection_color.name();
  colorsObj["background_color"] = colors.background_color.name();
  json["colors"] = colorsObj;
  
  // 线宽配置
  QJsonObject lineWidthsObj;
  lineWidthsObj["lane_centerline_width"] = line_widths.lane_centerline_width;
  lineWidthsObj["lane_boundary_width"] = line_widths.lane_boundary_width;
  lineWidthsObj["road_width"] = line_widths.road_width;
  lineWidthsObj["intersection_width"] = line_widths.intersection_width;
  json["line_widths"] = lineWidthsObj;
  
  // 线型配置
  QJsonObject lineStylesObj;
  lineStylesObj["lane_centerline_style"] = static_cast<int>(line_styles.lane_centerline_style);
  lineStylesObj["lane_boundary_style"] = static_cast<int>(line_styles.lane_boundary_style);
  lineStylesObj["road_style"] = static_cast<int>(line_styles.road_style);
  json["line_styles"] = lineStylesObj;
  
  // 其他配置
  json["antialiasing"] = antialiasing;
  
  return json;
}

void DisplayConfig::fromJson(const QJsonObject& json) {
  // 显示元素开关
  if (json.contains("visibility") && json["visibility"].isObject()) {
    QJsonObject visibilityObj = json["visibility"].toObject();
    if (visibilityObj.contains("show_lane_centerline"))
      visibility.show_lane_centerline = visibilityObj["show_lane_centerline"].toBool();
    if (visibilityObj.contains("show_lane_left_boundary"))
      visibility.show_lane_left_boundary = visibilityObj["show_lane_left_boundary"].toBool();
    if (visibilityObj.contains("show_lane_right_boundary"))
      visibility.show_lane_right_boundary = visibilityObj["show_lane_right_boundary"].toBool();
    if (visibilityObj.contains("show_road"))
      visibility.show_road = visibilityObj["show_road"].toBool();
    if (visibilityObj.contains("show_intersection"))
      visibility.show_intersection = visibilityObj["show_intersection"].toBool();
    if (visibilityObj.contains("show_lane_id"))
      visibility.show_lane_id = visibilityObj["show_lane_id"].toBool();
    if (visibilityObj.contains("show_road_id"))
      visibility.show_road_id = visibilityObj["show_road_id"].toBool();
  }
  
  // 颜色配置
  if (json.contains("colors") && json["colors"].isObject()) {
    QJsonObject colorsObj = json["colors"].toObject();
    if (colorsObj.contains("lane_centerline_color"))
      colors.lane_centerline_color = QColor(colorsObj["lane_centerline_color"].toString());
    if (colorsObj.contains("lane_left_boundary_color"))
      colors.lane_left_boundary_color = QColor(colorsObj["lane_left_boundary_color"].toString());
    if (colorsObj.contains("lane_right_boundary_color"))
      colors.lane_right_boundary_color = QColor(colorsObj["lane_right_boundary_color"].toString());
    if (colorsObj.contains("road_color"))
      colors.road_color = QColor(colorsObj["road_color"].toString());
    if (colorsObj.contains("intersection_color"))
      colors.intersection_color = QColor(colorsObj["intersection_color"].toString());
    if (colorsObj.contains("background_color"))
      colors.background_color = QColor(colorsObj["background_color"].toString());
  }
  
  // 线宽配置
  if (json.contains("line_widths") && json["line_widths"].isObject()) {
    QJsonObject lineWidthsObj = json["line_widths"].toObject();
    if (lineWidthsObj.contains("lane_centerline_width"))
      line_widths.lane_centerline_width = lineWidthsObj["lane_centerline_width"].toDouble();
    if (lineWidthsObj.contains("lane_boundary_width"))
      line_widths.lane_boundary_width = lineWidthsObj["lane_boundary_width"].toDouble();
    if (lineWidthsObj.contains("road_width"))
      line_widths.road_width = lineWidthsObj["road_width"].toDouble();
    if (lineWidthsObj.contains("intersection_width"))
      line_widths.intersection_width = lineWidthsObj["intersection_width"].toDouble();
  }
  
  // 线型配置
  if (json.contains("line_styles") && json["line_styles"].isObject()) {
    QJsonObject lineStylesObj = json["line_styles"].toObject();
    if (lineStylesObj.contains("lane_centerline_style"))
      line_styles.lane_centerline_style = static_cast<Qt::PenStyle>(lineStylesObj["lane_centerline_style"].toInt());
    if (lineStylesObj.contains("lane_boundary_style"))
      line_styles.lane_boundary_style = static_cast<Qt::PenStyle>(lineStylesObj["lane_boundary_style"].toInt());
    if (lineStylesObj.contains("road_style"))
      line_styles.road_style = static_cast<Qt::PenStyle>(lineStylesObj["road_style"].toInt());
  }
  
  // 其他配置
  if (json.contains("antialiasing"))
    antialiasing = json["antialiasing"].toBool();
}

bool DisplayConfig::saveToFile(const QString& filename) const {
  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly)) {
    qDebug() << "Failed to open file for writing:" << filename;
    return false;
  }
  
  QJsonDocument doc(toJson());
  file.write(doc.toJson());
  file.close();
  return true;
}

bool DisplayConfig::loadFromFile(const QString& filename) {
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    qDebug() << "Failed to open file for reading:" << filename;
    return false;
  }
  
  QByteArray data = file.readAll();
  file.close();
  
  QJsonParseError error;
  QJsonDocument doc = QJsonDocument::fromJson(data, &error);
  if (error.error != QJsonParseError::NoError) {
    qDebug() << "Failed to parse JSON:" << error.errorString();
    return false;
  }
  
  if (!doc.isObject()) {
    qDebug() << "JSON document is not an object";
    return false;
  }
  
  fromJson(doc.object());
  return true;
}

DisplayConfig DisplayConfig::defaultConfig() {
  return DisplayConfig();
}

