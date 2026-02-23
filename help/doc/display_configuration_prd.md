# Display配置系统设计文档 (PRD)

## 1. 需求概述

在Qt界面中实现一个可视化配置系统，允许用户通过子窗口设置显示配置，主Display界面根据配置动态调整显示内容。

## 2. 设计目标

- 提供友好的配置界面，通过子窗口进行配置
- 支持实时更新显示内容，无需重启应用
- 配置项覆盖地图元素的显示/隐藏、颜色、线宽等属性
- 配置可持久化保存和加载

## 3. 系统架构设计

### 3.1 整体架构

```
┌─────────────────────────────────────────┐
│         Display (主窗口)                 │
│  ┌───────────────────────────────────┐  │
│  │   paintEvent()                    │  │
│  │   drawMap()                       │  │
│  │   └─> 读取 DisplayConfig          │  │
│  └───────────────────────────────────┘  │
└─────────────────────────────────────────┘
              ▲                    │
              │                    │
              │ 信号槽机制          │ 打开配置窗口
              │                    │
┌─────────────┴────────────────────┴──────┐
│    DisplayConfigDialog (配置子窗口)      │
│  ┌───────────────────────────────────┐  │
│  │   UI控件 (CheckBox, ColorPicker)  │  │
│  │   └─> 修改 DisplayConfig          │  │
│  └───────────────────────────────────┘  │
└─────────────────────────────────────────┘
              │
              │ 保存/加载
              ▼
┌─────────────────────────────────────────┐
│      DisplayConfig (配置数据结构)        │
│  - 显示元素开关                          │
│  - 颜色配置                              │
│  - 线宽配置                              │
│  - 其他显示属性                          │
└─────────────────────────────────────────┘
```

### 3.2 核心组件

#### 3.2.1 DisplayConfig (配置数据结构)

**位置**: `display/display_config/display_config.h` 和 `display/display_config/display_config.cc`

**职责**: 
- 存储所有显示相关的配置项
- 提供配置的序列化/反序列化接口（JSON格式）
- 提供配置变更通知机制

**数据结构设计**:

```cpp
struct DisplayConfig {
  // 显示元素开关
  struct ElementVisibility {
    bool show_lane_centerline = true;      // 车道中心线
    bool show_lane_left_boundary = true;   // 左边界
    bool show_lane_right_boundary = true;  // 右边界
    bool show_road = false;                // 道路
    bool show_intersection = false;        // 交叉口
    bool show_lane_id = false;             // 车道ID标签
    bool show_road_id = false;             // 道路ID标签
  } visibility;

  // 颜色配置
  struct ColorConfig {
    QColor lane_centerline_color = Qt::darkGray;
    QColor lane_left_boundary_color = Qt::black;
    QColor lane_right_boundary_color = Qt::black;
    QColor road_color = Qt::blue;
    QColor intersection_color = Qt::red;
    QColor background_color = Qt::white;
  } colors;

  // 线宽配置
  struct LineWidthConfig {
    double lane_centerline_width = 3.0;
    double lane_boundary_width = 4.0;
    double road_width = 2.0;
    double intersection_width = 2.0;
  } line_widths;

  // 线型配置
  struct LineStyleConfig {
    Qt::PenStyle lane_centerline_style = Qt::DotLine;
    Qt::PenStyle lane_boundary_style = Qt::SolidLine;
    Qt::PenStyle road_style = Qt::SolidLine;
  } line_styles;

  // 其他配置
  bool antialiasing = true;                // 抗锯齿
};
```

#### 3.2.2 DisplayConfigDialog (配置窗口)

**位置**: `display/display_config/display_config_dialog.h`, `display/display_config/display_config_dialog.cc`, `display/display_config/display_config_dialog.ui`

**职责**:
- 提供图形化配置界面
- 实时更新配置并通知Display窗口
- 支持配置的保存和加载

**UI设计**:
- 使用QDialog作为子窗口
- 使用QGroupBox分组显示不同类别的配置
- 使用QCheckBox控制显示/隐藏
- 使用QColorDialog或QPushButton+颜色预览选择颜色
- 使用QDoubleSpinBox设置线宽
- 使用QComboBox选择线型
- 提供"应用"、"确定"、"取消"按钮

**信号设计**:
```cpp
signals:
  void configChanged(const DisplayConfig& config);  // 配置变更信号
```

