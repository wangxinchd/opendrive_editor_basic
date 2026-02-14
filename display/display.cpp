#include "display.h"
#include "ui_display.h"
#include "display_config/display_config_dialog.h"
#include "form_edit/edit_road/form_edit_road.h"

#include "map_sdk/map_engine/include/map_parser_factory.h"
#include <QFileDialog>
#include <QMessageBox>

Display::Display(QWidget *parent) : QMainWindow(parent), ui(new Ui::Display) {
  ui->setupUi(this);

  Scale_ = 5.0;
  WinXBias_ = 0.0;
  WinYBias_ = 0.0;

  status_bar_ = ui->statusbar;

  timer_ = new QTimer();
  timer_->setInterval(10);
  timer_->start();

  connect(timer_, &QTimer::timeout, this, &Display::timeoutFunc);
  connect(ui->actionopendrive_map, &QAction::triggered, this,
          &Display::openXodr);
  connect(ui->actionreload_file, &QAction::triggered, this,
          &Display::reloadXodr);
  connect(ui->actiondisplay_config, &QAction::triggered, this,
          &Display::openConfigDialog);
  connect(ui->actionedit_road, &QAction::triggered, this,
          &Display::openRoadEditPanel);

  // map
  map_ = std::make_shared<map_base::Map>();
  
  // 初始化默认配置
  config_ = DisplayConfig::defaultConfig();
}

Display::~Display() { delete ui; }

void Display::timeoutFunc() { this->update(); }
void Display::openXodr() {
#if 1
  QString open_fname =
      QFileDialog::getOpenFileName(this, "选择文件", "/home/", "Files(*.xodr)");
  if (open_fname.isEmpty()) {
    QMessageBox::warning(this, "waring", "no select xodr file!");
    return;
  }
  xodr_fname_ = open_fname.toStdString();
  is_map_loaded_ = false;
#else
  xodr_fname_ =
      "/home/gwm/project/driving_senario/xodr/straight_1lane_width_change.xodr";
#endif

  map_parser_ = map_sdk::MapParserFactory::GetInstance().CreateMapParser(
      map_sdk::MapType::MAP_TYPE_OPENDRIVE);

  map_parser_->Parse(xodr_fname_, *map_);

  is_map_loaded_ = true;
}

void Display::reloadXodr() {
  // 检查是否已经加载过文件
  if (xodr_fname_.empty()) {
    QMessageBox::warning(this, "警告", "没有已加载的文件，请先打开文件！");
    return;
  }

  // 重新创建map对象以清空之前的数据
  map_ = std::make_shared<map_base::Map>();

  // 重新创建解析器并解析文件
  map_parser_ = map_sdk::MapParserFactory::GetInstance().CreateMapParser(
      map_sdk::MapType::MAP_TYPE_OPENDRIVE);

  map_parser_->Parse(xodr_fname_, *map_);

  is_map_loaded_ = true;

  QMessageBox::information(this, "提示", "文件重新加载成功！");
}

void Display::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  // set antialiasing from config
  painter.setRenderHint(QPainter::Antialiasing, config_.antialiasing);
  // set background color from config
  painter.setBackground(QBrush(config_.colors.background_color));
  painter.setBackgroundMode(Qt::OpaqueMode);

  updateViewPoint();
  drawMap(painter);

#if 0
  // test code
  QPen pen;
  pen.setColor(Qt::black);
  pen.setWidthF(5);
  painter.setPen(pen);

  painter.drawLine(QPoint(0, 0), QPoint(100 * 4, 100 * 4));
  painter.drawText(100, 500, "Nine pineapples");
#endif
}

void Display::updateWin() { this->update(); }

void Display::resizeEvent(QResizeEvent *event) {
  // mWinWidth mWinHeight 是从菜单栏左上角到状态栏右下角，不算标题栏的大小
  WinWidth_ = event->size().width();
  WinHeight_ = event->size().height();
}

void Display::updateViewPoint() {
  if (map_ != nullptr) {
    MapCenterX_ = map_->metadata().center_x;
    MapCenterY_ = map_->metadata().center_y;
  }
}

void Display::openConfigDialog() {
  // 如果对话框不存在，创建它
  if (config_dialog_ == nullptr) {
    config_dialog_ = new DisplayConfigDialog(config_, this);
    
    // 连接信号槽：当配置变更时更新Display的配置
    connect(config_dialog_, &DisplayConfigDialog::configChanged,
            this, &Display::onConfigChanged);
    
    // 设置对话框为非模态
    config_dialog_->setModal(false);
  } else {
    // 如果对话框已存在，更新其配置数据
    config_dialog_->setConfig(config_);
  }
  
  // 非模态显示：允许用户同时操作主窗口和配置窗口
  config_dialog_->show();
  config_dialog_->raise();   // 将窗口置于最前
  config_dialog_->activateWindow();  // 激活窗口
}

void Display::onConfigChanged(const DisplayConfig& config) {
  config_ = config;  // 更新Display的配置
  update();          // 触发重绘
}

void Display::openRoadEditPanel() {
  // 如果面板不存在，创建它
  if (road_edit_panel_ == nullptr) {
    road_edit_panel_ = new FormEditRoad(this);
    road_edit_panel_->setWindowFlags(Qt::Window);  // 独立窗口
    road_edit_panel_->setWindowTitle("道路参考线编辑 (ParamPoly3)");
  }

  if (map_ != nullptr) {
    road_edit_panel_->setCurrentFile(QString::fromStdString(map_->name()));
  }

  // 显示面板
  road_edit_panel_->show();
  road_edit_panel_->raise();
  road_edit_panel_->activateWindow();
}