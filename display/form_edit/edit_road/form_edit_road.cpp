#include "form_edit_road.h"
#include "ui_form_edit_road.h"
#include "common/odr_geometry.h"

#include <QMessageBox>
#include <QHeaderView>

FormEditRoad::FormEditRoad(QWidget *parent)
    : QWidget(parent), ui(new Ui::FormEditRoad) {
  ui->setupUi(this);

  // 设置表格列宽
  ui->tableWidget_controlPoints->setColumnWidth(0, 40);   // 序号
  ui->tableWidget_controlPoints->setColumnWidth(1, 100);  // X
  ui->tableWidget_controlPoints->setColumnWidth(2, 100);  // Y
  ui->tableWidget_controlPoints->setColumnWidth(3, 100);  // yaw

  // 设置系数表格列宽
  ui->tableWidget_segments->setColumnWidth(0, 40);   // 段
  ui->tableWidget_segments->setColumnWidth(1, 80);  // p范围
  // 系数列宽
  for (int i = 2; i < 10; ++i) {
    ui->tableWidget_segments->setColumnWidth(i, 70);
  }

  // 禁用编辑按钮（初始状态）
  ui->pushButton_deletePoint->setEnabled(false);
  ui->pushButton_moveUp->setEnabled(false);
  ui->pushButton_moveDown->setEnabled(false);
  ui->pushButton_fit->setEnabled(false);
  ui->pushButton_save->setEnabled(false);
  ui->pushButton_reset->setEnabled(false);
}

FormEditRoad::~FormEditRoad() { delete ui; }

void FormEditRoad::setRoadList(const QStringList& road_names) {
  ui->comboBox_road->clear();
  ui->comboBox_road->addItems(road_names);
}

void FormEditRoad::setCurrentFile(const QString& file_path) {
  ui->lineEdit_filePath->setText(file_path);
}

void FormEditRoad::setParamPoly3Segments(
    const std::vector<common::OpenDriveParamPoly3>& segments) {
  original_segments_ = segments;
  fitted_segments_ = segments;
  updateSegmentTable();
  updateOpenDriveFormat();
}

void FormEditRoad::setControlPoints(
    const std::vector<common::Pose2d>& control_points) {
  control_points_ = control_points;
  updateControlPointTable();

  // 如果有控制点，启用相关按钮
  bool has_points = !control_points_.empty();
  ui->pushButton_fit->setEnabled(has_points);
  ui->pushButton_reset->setEnabled(has_points);
}

std::vector<common::Pose2d> FormEditRoad::getControlPoints() const {
  return control_points_;
}

std::vector<common::OpenDriveParamPoly3> FormEditRoad::getFittedSegments() const {
  return fitted_segments_;
}

void FormEditRoad::onRoadSelected(int index) {
  if (index >= 0) {
    emit roadSelected(index);
  }
}

void FormEditRoad::onAddPointClicked() {
  // 先同步表格中已编辑的数据到 control_points_
  syncTableToControlPoints();
  
  // 添加新的控制点（默认在原点）
  common::Pose2d new_point(0.0, 0.0, 0.0);
  control_points_.push_back(new_point);
  selected_point_index_ = control_points_.size() - 1;

  updateControlPointTable();

  ui->pushButton_fit->setEnabled(true);
  ui->pushButton_reset->setEnabled(true);
}

void FormEditRoad::onDeletePointClicked() {
  // 先同步表格中已编辑的数据到 control_points_
  syncTableToControlPoints();
  
  if (selected_point_index_ >= 0 && 
      selected_point_index_ < static_cast<int>(control_points_.size())) {
    // 至少保留一个控制点
    if (control_points_.size() <= 1) {
      QMessageBox::warning(this, "警告", "至少需要保留1个控制点");
      return;
    }

    control_points_.erase(control_points_.begin() + selected_point_index_);
    selected_point_index_ = -1;

    updateControlPointTable();

    // 更新按钮状态
    ui->pushButton_deletePoint->setEnabled(false);
    ui->pushButton_moveUp->setEnabled(false);
    ui->pushButton_moveDown->setEnabled(false);

    // 如果控制点少于2个，禁用拟合
    ui->pushButton_fit->setEnabled(control_points_.size() >= 2);
  }
}

void FormEditRoad::onMoveUpClicked() {
  // 先同步表格中已编辑的数据到 control_points_
  syncTableToControlPoints();
  
  if (selected_point_index_ > 0) {
    std::swap(control_points_[selected_point_index_], 
              control_points_[selected_point_index_ - 1]);
    selected_point_index_--;
    updateControlPointTable();
  }
}

void FormEditRoad::onMoveDownClicked() {
  // 先同步表格中已编辑的数据到 control_points_
  syncTableToControlPoints();
  
  if (selected_point_index_ >= 0 && 
      selected_point_index_ < static_cast<int>(control_points_.size()) - 1) {
    std::swap(control_points_[selected_point_index_], 
              control_points_[selected_point_index_ + 1]);
    selected_point_index_++;
    updateControlPointTable();
  }
}