#### 3.2.3 Display类修改

**修改点**:
1. 添加DisplayConfig成员变量
2. 在构造函数中初始化默认配置
3. 连接配置窗口的信号到槽函数
4. 修改drawMap()函数，根据配置决定绘制内容
5. 添加菜单项或工具栏按钮打开配置窗口

**关键方法**:
```cpp
private slots:
  void onConfigChanged(const DisplayConfig& config);  // 响应配置变更
  void openConfigDialog();                             // 打开配置窗口

private:
  DisplayConfig config_;                              // 配置对象
  DisplayConfigDialog* config_dialog_ = nullptr;      // 配置窗口指针
```

## 4. 详细设计

### 4.1 配置数据结构实现

**文件**: `display/display_config.h`

```cpp
#ifndef DISPLAY_CONFIG_H
#define DISPLAY_CONFIG_H

#include <QColor>
#include <QPen>
#include <QJsonObject>
#include <QJsonDocument>

struct DisplayConfig {
  // ... 数据结构定义 ...
  
  // 序列化到JSON
  QJsonObject toJson() const;
  
  // 从JSON反序列化
  void fromJson(const QJsonObject& json);
  
  // 保存到文件
  bool saveToFile(const QString& filename) const;
  
  // 从文件加载
  bool loadFromFile(const QString& filename);
  
  // 获取默认配置
  static DisplayConfig defaultConfig();
};

#endif
```

### 4.2 配置窗口实现

**UI布局建议**:
```
┌─────────────────────────────────────┐
│  显示配置                            │
├─────────────────────────────────────┤
│  [ ] 显示车道中心线                    │
│  [ ] 显示左边界                        │
│  [ ] 显示右边界                        │
│  [ ] 显示道路                          │
│  [ ] 显示交叉口                        │
│  [ ] 显示车道ID                        │
│  [ ] 显示道路ID                        │
├─────────────────────────────────────┤
│  颜色配置                            │
│  车道中心线颜色: [颜色选择按钮]        │
│  左边界颜色: [颜色选择按钮]            │
│  右边界颜色: [颜色选择按钮]            │
│  道路颜色: [颜色选择按钮]              │
├─────────────────────────────────────┤
│  线宽配置                            │
│  车道中心线宽度: [SpinBox]            │
│  边界线宽度: [SpinBox]                │
├─────────────────────────────────────┤
│  其他配置                            │
│  [ ] 启用抗锯齿                       │
├─────────────────────────────────────┤
│        [应用] [确定] [取消] [保存]    │
└─────────────────────────────────────┘
```

#### 4.2.1 DisplayConfigDialog获取DisplayConfig数据的过程

**数据获取流程**:

DisplayConfigDialog通过以下方式获取和操作DisplayConfig数据：

**方式1: 构造函数传入（推荐）**

在Display类中打开配置窗口时，将当前的配置对象传递给对话框：

```cpp
// display_config_dialog.h
class DisplayConfigDialog : public QDialog {
  Q_OBJECT

public:
  // 方式1: 通过构造函数传入配置（推荐）
  explicit DisplayConfigDialog(const DisplayConfig& config, 
                                QWidget* parent = nullptr);
  
  // 方式2: 通过setter方法设置配置
  void setConfig(const DisplayConfig& config);
  
  // 获取当前配置（用于取消操作时恢复）
  DisplayConfig getConfig() const;

signals:
  void configChanged(const DisplayConfig& config);

private slots:
  void onApplyClicked();      // 应用按钮
  void onOkClicked();         // 确定按钮
  void onCancelClicked();     // 取消按钮
  void onSaveClicked();       // 保存按钮

private:
  void loadConfigToUI(const DisplayConfig& config);  // 将配置加载到UI控件
  DisplayConfig getConfigFromUI() const;              // 从UI控件获取配置
  
  DisplayConfig current_config_;      // 当前显示的配置
  DisplayConfig original_config_;     // 原始配置（用于取消操作）
  
  // UI控件指针（通过ui->访问）
  QCheckBox* checkbox_lane_centerline_;
  QCheckBox* checkbox_left_boundary_;
  // ... 其他UI控件
};
```

**实现细节**:

