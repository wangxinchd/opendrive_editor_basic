#include "display.h"

#define modifiers_str(state)                                                   \
  (state == Qt::NoModifier        ? "None"                                     \
   : state == Qt::ShiftModifier   ? "Shift"                                    \
   : state == Qt::ControlModifier ? "Ctrl"                                     \
   : state == Qt::AltModifier     ? "Alt"                                      \
                                  : "Unknown")

void Display::keyPressEvent(QKeyEvent *event) {
  printf("modifiers:%s, press key: (%d)\n", modifiers_str(event->modifiers()),
         event->key());
  switch (event->modifiers()) {
  case Qt::NoModifier:
    normalKeyPressEvent(event);
    break;
  case Qt::ShiftModifier:
    keyWithShiftPressEvent(event);
    break;
  case Qt::ControlModifier:
    keyWithCtrlPressEvent(event);
    break;
  case Qt::AltModifier:
    break;
  default:
    break;
  }
}

void Display::normalKeyPressEvent(QKeyEvent *event) {
  printf("== normal Key==\n");
  bool is_update = false;
  switch (event->key()) {
  case Qt::Key_Minus: // -
    Scale_ /= 2.0;
    is_update = true;
    printf(" Qt::Key_Less scale :%lf\n", Scale_);
    break;
  case Qt::Key_Space:
    Scale_ = 1.0;
    WinXBias_ = 0.0;
    WinYBias_ = 0.0;
    is_update = true;
    break;
  case Qt::Key_I: {
    break;
  }
  case Qt::Key_A: {

  }

  break;
  default:
    break;
  }

  if (is_update)
    updateWin();
}

void Display::keyWithShiftPressEvent(QKeyEvent *event) {
  bool is_update = false;
  switch (event->key()) {
  case Qt::Key_Plus: // + (shift + =)
    Scale_ *= 2.0;
    is_update = true;
    printf("Qt::Key_Equal scale :%lf\n", Scale_);
    break;

  default:
    break;
  }

  if (is_update)
    updateWin();
}

void Display::keyWithCtrlPressEvent(QKeyEvent *event) {
  bool is_update = false;
  switch (event->key()) {
  case Qt::Key_Space: // (ctrl + =)

    break;
  default:
    break;
  }

  if (is_update)
    updateWin();
}