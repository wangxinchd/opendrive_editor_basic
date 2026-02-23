#ifndef DISPLAY_CONFIG_DIALOG_H
#define DISPLAY_CONFIG_DIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QColor>
#include "display_config.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class DisplayConfigDialog;
}
QT_END_NAMESPACE

class DisplayConfigDialog : public QDialog {
  Q_OBJECT

public:
  explicit DisplayConfigDialog(const DisplayConfig& config, 
                                QWidget* parent = nullptr);
  ~DisplayConfigDialog();

  // 设置配置
  void setConfig(const DisplayConfig& config);
  
  // 获取当前配置
  DisplayConfig getConfig() const;

signals:
  void configChanged(const DisplayConfig& config);

private slots:
  void onApplyClicked();      // 应用按钮
  void onOkClicked();         // 确定按钮
  void onCancelClicked();     // 取消按钮
  void onSaveClicked();       // 保存按钮
  
  // 颜色选择按钮槽函数
  void onLaneCenterlineColorClicked();
  void onLeftBoundaryColorClicked();
  void onRightBoundaryColorClicked();
  void onRoadColorClicked();

private:
  void loadConfigToUI(const DisplayConfig& config);  // 将配置加载到UI控件
  DisplayConfig getConfigFromUI() const;              // 从UI控件获取配置
  
  void updateColorButton(QPushButton* button, const QColor& color);
  QColor getColorFromButton(QPushButton* button) const;
  
  Ui::DisplayConfigDialog *ui;
  DisplayConfig current_config_;      // 当前显示的配置
  DisplayConfig original_config_;     // 原始配置（用于取消操作）
};

#endif // DISPLAY_CONFIG_DIALOG_H