```cpp
// display_config_dialog.cc

// 构造函数：接收配置并初始化UI
DisplayConfigDialog::DisplayConfigDialog(const DisplayConfig& config, 
                                         QWidget* parent)
    : QDialog(parent), 
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
}

// 将DisplayConfig数据加载到UI控件
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
  
  // 颜色配置 - 更新颜色按钮的背景色
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
  
  // 其他配置
  ui->checkboxAntialiasing->setChecked(config.antialiasing);
}

// 从UI控件获取配置数据
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
  
  // 从UI控件读取其他配置
  config.antialiasing = ui->checkboxAntialiasing->isChecked();
  
  return config;
}

// 应用按钮：立即应用配置并发送信号
void DisplayConfigDialog::onApplyClicked() {
  current_config_ = getConfigFromUI();
  emit configChanged(current_config_);
}

// 确定按钮：应用配置、发送信号并关闭窗口
void DisplayConfigDialog::onOkClicked() {
  current_config_ = getConfigFromUI();
  emit configChanged(current_config_);
  accept();  // 关闭对话框并返回QDialog::Accepted
}

// 取消按钮：恢复原始配置并关闭窗口
void DisplayConfigDialog::onCancelClicked() {
  // 恢复原始配置到UI（可选，因为窗口会关闭）
  loadConfigToUI(original_config_);
  reject();  // 关闭对话框并返回QDialog::Rejected
}

// 保存按钮：保存配置到文件
void DisplayConfigDialog::onSaveClicked() {
  current_config_ = getConfigFromUI();
  QString filename = QFileDialog::getSaveFileName(
      this, "保存配置", "", "JSON Files (*.json)");
  if (!filename.isEmpty()) {
    current_config_.saveToFile(filename);
  }
}

// 辅助函数：更新颜色按钮的显示
void DisplayConfigDialog::updateColorButton(QPushButton* button, 
                                            const QColor& color) {
  QPixmap pixmap(20, 20);
  pixmap.fill(color);
  button->setIcon(QIcon(pixmap));
  button->setProperty("color", color);  // 存储颜色数据
}

// 辅助函数：从颜色按钮获取颜色
QColor DisplayConfigDialog::getColorFromButton(QPushButton* button) const {
  return button->property("color").value<QColor>();
}
```

**在Display类中的使用**:

```cpp
// display.cpp

void Display::openConfigDialog() {
  // 如果对话框不存在，创建它
  if (config_dialog_ == nullptr) {
    config_dialog_ = new DisplayConfigDialog(config_, this);
    
    // 连接信号槽：当配置变更时更新Display的配置
    connect(config_dialog_, &DisplayConfigDialog::configChanged,
            this, &Display::onConfigChanged);
  } else {
    // 如果对话框已存在，更新其配置数据
    config_dialog_->setConfig(config_);
  }
  
  // 显示对话框（模态或非模态）
  config_dialog_->exec();  // 模态显示
  // 或
  // config_dialog_->show();  // 非模态显示
}

void Display::onConfigChanged(const DisplayConfig& config) {
  config_ = config;  // 更新Display的配置
  update();          // 触发重绘
}
```

**模态与非模态对话框的区别**:

**模态对话框 (Modal Dialog)** - 使用 `exec()`:
- **阻塞特性**: 当对话框显示时，会**阻塞**主窗口的交互，用户必须先关闭对话框才能继续操作主窗口
- **执行流程**: `exec()` 函数会**暂停**当前函数的执行，直到对话框关闭才返回
- **返回值**: `exec()` 返回 `QDialog::Accepted` 或 `QDialog::Rejected`，可以判断用户是点击了"确定"还是"取消"
- **适用场景**: 
  - 需要用户必须做出选择才能继续的场景（如确认对话框）
  - 配置窗口如果希望用户专注于配置，完成后才能继续操作主窗口
- **用户体验**: 用户无法在配置窗口打开时操作主窗口，避免误操作

**示例**:
```cpp
void Display::openConfigDialog() {
  if (config_dialog_ == nullptr) {
    config_dialog_ = new DisplayConfigDialog(config_, this);
    connect(config_dialog_, &DisplayConfigDialog::configChanged,
            this, &Display::onConfigChanged);
  }
  
  // 模态显示：阻塞执行，直到对话框关闭
  int result = config_dialog_->exec();
  if (result == QDialog::Accepted) {
    // 用户点击了"确定"
    qDebug() << "配置已应用";
  } else {
    // 用户点击了"取消"
    qDebug() << "配置已取消";
  }
  // 只有对话框关闭后，代码才会继续执行到这里
}
```

