#include "display_config_dialog.h"
#include "ui_display_config_dialog.h"
#include <QDebug>
#include <QFileDialog>
#include <QColorDialog>
#include <QPushButton>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>

DisplayConfigDialog::DisplayConfigDialog(const DisplayConfig& config, 
                                         QWidget* parent)
    : QDialog(parent), 
      ui(new Ui::DisplayConfigDialog),
      current_config_(config),
      original_config_(config) {
  ui->setupUi(this);
  
  // 将配置数据加载到UI控件
  loadConfigToUI(config);
  
  // 连接信号槽
  connect(ui->buttonApply, &QPushButton::clicked, 
          this, &DisplayConfigDialog::onApplyClicked);
  connect(ui->buttonOk, &QPushButton::clicked, 
          this, &DisplayConfigDialog::onOkClicked);
  connect(ui->buttonCancel, &QPushButton::clicked, 
          this, &DisplayConfigDialog::onCancelClicked);
  connect(ui->buttonSave, &QPushButton::clicked, 
          this, &DisplayConfigDialog::onSaveClicked);
  
  // 连接颜色按钮
  connect(ui->buttonLaneCenterlineColor, &QPushButton::clicked,
          this, &DisplayConfigDialog::onLaneCenterlineColorClicked);
  connect(ui->buttonLeftBoundaryColor, &QPushButton::clicked,
          this, &DisplayConfigDialog::onLeftBoundaryColorClicked);
  connect(ui->buttonRightBoundaryColor, &QPushButton::clicked,
          this, &DisplayConfigDialog::onRightBoundaryColorClicked);
  connect(ui->buttonRoadColor, &QPushButton::clicked,
          this, &DisplayConfigDialog::onRoadColorClicked);
}

DisplayConfigDialog::~DisplayConfigDialog() {
  delete ui;
}

void DisplayConfigDialog::setConfig(const DisplayConfig& config) {
  current_config_ = config;
  original_config_ = config;
  loadConfigToUI(config);
}

DisplayConfig DisplayConfigDialog::getConfig() const {
  return current_config_;
}

void DisplayConfigDialog::loadConfigToUI(const DisplayConfig& config) {
  // 显示元素开关
  ui->checkboxLaneCenterline->setChecked(
      config.visibility.show_lane_centerline);
  ui->checkboxLeftBoundary->setChecked(
      config.visibility.show_lane_left_boundary);
  ui->checkboxRightBoundary->setChecked(
      config.visibility.show_lane_right_boundary);
  ui->checkboxRoad->setChecked(config.visibility.show_road);
  ui->checkboxIntersection->setChecked(
      config.visibility.show_intersection);
  ui->checkboxLaneId->setChecked(config.visibility.show_lane_id);
  ui->checkboxRoadId->setChecked(config.visibility.show_road_id);
  
  // 颜色配置 - 更新颜色按钮的显示
  updateColorButton(ui->buttonLaneCenterlineColor, 
                     config.colors.lane_centerline_color);
  updateColorButton(ui->buttonLeftBoundaryColor, 
                     config.colors.lane_left_boundary_color);
  updateColorButton(ui->buttonRightBoundaryColor, 
                     config.colors.lane_right_boundary_color);
  updateColorButton(ui->buttonRoadColor, 
                     config.colors.road_color);
  
  // 线宽配置
  ui->spinboxLaneCenterlineWidth->setValue(
      config.line_widths.lane_centerline_width);
  ui->spinboxLaneBoundaryWidth->setValue(
      config.line_widths.lane_boundary_width);
  
  // 线型配置 - 映射Qt::PenStyle到ComboBox索引
  // Qt::SolidLine=0, Qt::DashLine=1, Qt::DotLine=2, Qt::DashDotLine=3
  int centerlineIndex = 0;
  switch (config.line_styles.lane_centerline_style) {
    case Qt::SolidLine: centerlineIndex = 0; break;
    case Qt::DashLine: centerlineIndex = 1; break;
    case Qt::DotLine: centerlineIndex = 2; break;
    case Qt::DashDotLine: centerlineIndex = 3; break;
    default: centerlineIndex = 2; break; // 默认点线
  }
  ui->comboLaneCenterlineStyle->setCurrentIndex(centerlineIndex);
  
  int boundaryIndex = 0;
  switch (config.line_styles.lane_boundary_style) {
    case Qt::SolidLine: boundaryIndex = 0; break;
    case Qt::DashLine: boundaryIndex = 1; break;
    case Qt::DotLine: boundaryIndex = 2; break;
    case Qt::DashDotLine: boundaryIndex = 3; break;
    default: boundaryIndex = 0; break; // 默认实线
  }
  ui->comboLaneBoundaryStyle->setCurrentIndex(boundaryIndex);
  
  // 其他配置
  ui->checkboxAntialiasing->setChecked(config.antialiasing);
}