void FormEditRoad::onControlPointClicked(int row, int column) {
  (void)column;  // 未使用的参数
  selected_point_index_ = row;

  // 启用编辑按钮
  ui->pushButton_deletePoint->setEnabled(true);
  ui->pushButton_moveUp->setEnabled(row > 0);
  ui->pushButton_moveDown->setEnabled(row < static_cast<int>(control_points_.size()) - 1);
}

void FormEditRoad::onFitClicked() {
  // 先同步表格中已编辑的数据到 control_points_
  syncTableToControlPoints();
  
  // 至少需要2个控制点
  if (control_points_.size() < 2) {
    QMessageBox::warning(this, "警告", "至少需要2个控制点才能拟合曲线");
    return;
  }

  // 根据选择的拟合方法进行拟合
  std::vector<common::OpenDriveParamPoly3> segments;

  if (ui->radioButton_hermite->isChecked()) {
    // 分段三次 Hermite 插值
    segments = common::OdrGeometry::GenerateFromPoses(control_points_);
  } else {
    // 最小二乘法（暂未实现）
    QMessageBox::information(this, "提示", "最小二乘法拟合暂未实现，使用 Hermite 插值");
    segments = common::OdrGeometry::GenerateFromPoses(control_points_);
  }

  fitted_segments_ = segments;
  updateSegmentTable();
  updateOpenDriveFormat();

  // 发送系数变化信号（用于预览）
  emit coeffChanged(fitted_segments_);

  // 启用保存按钮
  ui->pushButton_save->setEnabled(true);
}

void FormEditRoad::onSaveClicked() {
  // 发送保存请求信号
  emit saveRequested(fitted_segments_);
}

void FormEditRoad::onCancelClicked() {
  // 发送取消信号
  emit cancelRequested();
  close();
}

void FormEditRoad::onResetClicked() {
  // 重置为原始系数
  fitted_segments_ = original_segments_;
  updateSegmentTable();
  updateOpenDriveFormat();

  // 清空控制点
  control_points_.clear();
  selected_point_index_ = -1;
  updateControlPointTable();

  // 禁用按钮
  ui->pushButton_deletePoint->setEnabled(false);
  ui->pushButton_moveUp->setEnabled(false);
  ui->pushButton_moveDown->setEnabled(false);
  ui->pushButton_fit->setEnabled(false);
  ui->pushButton_save->setEnabled(false);
  ui->pushButton_reset->setEnabled(false);

  // 发送重置信号
  emit resetRequested();
}

void FormEditRoad::updateControlPointTable() {
  ui->tableWidget_controlPoints->setRowCount(control_points_.size());

  for (size_t i = 0; i < control_points_.size(); ++i) {
    const auto& point = control_points_[i];

    // 序号
    QTableWidgetItem* item_idx = new QTableWidgetItem(QString::number(i + 1));
    item_idx->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget_controlPoints->setItem(i, 0, item_idx);

    // X
    QTableWidgetItem* item_x = new QTableWidgetItem(QString::number(point.x, 'f', 3));
    item_x->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget_controlPoints->setItem(i, 1, item_x);

    // Y
    QTableWidgetItem* item_y = new QTableWidgetItem(QString::number(point.y, 'f', 3));
    item_y->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget_controlPoints->setItem(i, 2, item_y);

    // yaw (转换为度显示)
    double yaw_deg = point.yaw;
    QTableWidgetItem* item_yaw = new QTableWidgetItem(QString::number(yaw_deg, 'f', 1));
    item_yaw->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget_controlPoints->setItem(i, 3, item_yaw);
  }
}

