#ifndef QPAINTER_WRAPPER_H
#define QPAINTER_WRAPPER_H

#include "common/point.h"
#include <QPainter>
#include <QPen>
#include <QtGui>

class GuiQPainter {
public:
  using Point2d = common::Point2d;

  GuiQPainter();
  ~GuiQPainter();

  void GuidrawPoint(QPainter &painter, double x, double y);

  void GuidrawLine(QPainter &painter, double start_x, double start_y,
                   double end_x, double end_y);

  void GuidrawArc(QPainter &painter, double x, double y, double r);

  void GuidrawText(QPainter &painter, double x, double y, const QString &text);

  void drawRect(QPainter &painter, double start_x, double start_y,
                double length, double width);

  void GuidrawWrapText(QPainter &painter, double x, double y,
                       const QString &text);

  void CalcVertexes(double startX, double startY, double endX, double endY,
                    double &arrow_x1, double &arrow_y1, double &arrow_x2,
                    double &arrow_y2);

  void GuidrawArrow(QPainter &painter, double start_x, double start_y,
                    double end_x, double end_y);

  void drawPolygon(QPainter &painter, const Point2d *point, int32_t point_num);
};

#endif // QPAINTER_WRAPPER_H
