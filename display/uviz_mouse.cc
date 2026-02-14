#include "display.h"
#include <QScreen>

void Display::mousePressEvent(
    QMouseEvent *event) // 按下的一瞬间执行，持续按下只会执行一次
{
  if (event->button() == Qt::LeftButton) { //左键
    IsLMousePress_ = true;
    LMousePosX_ = event->x();
    LMousePosY_ = event->y();
  } else if (event->button() == Qt::RightButton) { //右键
    IsRMousePress_ = true;
    RMousePosX_ = event->x();
    RMousePosY_ = event->y();
  } else if (event->button() == Qt::MiddleButton) { //滚轮键
    IsMMousePress_ = true;
    MMousePosX_ = event->x();
    MMousePosY_ = event->y();
  }
}

void Display::mouseReleaseEvent(QMouseEvent *event) {
  bool is_update = false;
  if (event->button() == Qt::LeftButton) { //左键
    if (INVALID_MOUSE_PIXEL_VALUE != LMousePosX_ &&
        INVALID_MOUSE_PIXEL_VALUE != LMousePosY_) {
      WinXBias_ += (event->x() - LMousePosX_) / Scale_;
      WinYBias_ += (event->y() - LMousePosY_) / Scale_;
      is_update = true;
    }
    IsLMousePress_ = false;
    LMousePosX_ = INVALID_MOUSE_PIXEL_VALUE;
    LMousePosY_ = INVALID_MOUSE_PIXEL_VALUE;
  } else if (event->button() == Qt::RightButton) { //右键
    IsRMousePress_ = false;
  } else if (event->button() == Qt::MiddleButton) { //滚轮键
    IsMMousePress_ = false;
  }

  if (is_update)
    updateWin();
}

void Display::wheelEvent(QWheelEvent *event) {
  if (event->angleDelta().y() > 0) {
    Scale_ /= 2.0;
    updateWin();
    printf("Qt::wheelEvent scale :%lf\n", Scale_);
  } else if (event->angleDelta().y() < 0) {
    Scale_ *= 2.0;
    printf("Qt::wheelEvent scale :%lf\n", Scale_);
    updateWin();
  }
}

void Display::mouseMoveEvent(QMouseEvent *event) {
  MouseMoveX_ = event->pos().x();
  MouseMoveY_ = event->pos().y();

  if (IsRMousePress_ || IsLMousePress_) {
    // 鼠标在屏幕上的位置
    QPoint globalPos = event->globalPos();

    // 将屏幕坐标转换为地图坐标
    double map_x = ConvertScreenXToXodrPoint(event->pos().x());
    double map_y = ConvertScreenYToXodrPoint(event->pos().y());

    // 在状态栏显示鼠标位置和地图位置信息
    status_bar_->showMessage(
        QString("鼠标位置: (%1, %2)  |  地图位置: (%3, %4)")
            .arg(event->pos().x())
            .arg(event->pos().y())
            .arg(map_x, 0, 'f', 2)
            .arg(map_y, 0, 'f', 2),
        40000);
  }
}