void FormEditRoad::updateSegmentTable() {
  ui->tableWidget_segments->setRowCount(fitted_segments_.size());

  double accumulated_length = 0.0;

  for (size_t i = 0; i < fitted_segments_.size(); ++i) {
    const auto& seg = fitted_segments_[i];

    // 计算段长度（简化估算）
    double segment_length = std::hypot(seg.bU, seg.bV);
    double p_start = accumulated_length;
    double p_end = accumulated_length + segment_length;
    accumulated_length = p_end;

    // 段号
    QTableWidgetItem* item_seg = new QTableWidgetItem(QString::number(i + 1));
    item_seg->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget_segments->setItem(i, 0, item_seg);

    // p范围
    QString p_range = QString("[%1, %2]").arg(p_start, 0, 'f', 1).arg(p_end, 0, 'f', 1);
    QTableWidgetItem* item_range = new QTableWidgetItem(p_range);
    item_range->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget_segments->setItem(i, 1, item_range);

    // U系数
    ui->tableWidget_segments->setItem(i, 2, new QTableWidgetItem(QString::number(seg.aU, 'f', 3)));
    ui->tableWidget_segments->setItem(i, 3, new QTableWidgetItem(QString::number(seg.bU, 'f', 3)));
    ui->tableWidget_segments->setItem(i, 4, new QTableWidgetItem(QString::number(seg.cU, 'f', 3)));
    ui->tableWidget_segments->setItem(i, 5, new QTableWidgetItem(QString::number(seg.dU, 'f', 3)));

    // V系数
    ui->tableWidget_segments->setItem(i, 6, new QTableWidgetItem(QString::number(seg.aV, 'f', 3)));
    ui->tableWidget_segments->setItem(i, 7, new QTableWidgetItem(QString::number(seg.bV, 'f', 3)));
    ui->tableWidget_segments->setItem(i, 8, new QTableWidgetItem(QString::number(seg.cV, 'f', 3)));
    ui->tableWidget_segments->setItem(i, 9, new QTableWidgetItem(QString::number(seg.dV, 'f', 3)));
  }

  // 设置所有单元格为只读
  for (int i = 0; i < ui->tableWidget_segments->rowCount(); ++i) {
    for (int j = 0; j < ui->tableWidget_segments->columnCount(); ++j) {
      if (ui->tableWidget_segments->item(i, j)) {
        ui->tableWidget_segments->item(i, j)->setFlags(
            ui->tableWidget_segments->item(i, j)->flags() & ~Qt::ItemIsEditable);
      }
    }
  }
}

void FormEditRoad::syncTableToControlPoints() {
  // 将表格中编辑的数据同步到 control_points_
  for (int i = 0; i < ui->tableWidget_controlPoints->rowCount(); ++i) {
    if (i < static_cast<int>(control_points_.size())) {
      // 从表格读取数据
      QString x_str = ui->tableWidget_controlPoints->item(i, 1)->text();
      QString y_str = ui->tableWidget_controlPoints->item(i, 2)->text();
      QString yaw_str = ui->tableWidget_controlPoints->item(i, 3)->text();

      control_points_[i].x = x_str.toDouble();
      control_points_[i].y = y_str.toDouble();
      control_points_[i].yaw = yaw_str.toDouble();
    }
  }
}

void FormEditRoad::updateOpenDriveFormat() {
  ui->textEdit_opendrive->setPlainText(generateOpenDriveFormat());
}

QString FormEditRoad::generateOpenDriveFormat() const {
  if (fitted_segments_.empty() || control_points_.empty()) {
    return "";
  }

  QString result;
  double accumulated_s = 0.0;

  for (size_t i = 0; i < fitted_segments_.size(); ++i) {
    const auto& seg = fitted_segments_[i];

    // 获取该段的起点和终点
    const auto& p1 = control_points_[i];
    const auto& p2 = control_points_[i + 1];

    // 起点坐标
    double x = p1.x;
    double y = p1.y;

    // 航向角 (使用 p1 的 yaw)
    double hdg = p1.yaw;

    // 段长度 = 两点之间的欧氏距离
    double segment_length = std::hypot(p2.x - p1.x, p2.y - p1.y);
    double s = accumulated_s;
    double length = segment_length;

    // 生成 XML 格式
    // 注意：系数是基于 pRange="normalized" (p ∈ [0,1]) 计算的
    QString geometry;
    geometry += QString("    <geometry s=\"%1\" x=\"%2\" y=\"%3\" hdg=\"%4\" length=\"%5\">\n")
        .arg(s, 0, 'f', 3)
        .arg(x, 0, 'f', 3)
        .arg(y, 0, 'f', 3)
        .arg(hdg, 0, 'f', 6)
        .arg(length, 0, 'f', 3);

    geometry += QString("        <paramPoly3\n"
                        "        aU=\"%1\"\n"
                        "        bU=\"%2\"\n"
                        "        cU=\"%3\"\n"
                        "        dU=\"%4\"\n"
                        "        aV=\"%5\"\n"
                        "        bV=\"%6\"\n"
                        "        cV=\"%7\"\n"
                        "        dV=\"%8\"\n"
                        "        pRange=\"normalized\">\n"
                        "        </paramPoly3>\n"
                        "    </geometry>\n")
        .arg(seg.aU, 0, 'g', 15)
        .arg(seg.bU, 0, 'g', 15)
        .arg(seg.cU, 0, 'g', 15)
        .arg(seg.dU, 0, 'g', 15)
        .arg(seg.aV, 0, 'g', 15)
        .arg(seg.bV, 0, 'g', 15)
        .arg(seg.cV, 0, 'g', 15)
        .arg(seg.dV, 0, 'g', 15);

    result += geometry;
    accumulated_s += segment_length;
  }

  return result;
}

void FormEditRoad::clearAll() {
  control_points_.clear();
  original_segments_.clear();
  fitted_segments_.clear();
  selected_point_index_ = -1;

  updateControlPointTable();
  updateSegmentTable();
  updateOpenDriveFormat();
}
