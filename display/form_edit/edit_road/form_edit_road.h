#ifndef FORM_EDIT_ROAD_H
#define FORM_EDIT_ROAD_H

#include <QWidget>
#include <vector>
#include <QTableWidgetItem>

// 前向声明
namespace common {
struct Pose2d;
struct OpenDriveParamPoly3;
}

namespace Ui {
class FormEditRoad;
}

class FormEditRoad : public QWidget {
  Q_OBJECT

public:
  explicit FormEditRoad(QWidget *parent = nullptr);
  ~FormEditRoad();

  /**
   * @brief 设置道路列表
   * @param road_names 道路名称列表
   */
  void setRoadList(const QStringList& road_names);

  /**
   * @brief 设置当前文件路径
   * @param file_path 文件路径
   */
  void setCurrentFile(const QString& file_path);

  /**
   * @brief 设置 ParamPoly3 系数（多段）
   * @param segments 各段系数
   */
  void setParamPoly3Segments(const std::vector<common::OpenDriveParamPoly3>& segments);

  /**
   * @brief 设置控制点
   * @param control_points 控制点列表
   */
  void setControlPoints(const std::vector<common::Pose2d>& control_points);

  /**
   * @brief 获取当前控制点
   * @return 控制点列表
   */
  std::vector<common::Pose2d> getControlPoints() const;

  /**
   * @brief 获取拟合后的 ParamPoly3 系数
   * @return 各段系数
   */
  std::vector<common::OpenDriveParamPoly3> getFittedSegments() const;

  /**
   * @brief 生成 OpenDRIVE 格式的 XML 字符串
   * @return XML 格式字符串
   */
  QString generateOpenDriveFormat() const;

signals:
  /**
   * @brief 道路选择变化信号
   * @param index 选择的道路索引
   */
  void roadSelected(int index);

  /**
   * @brief 拟合请求信号
   * @param control_points 控制点列表
   */
  void fitRequested(const std::vector<common::Pose2d>& control_points);

  /**
   * @brief 系数变化信号（预览）
   * @param segments 各段系数
   */
  void coeffChanged(const std::vector<common::OpenDriveParamPoly3>& segments);

  /**
   * @brief 保存请求信号
   * @param segments 拟合后的系数
   */
  void saveRequested(const std::vector<common::OpenDriveParamPoly3>& segments);

  /**
   * @brief 取消信号
   */
  void cancelRequested();

  /**
   * @brief 重置信号
   */
  void resetRequested();

private slots:
  // 道路选择
  void onRoadSelected(int index);

  // 控制点操作
  void onAddPointClicked();
  void onDeletePointClicked();
  void onMoveUpClicked();
  void onMoveDownClicked();
  void onControlPointClicked(int row, int column);

  // 拟合操作
  void onFitClicked();
  void onSaveClicked();
  void onCancelClicked();
  void onResetClicked();

private:
  Ui::FormEditRoad *ui;

  // 数据
  std::vector<common::Pose2d> control_points_;
  std::vector<common::OpenDriveParamPoly3> original_segments_;
  std::vector<common::OpenDriveParamPoly3> fitted_segments_;
  int selected_point_index_ = -1;

  // 辅助函数
  void updateControlPointTable();
  void updateSegmentTable();
  void updateOpenDriveFormat();
  void syncTableToControlPoints();
  void clearAll();
};

#endif // FORM_EDIT_ROAD_H
