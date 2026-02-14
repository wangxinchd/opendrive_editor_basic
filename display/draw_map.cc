#include "display.h"
#include <QString>
#include <QPointF>

void Display::drawMap(QPainter &painter) {
  if (map_ == nullptr)
    return;

  QPen pen;
  double start_x, start_y, end_x, end_y;
  
  // draw lanes
  std::vector<std::shared_ptr<map_base::Lane>> lanes = map_->lanes();
  for (int i = 0; i < lanes.size(); i++) {
    // draw lane centerline
    if (config_.visibility.show_lane_centerline) {
      const std::vector<common::Pose2d> &centerline = lanes[i]->centerline();
      for (int j = 0; j < centerline.size() - 1; j++) {
        start_x = convertXCoordinateUsingMapCenterPoint(centerline[j].x);
        start_y = convertYCoordinateUsingMapCenterPoint(centerline[j].y);

        end_x = convertXCoordinateUsingMapCenterPoint(centerline[j + 1].x);
        end_y = convertYCoordinateUsingMapCenterPoint(centerline[j + 1].y);

        pen.setColor(config_.colors.lane_centerline_color);
        pen.setWidthF(config_.line_widths.lane_centerline_width);
        pen.setStyle(config_.line_styles.lane_centerline_style);
        painter.setPen(pen);
        GuiPainter_.GuidrawLine(painter, start_x, start_y, end_x, end_y);
      }
    }

    // draw left lane boundary
    if (config_.visibility.show_lane_left_boundary) {
      const std::shared_ptr<map_base::LaneBoundary> left_lane_boundary =
          lanes[i]->leftLaneBoundary();

      if (left_lane_boundary != nullptr) {
        const std::vector<common::Pose2d> &poses = left_lane_boundary->poses();
        for (int j = 0; j < poses.size() - 1; j++) {
          start_x = convertXCoordinateUsingMapCenterPoint(poses[j].x);
          start_y = convertYCoordinateUsingMapCenterPoint(poses[j].y);

          end_x = convertXCoordinateUsingMapCenterPoint(poses[j + 1].x);
          end_y = convertYCoordinateUsingMapCenterPoint(poses[j + 1].y);

          pen.setColor(config_.colors.lane_left_boundary_color);
          pen.setWidthF(config_.line_widths.lane_boundary_width);
          pen.setStyle(config_.line_styles.lane_boundary_style);
          painter.setPen(pen);
          GuiPainter_.GuidrawLine(painter, start_x, start_y, end_x, end_y);
        }
      }
    }

    // draw right lane boundary
    if (config_.visibility.show_lane_right_boundary) {
      const std::shared_ptr<map_base::LaneBoundary> right_lane_boundary =
          lanes[i]->rightLaneBoundary();

      if (right_lane_boundary != nullptr) {
        const std::vector<common::Pose2d> &poses = right_lane_boundary->poses();
        for (int j = 0; j < poses.size() - 1; j++) {
          start_x = convertXCoordinateUsingMapCenterPoint(poses[j].x);
          start_y = convertYCoordinateUsingMapCenterPoint(poses[j].y);

          end_x = convertXCoordinateUsingMapCenterPoint(poses[j + 1].x);
          end_y = convertYCoordinateUsingMapCenterPoint(poses[j + 1].y);

          pen.setColor(config_.colors.lane_right_boundary_color);
          pen.setWidthF(config_.line_widths.lane_boundary_width);
          pen.setStyle(config_.line_styles.lane_boundary_style);
          painter.setPen(pen);
          GuiPainter_.GuidrawLine(painter, start_x, start_y, end_x, end_y);
        }
      }
    }

    // draw lane ID
    if (config_.visibility.show_lane_id && !lanes[i]->centerline().empty()) {
      const auto& centerline = lanes[i]->centerline();
      int mid_index = centerline.size() / 2;
      double text_x = convertXCoordinateUsingMapCenterPoint(centerline[mid_index].x);
      double text_y = convertYCoordinateUsingMapCenterPoint(centerline[mid_index].y);
      
      QString laneIdText = QString("L%1").arg(lanes[i]->id().lane_id);
      painter.setPen(QPen(config_.colors.lane_centerline_color));
      painter.drawText(QPointF(text_x, text_y), laneIdText);
    }
  }

  // draw roads (if needed in future)
  if (config_.visibility.show_road) {
    // TODO: 实现道路绘制
  }

  // draw intersections (if needed in future)
  if (config_.visibility.show_intersection) {
    // TODO: 实现交叉口绘制
  }

  // draw road IDs (if needed in future)
  if (config_.visibility.show_road_id) {
    // TODO: 实现道路ID标签绘制
  }
}