**非模态对话框 (Modeless Dialog)** - 使用 `show()`:
- **非阻塞特性**: 对话框显示时，**不会阻塞**主窗口，用户可以同时操作主窗口和对话框
- **执行流程**: `show()` 函数**立即返回**，不会等待对话框关闭
- **返回值**: `show()` 没有返回值（void），无法直接判断用户操作
- **适用场景**:
  - 需要同时查看主窗口和配置窗口的场景
  - 配置窗口可以保持打开，用户可以在主窗口和配置窗口之间切换
  - 实时预览配置效果（配置窗口保持打开，主窗口实时更新）
- **用户体验**: 用户可以同时操作主窗口和配置窗口，更加灵活

**示例**:
```cpp
void Display::openConfigDialog() {
  if (config_dialog_ == nullptr) {
    config_dialog_ = new DisplayConfigDialog(config_, this);
    connect(config_dialog_, &DisplayConfigDialog::configChanged,
            this, &Display::onConfigChanged);
  }
  
  // 非模态显示：立即返回，不阻塞
  config_dialog_->show();
  // 代码会立即继续执行到这里，不会等待对话框关闭
  
  // 如果需要检查对话框是否可见
  if (config_dialog_->isVisible()) {
    qDebug() << "配置窗口已显示";
  }
}
```

**对比总结**:

| 特性 | 模态对话框 (`exec()`) | 非模态对话框 (`show()`) |
|------|---------------------|----------------------|
| **阻塞性** | 阻塞主窗口 | 不阻塞主窗口 |
| **函数返回** | 等待对话框关闭 | 立即返回 |
| **返回值** | 返回 `Accepted`/`Rejected` | 无返回值 |
| **用户交互** | 必须先关闭对话框 | 可同时操作主窗口 |
| **适用场景** | 必须做出选择 | 实时预览、灵活操作 |
| **内存管理** | 对话框关闭后可能需要重新创建 | 对话框可保持打开状态 |

**本项目的推荐方案**:

对于配置窗口，推荐使用**非模态对话框** (`show()`)，原因：
1. **实时预览**: 用户可以在配置窗口调整参数，主窗口实时显示效果
2. **灵活操作**: 用户可以同时查看主窗口的地图显示和配置窗口的设置
3. **更好的用户体验**: 配置窗口可以保持打开，用户可以在两者之间切换

**实现示例**:
```cpp
void Display::openConfigDialog() {
  if (config_dialog_ == nullptr) {
    config_dialog_ = new DisplayConfigDialog(config_, this);
    connect(config_dialog_, &DisplayConfigDialog::configChanged,
            this, &Display::onConfigChanged);
    
    // 设置对话框为非模态（可选，show()默认就是非模态）
    config_dialog_->setModal(false);
  } else {
    config_dialog_->setConfig(config_);
  }
  
  // 非模态显示：允许用户同时操作主窗口和配置窗口
  config_dialog_->show();
  config_dialog_->raise();   // 将窗口置于最前
  config_dialog_->activateWindow();  // 激活窗口
}
```

**数据流向图**:

```
Display类 (主窗口)
  │
  │ 1. 打开配置窗口时
  │    config_dialog_ = new DisplayConfigDialog(config_, this)
  │    └─> 将当前config_传递给对话框
  │
  ▼
DisplayConfigDialog (配置窗口)
  │
  │ 2. 构造函数中
  │    loadConfigToUI(config)
  │    └─> 将DisplayConfig数据填充到UI控件
  │
  │ 3. 用户修改UI控件
  │    └─> UI控件状态改变
  │
  │ 4. 用户点击"应用"或"确定"
  │    getConfigFromUI()
  │    └─> 从UI控件读取数据，构建DisplayConfig
  │
  │ 5. 发送信号
  │    emit configChanged(current_config_)
  │
  ▼
Display类接收信号
  │
  │ 6. 槽函数响应
  │    onConfigChanged(const DisplayConfig& config)
  │    └─> 更新config_成员变量
  │
  │ 7. 触发重绘
  │    update()
  │    └─> paintEvent() -> drawMap() -> 使用新的config_
```

**关键设计要点**:

1. **数据所有权**: Display类拥有配置数据的所有权，配置窗口只是临时编辑
2. **数据同步**: 通过构造函数参数或setter方法将配置传入对话框
3. **数据提取**: 通过`getConfigFromUI()`从UI控件提取配置数据
4. **数据回传**: 通过信号槽机制将修改后的配置传回Display类
5. **取消操作**: 保存原始配置，取消时恢复
6. **实时更新**: "应用"按钮支持实时预览，无需关闭窗口

### 4.3 Display类集成

**修改drawMap()函数**:
```cpp
void Display::drawMap(QPainter &painter) {
  if (map_ == nullptr) return;
  // to do
}
```

### 4.4 信号槽连接

```cpp
// 在Display构造函数中
connect(config_dialog_, &DisplayConfigDialog::configChanged,
        this, &Display::onConfigChanged);

// 槽函数实现
void Display::onConfigChanged(const DisplayConfig& config) {
  config_ = config;
  update();  // 触发重绘
}
```

## 5. 实现步骤

### 阶段1: 基础数据结构
1. 创建`display_config.h`和`display_config.cc`
2. 定义DisplayConfig数据结构
3. 实现JSON序列化/反序列化

### 阶段2: 配置窗口
1. 使用Qt Designer创建`display_config_dialog.ui`
2. 创建`display_config_dialog.h`和`display_config_dialog.cc`
3. 实现UI控件与配置数据的绑定
4. 实现配置变更信号

### 阶段3: Display集成
1. 在Display类中添加DisplayConfig成员
2. 修改drawMap()函数，根据配置绘制
3. 添加打开配置窗口的入口（菜单或工具栏）
4. 实现信号槽连接

### 阶段4: 功能完善
1. 实现配置的保存/加载功能
2. 添加配置预设功能（可选）
3. 优化UI交互体验
4. 添加配置验证和错误处理

## 6. 扩展性考虑

### 6.1 未来可扩展的配置项
- 图层管理（不同元素在不同图层）
- 缩放级别相关的显示控制
- 动画效果配置
- 性能优化选项（如LOD - Level of Detail）

### 6.2 插件化配置
- 支持配置插件，允许第三方扩展配置项
- 配置项的动态注册机制

## 7. 技术要点

### 7.1 Qt信号槽机制
- 使用信号槽实现配置窗口与Display窗口的解耦
- 配置变更时自动触发重绘

### 7.2 配置持久化
- 使用JSON格式存储配置
- 默认配置文件位置：`~/.opendrive_editor/config.json`
- 支持多配置文件（预设）

### 7.3 性能考虑
- 配置变更时只重绘必要的部分（如果可能）
- 大量元素时考虑使用QGraphicsView替代QPainter

## 8. 测试要点

1. 配置窗口的打开/关闭
2. 配置项的修改和实时更新
3. 配置的保存和加载
4. 边界值测试（如线宽为0或负数）
5. 多窗口场景下的配置同步

## 9. 用户交互流程

```
用户操作流程:
1. 用户点击菜单"视图" -> "显示配置" 或工具栏按钮
2. 弹出配置子窗口
3. 用户修改配置项（勾选/取消、选择颜色、调整线宽等）
4. 用户点击"应用"按钮 -> 立即生效，窗口保持打开
5. 用户点击"确定"按钮 -> 生效并关闭窗口
6. 用户点击"取消"按钮 -> 放弃修改并关闭窗口
7. 用户点击"保存"按钮 -> 保存配置到文件
```

## 10. 文件清单

新增文件:
- `display/display_config/display_config.h` - 配置数据结构头文件
- `display/display_config/display_config.cc` - 配置数据结构实现
- `display/display_config/display_config_dialog.h` - 配置窗口头文件
- `display/display_config/display_config_dialog.cc` - 配置窗口实现
- `display/display_config/display_config_dialog.ui` - 配置窗口UI文件

修改文件:
- `display/display.h` - 添加配置相关成员和方法
- `display/display.cpp` - 集成配置系统
- `display/draw_map.cc` - 根据配置绘制
- `display/display.ui` - 添加配置菜单项或工具栏按钮
- `display/CMakeLists.txt` - 添加新文件到构建系统

## 11. 总结

本设计采用经典的MVC模式：
- **Model**: DisplayConfig（配置数据）
- **View**: DisplayConfigDialog（配置界面）
- **Controller**: Display类（协调配置和应用）

通过信号槽机制实现松耦合，配置变更可以实时反映到显示界面，提供了良好的用户体验和系统可扩展性。

