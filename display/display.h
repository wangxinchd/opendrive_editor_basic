#ifndef DISPLAY_H
#define DISPLAY_H

#include <QMainWindow>

#include "QPainter_wrapper.h"
#include <QAction>
#include <QStatusBar>
#include <QtGui>
#include <memory>

#include "map_sdk/map_base/map.h"
#include "map_sdk/map_engine/include/map_parser_base.h"
#include "display_config/display_config.h"
#include <type_traits>

#define INVALID_MOUSE_PIXEL_VALUE (-1)

QT_BEGIN_NAMESPACE
namespace Ui {
class Display;
}
QT_END_NAMESPACE

// 前向声明
class DisplayConfigDialog;
class FormEditRoad;

class Display : public QMainWindow {
  Q_OBJECT

public:
  Display(QWidget *parent = nullptr);
  ~Display();

private:
  QTimer *timer_;
  QStatusBar *status_bar_;

  GuiQPainter GuiPainter_;

  Ui::Display *ui;

  std::string xodr_fname_;
  std::string is_map_loaded_;

  std::unique_ptr<map_sdk::MapParserBase> map_parser_;
  std::shared_ptr<map_base::Map> map_;

  // 坐标转换相关成员变量
  double MapCenterX_ = 0.0;
  double MapCenterY_ = 0.0;
  double WinXBias_ = 0.0;
  double WinYBias_ = 0.0;
  double Scale_ = 1.0;
  int WinWidth_ = 0.0;
  int WinHeight_ = 0.0;

  // mouse
  int32_t LMousePosX_; // 鼠标点击或者释放的位置
  int32_t LMousePosY_;
  int32_t RMousePosX_;
  int32_t RMousePosY_;
  int32_t MMousePosX_;
  int32_t MMousePosY_;
  int32_t MouseMoveX_;
  int32_t MouseMoveY_;

  bool IsLMousePress_;
  bool IsRMousePress_;
  bool IsMMousePress_;

  // 显示配置
  DisplayConfig config_;
  DisplayConfigDialog* config_dialog_ = nullptr;

  // 道路编辑面板
  FormEditRoad* road_edit_panel_ = nullptr;

private slots:
  void onConfigChanged(const DisplayConfig& config);  // 响应配置变更
  void openConfigDialog();                             // 打开配置窗口
  void openRoadEditPanel();                            // 打开道路编辑面板
  void reloadXodr();                                   // 重新加载文件

private:
  void timeoutFunc();
  void openXodr();

  void drawMap(QPainter &painter);

  // 多态函数
  void paintEvent(QPaintEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

  void keyPressEvent(QKeyEvent *event) override;
  void normalKeyPressEvent(QKeyEvent *event);
  void keyWithShiftPressEvent(QKeyEvent *event);
  void keyWithCtrlPressEvent(QKeyEvent *event);

  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;

  void updateWin();
  void updateViewPoint();

  // 屏幕坐标到地图坐标的转换函数
  double ConvertScreenXToXodrPoint(const int32_t x);
  double ConvertScreenYToXodrPoint(const int32_t y);

  // 坐标转换模板函数
  template <typename T>
  double convertXCoordinateUsingMapCenterPoint(const T &value);

  template <typename T>
  double convertYCoordinateUsingMapCenterPoint(const T &value);
};
#endif // DISPLAY_H