DisplayConfig DisplayConfigDialog::getConfigFromUI() const {
  DisplayConfig config;
  
  // 从UI控件读取显示元素开关
  config.visibility.show_lane_centerline = 
      ui->checkboxLaneCenterline->isChecked();
  config.visibility.show_lane_left_boundary = 
      ui->checkboxLeftBoundary->isChecked();
  config.visibility.show_lane_right_boundary = 
      ui->checkboxRightBoundary->isChecked();
  config.visibility.show_road = ui->checkboxRoad->isChecked();
  config.visibility.show_intersection = 
      ui->checkboxIntersection->isChecked();
  config.visibility.show_lane_id = ui->checkboxLaneId->isChecked();
  config.visibility.show_road_id = ui->checkboxRoadId->isChecked();
  
  // 从UI控件读取颜色配置
  config.colors.lane_centerline_color = 
      getColorFromButton(ui->buttonLaneCenterlineColor);
  config.colors.lane_left_boundary_color = 
      getColorFromButton(ui->buttonLeftBoundaryColor);
  config.colors.lane_right_boundary_color = 
      getColorFromButton(ui->buttonRightBoundaryColor);
  config.colors.road_color = 
      getColorFromButton(ui->buttonRoadColor);
  
  // 从UI控件读取线宽配置
  config.line_widths.lane_centerline_width = 
      ui->spinboxLaneCenterlineWidth->value();
  config.line_widths.lane_boundary_width = 
      ui->spinboxLaneBoundaryWidth->value();
  
  // 从UI控件读取线型配置 - 映射ComboBox索引到Qt::PenStyle
  int centerlineIndex = ui->comboLaneCenterlineStyle->currentIndex();
  switch (centerlineIndex) {
    case 0: config.line_styles.lane_centerline_style = Qt::SolidLine; break;
    case 1: config.line_styles.lane_centerline_style = Qt::DashLine; break;
    case 2: config.line_styles.lane_centerline_style = Qt::DotLine; break;
    case 3: config.line_styles.lane_centerline_style = Qt::DashDotLine; break;
    default: config.line_styles.lane_centerline_style = Qt::DotLine; break;
  }
  
  int boundaryIndex = ui->comboLaneBoundaryStyle->currentIndex();
  switch (boundaryIndex) {
    case 0: config.line_styles.lane_boundary_style = Qt::SolidLine; break;
    case 1: config.line_styles.lane_boundary_style = Qt::DashLine; break;
    case 2: config.line_styles.lane_boundary_style = Qt::DotLine; break;
    case 3: config.line_styles.lane_boundary_style = Qt::DashDotLine; break;
    default: config.line_styles.lane_boundary_style = Qt::SolidLine; break;
  }
  
  // 从UI控件读取其他配置
  config.antialiasing = ui->checkboxAntialiasing->isChecked();
  
  return config;
}

void DisplayConfigDialog::onApplyClicked() {
  current_config_ = getConfigFromUI();
  emit configChanged(current_config_);
}

void DisplayConfigDialog::onOkClicked() {
  current_config_ = getConfigFromUI();
  emit configChanged(current_config_);
  accept();
}

void DisplayConfigDialog::onCancelClicked() {
  // 恢复原始配置到UI（可选，因为窗口会关闭）
  loadConfigToUI(original_config_);
  reject();
}

void DisplayConfigDialog::onSaveClicked() {
  current_config_ = getConfigFromUI();
  QString filename = QFileDialog::getSaveFileName(
      this, "保存配置", "", "JSON Files (*.json)");
  if (!filename.isEmpty()) {
    if (current_config_.saveToFile(filename)) {
      qDebug() << "配置已保存到:" << filename;
    } else {
      qDebug() << "保存配置失败";
    }
  }
}

void DisplayConfigDialog::onLaneCenterlineColorClicked() {
  QColor color = QColorDialog::getColor(
      getColorFromButton(ui->buttonLaneCenterlineColor), 
      this, "选择车道中心线颜色");
  if (color.isValid()) {
    updateColorButton(ui->buttonLaneCenterlineColor, color);
  }
}

void DisplayConfigDialog::onLeftBoundaryColorClicked() {
  QColor color = QColorDialog::getColor(
      getColorFromButton(ui->buttonLeftBoundaryColor), 
      this, "选择左边界颜色");
  if (color.isValid()) {
    updateColorButton(ui->buttonLeftBoundaryColor, color);
  }
}

void DisplayConfigDialog::onRightBoundaryColorClicked() {
  QColor color = QColorDialog::getColor(
      getColorFromButton(ui->buttonRightBoundaryColor), 
      this, "选择右边界颜色");
  if (color.isValid()) {
    updateColorButton(ui->buttonRightBoundaryColor, color);
  }
}

void DisplayConfigDialog::onRoadColorClicked() {
  QColor color = QColorDialog::getColor(
      getColorFromButton(ui->buttonRoadColor), 
      this, "选择道路颜色");
  if (color.isValid()) {
    updateColorButton(ui->buttonRoadColor, color);
  }
}

void DisplayConfigDialog::updateColorButton(QPushButton* button, 
                                            const QColor& color) {
  QPixmap pixmap(20, 20);
  pixmap.fill(color);
  button->setIcon(QIcon(pixmap));
  button->setProperty("color", color);
}

QColor DisplayConfigDialog::getColorFromButton(QPushButton* button) const {
  return button->property("color").value<QColor>();
}

