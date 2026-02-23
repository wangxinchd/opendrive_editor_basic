#include "QPainter_wrapper.h"
#include <QPainter>

GuiQPainter::GuiQPainter() {}

GuiQPainter::~GuiQPainter() {}

/**
 * \brief 求得箭头两点坐标
 *
 **/
void GuiQPainter::CalcVertexes(double startX, double startY, double endX,
                               double endY, double &arrow_x1, double &arrow_y1,
                               double &arrow_x2, double &arrow_y2) {
  double array_scale = 40.0;
  // double array_max_scale = 20.0;

  // if (mScale >= array_max_scale)
  // {
  //     array_scale = array_max_scale;
  // }
  // else
  // {
  //     array_scale = mScale;
  // }

  double arrowLength = 0.5 * array_scale; // 箭头长度，一般固定
  double arrowDegrees = 0.5;              // 箭头角度，一般固定

  // 求 y / x 的反正切值
  double angle = atan2(endY - startY, endX - startX) + M_PI;

  // 求得箭头点 1 的坐标
  arrow_x1 = endX + arrowLength * cos(angle - arrowDegrees);
  arrow_y1 = endY + arrowLength * sin(angle - arrowDegrees);

  // 求得箭头点 2 的坐标
  arrow_x2 = endX + arrowLength * cos(angle + arrowDegrees);
  arrow_y2 = endY + arrowLength * sin(angle + arrowDegrees);
}

void GuiQPainter::GuidrawPoint(QPainter &painter, double x, double y) {
  painter.drawPoint(x, y);
}

void GuiQPainter::GuidrawLine(QPainter &painter, double start_x, double start_y,
                              double end_x, double end_y) {
  painter.drawLine(start_x, start_y, end_x, end_y);
}

void GuiQPainter::GuidrawArc(QPainter &painter, double x, double y, double r) {
  painter.drawArc(x - r / 2.0, y - r / 2.0, r / 2.0, r / 2.0, 0, 360 * 16);
}

void GuiQPainter::GuidrawText(QPainter &painter, double x, double y,
                              const QString &text) {
  painter.drawText(x, y, text);
}

void GuiQPainter::GuidrawArrow(QPainter &painter, double start_x,
                               double start_y, double end_x, double end_y) {
  double arrow_x1, arrow_y1, arrow_x2, arrow_y2;

  CalcVertexes(start_x, start_y, end_x, end_y, arrow_x1, arrow_y1, arrow_x2,
               arrow_y2);
  GuidrawLine(painter, arrow_x1, arrow_y1, end_x, end_y);
  GuidrawLine(painter, arrow_x2, arrow_y2, end_x, end_y);
}

void GuiQPainter::drawRect(QPainter &painter, double start_x, double start_y,
                           double length, double width) {
  painter.drawRect(start_x - length / 2.0, start_y - width / 2.0, length,
                   width);
}

void GuiQPainter::drawPolygon(QPainter &painter, const Point2d *point,
                              int32_t point_num) {
  int32_t i;
  const Point2d *curr_point, *next_point;
  double start_x, start_y, end_x, end_y;
  for (i = 0; i < point_num; i++) {
    curr_point = &point[i];
    next_point = &point[(i + 1) % point_num];

    start_x = curr_point->x;
    start_y = curr_point->y;
    end_x = next_point->x;
    end_y = next_point->y;

    GuidrawLine(painter, start_x, start_y, end_x, end_y);
  }
}