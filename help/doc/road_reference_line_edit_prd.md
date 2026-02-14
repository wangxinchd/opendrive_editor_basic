# 道路参考线编辑功能设计文档 (PRD)

## 1. 需求概述

在OpenDRIVE地图编辑器中实现道路参考线的编辑功能。专注于 **ParamPoly3（参数多项式）** 类型的参考线编辑，通过用户提供的一系列姿态点（包含 x, y, yaw 航向角）来拟合三次多项式曲线。

## 2. 现有系统架构

### 2.1 当前地图加载流程

```
┌─────────────────┐     ┌──────────────────────┐     ┌─────────────────┐
│  xodr 文件      │────▶│ OpenDriveMapParser  │────▶│ MapBase::Map   │
│  (OpenDRIVE格式)│     │ (工厂模式转换)       │     │ (内部地图结构)  │
└─────────────────┘     └──────────────────────┘     └─────────────────┘
                                                          │
                                                          ▼
                                                 ┌─────────────────┐
                                                 │ Display         │
                                                 │ (地图渲染)      │
                                                 └─────────────────┘
```

### 2.2 关键数据流

1. **加载阶段**: `Display::openXodr()` → `MapParserFactory` → `OpenDriveMapParser::Parse()` → `MapBase::Map`
2. **渲染阶段**: `Display::paintEvent()` → `Display::drawMap()` → 读取 `MapBase::Map` 数据

### 2.3 现有 ParamPoly3 数据结构

**RoadManager 中的 ParamPoly3 类** (`RoadManager/RoadManager.hpp`):

```cpp
class ParamPoly3 : public Geometry
{
public:
    // 三次多项式: U(p) = a + b*p + c*p² + d*p³
    Polynomial poly3U_;  // 横向偏移
    Polynomial poly3V_;  // 纵向偏移
};

class Polynomial
{
    double a_, b_, c_, d_;  // 三次多项式系数
    double scale_;           // 缩放因子
};
```

**公式**:
```
U(p) = aU + bU*p + cU*p² + dU*p³
V(p) = aV + bV*p + cV*p² + dV*p³

其中 p ∈ [0, length] 或 p ∈ [0, 1] (normalized)
```

## 3. 功能需求

### 3.1 核心功能

1. **道路选择**
   - 通过下拉框选择要编辑的 **ParamPoly3** 类型道路
   - 在地图上高亮显示选中道路的参考线
   - 非 ParamPoly3 类型的道路禁用编辑

2. **姿态点管理**
   - 显示当前道路的 ParamPoly3 参数（aU-dU, aV-dV）
   - 添加/删除/移动姿态点
   - 姿态点包含：x（世界坐标X）, y（世界坐标Y）, yaw（航向角）

3. **多项式拟合**
   - 根据姿态点序列拟合三次多项式曲线
   - 支持两种约束条件：
     - **点约束**：曲线必须经过姿态点
     - **切线约束**：曲线在姿态点处的切线方向必须匹配 yaw
   - 使用分段三次 Hermite 插值

4. **预览与保存**
   - 实时预览拟合后的 ParamPoly3 曲线
   - 预览满意后，覆盖写回原xodr文件
   - 自动重新加载文件，更新地图显示

### 3.2 用户交互流程

```
┌──────────────────────────────────────────────────────────────────┐
│                           用户操作流程                             │
├──────────────────────────────────────────────────────────────────┤
│                                                                  │
│  1. 点击菜单 "Edit" -> "Edit Road Reference Line"              │
│     ┌─────────────────────────────────────────┐                  │
│     │ 道路编辑面板 (FormEditRoad)            │                  │
│     │  - 道路下拉框 (仅显示ParamPoly3道路)   │                  │
│     │  - 当前分段参数方程显示 (n-1段)        │                  │
│     │  - 姿态点列表                         │                  │
│     │  - 操作按钮                           │                  │
│     └─────────────────────────────────────────┘                  │
│                           │                                      │
│                           ▼                                      │
│  2. 从下拉框选择 ParamPoly3 类型的道路                         │
│     → 解析该道路的 ParamPoly3 参数（可能有多个分段）          │
│     → 显示各分段参数方程                                       │
│     → 反算姿态点（根据各段参数生成采样点）                   │
│                           │                                      │
│                           ▼                                      │
│  3. 添加/编辑/删除姿态点                                      │
│     → 在表格中修改姿态点坐标和航向角                          │
│     → 或在地图上通过鼠标交互添加/拖拽                         │
│                           │                                      │
│                           ▼                                      │
│  4. 点击 "拟合曲线"                                           │
│     → 根据姿态点拟合分段三次多项式（共 n-1 段）             │
│     → 实时在地图上预览效果                                   │
│                           │                                      │
│                           ▼                                      │
│  5. 点击 "确定"                                               │
│     → 弹出保存确认对话框                                      │
│     → 确认后覆盖写原xodr文件                                 │
│     → 自动重新加载文件                                        │
│     → 地图更新显示新数据                                      │
│                                                                  │
└──────────────────────────────────────────────────────────────────┘
```

## 4. 数学基础

### 4.1 核心数据结构

**文件**: `common/odr_geometry.h`

```cpp
#include "point.h"

namespace common {

// OpenDRIVE ParamPoly3 表示
struct OpenDriveParamPoly3 {
  double aU = 0.0;  // U(p) = aU + bU*p + cU*p² + dU*p³
  double bU = 0.0;
  double cU = 0.0;
  double dU = 0.0;

  double aV = 0.0;  // V(p) = aV + bV*p + cV*p² + dV*p³
  double bV = 0.0;
  double cV = 0.0;
  double dV = 0.0;
};

// 姿态点（来自 common/point.h）
struct Pose2d {
  double x = 0.0;
  double y = 0.0;
  double yaw = 0.0; // rad

  Pose2d() = default;
  Pose2d(double x, double y, double yaw) : x(x), y(y), yaw(yaw) {}
};

// OpenDRIVE 几何线形生成器
class OdrGeometry {
public:
  // 使用分段三次 Hermite 插值生成几何线形
  // n 个姿态 → n-1 段曲线
  static std::vector<OpenDriveParamPoly3> GenerateFromPoses(
      const std::vector<Pose2d>& poses
  );
};
```

**文件**: `common/cubic_curve_fitter.h`

```cpp
// 三次曲线拟合器（Hermite 插值）
class CubicCurveFitter {
public:
  using Vec2 = std::array<double,2>;

  // 构造函数：输入两个姿态点
  CubicCurveFitter(double x0, double y0, double yaw0,
                   double x1, double y1, double yaw1);

  // 评估曲线 t ∈ [0, 1]
  Vec2 evaluate(double t) const;

  // 弧长重参数化
  void reparameterizeArcLength(std::vector<double>& s_vals,
                               std::vector<Vec2>& uv_vals,
                               double& total_length,
                               int n = 2000) const;

  // 世界坐标转局部坐标
  static Vec2 worldToLocal(double x, double y,
                           double x0, double y0,
                           double hdg);

  // 从 uv_vals 中提取 U 分量
  static std::vector<double> extractU(const std::vector<Vec2>& uv_vals);

  // 从 uv_vals 中提取 V 分量
  static std::vector<double> extractV(const std::vector<Vec2>& uv_vals);

  // 三次多项式拟合（最小二乘）
  static std::array<double,4> fitCubic(const std::vector<double>& s_norm,
                                       const std::vector<double>& vals);
};
```

### 4.2 分段拟合流程

```
姿态点序列: Pose0, Pose1, Pose2, ..., Posen

分段拟合:
  段0: Pose0 → Pose1  → ParamPoly3_0
  段1: Pose1 → Pose2  → ParamPoly3_1
  段2: Pose2 → Pose3  → ParamPoly3_2
  ...
  段(n-1): Pose(n-1) → Posen → ParamPoly3_(n-1)

总段数 = 姿态点数量 - 1
```

**实现步骤**:

```cpp
// 伪代码：多姿态点拟合
std::vector<OpenDriveParamPoly3> OdrGeometry::GenerateFromPoses(
    const std::vector<Pose2d>& poses
) {
    std::vector<OpenDriveParamPoly3> result;

    if (poses.size() < 2) {
        return result;  // 需要至少2个姿态点
    }

    // 分段拟合：每对相邻姿态点之间用三次 Hermite 插值
    for (size_t i = 0; i < poses.size() - 1; i++) {
        const auto& p1 = poses[i];
        const auto& p2 = poses[i + 1];

        // 创建三次曲线拟合器
        CubicCurveFitter curve(p1.x, p1.y, p1.yaw, p2.x, p2.y, p2.yaw);

        // 弧长重参数化
        std::vector<double> s_vals;
        std::vector<CubicCurveFitter::Vec2> uv_vals;
        double total_length;
        curve.reparameterizeArcLength(s_vals, uv_vals, total_length);

        // 提取 U 和 V 分量
        auto u_vals = CubicCurveFitter::extractU(uv_vals);
        auto v_vals = CubicCurveFitter::extractV(uv_vals);

        // 拟合三次多项式
        auto coef_U = CubicCurveFitter::fitCubic(s_vals, u_vals);
        auto coef_V = CubicCurveFitter::fitCubic(s_vals, v_vals);

        // 设置 OpenDRIVE ParamPoly3 系数
        OpenDriveParamPoly3 param_poly;
        param_poly.aU = coef_U[3];  // a (p^3)
        param_poly.bU = coef_U[2];  // b (p^2)
        param_poly.cU = coef_U[1];  // c (p^1)
        param_poly.dU = coef_U[0];  // d (p^0)

        param_poly.aV = coef_V[3];
        param_poly.bV = coef_V[2];
        param_poly.cV = coef_V[1];
        param_poly.dV = coef_V[0];

        result.push_back(param_poly);
    }

    return result;
}
```

### 4.3 三次 Hermite 插值原理

CubicCurveFitter 内部使用三次 Hermite 插值：

```cpp
// 三次 Hermite 插值公式
// 已知 p=0 和 p=1 处的值和导数，求三次多项式系数
// f(p) = a*p^3 + b*p^2 + c*p + d
// f(0) = y0,  f(1) = y1
// f'(0) = m0, f'(1) = m1

// 系数解法：
d = y0
c = m0
b = -3*y0 - 2*m0 + 3*y1 - m1
a = 2*y0 + m0 - 2*y1 + m1
```

### 4.4 拟合算法选择

| 方法 | 优点 | 缺点 | 适用场景 |
|------|------|------|----------|
| 分段 Hermite 插值 | 曲线经过所有姿态点，用户指定切线 | 需要显式切线 (yaw) | 姿态点精确，用户可控制曲线形态 |
| 最小二乘法 | 抗噪声 | 曲线不一定经过姿态点 | 数据有测量误差 |
| Catmull-Rom | 切线自动计算，连续 | 需要多姿态点 | 无需指定切线 |

**推荐**：使用**分段三次 Hermite 插值**，因为：
1. 曲线通过所有姿态点
2. 用户可通过 yaw 直接控制每个点的切线方向
3. 使用 `OdrGeometry::GenerateFromPoses` 统一接口

## 5. UI/UX 设计

### 5.1 道路编辑面板布局

```
┌────────────────────────────────────────────────────────────────┐
│  道路参考线编辑 (ParamPoly3)                      [_][□][X]   │
├────────────────────────────────────────────────────────────────┤
│                                                                │
│  道路选择: ┌─────────────────────────────────────┐ [▼]        │
│            │ Road 1 - ParamPoly3                 │            │
│            └─────────────────────────────────────┘            │
│                                                                │
│  ───────────────────────────────────────────────────────────  │
│                                                                │
│  当前 ParamPoly3 系数 (共 n-1 段):                                │
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │ [段1] U(p) = aU + bU*p + cU*p² + dU*p³   p∈[0, L1]       │ │
│  │       aU: 0.0    bU: 0.0    cU: 0.0    dU: 0.0            │ │
│  │       V(p) = aV + bV*p + cV*p² + dV*p³                    │ │
│  │       aV: 0.0    bV: 10.0   cV: 0.0    dV: 0.0            │ │
│  ├─────────────────────────────────────────────────────────────┤ │
│  │ [段2] U(p) = aU + bU*p + cU*p² + dU*p³   p∈[L1, L1+L2]   │ │
│  │       aU: 0.0    bU: 0.0    cU: 0.0    dU: 0.0            │ │
│  │       V(p) = aV + bV*p + cV*p² + dV*p³                    │ │
│  │       aV: 0.0    bV: 15.0   cV: 0.0    dV: 0.0            │ │
│  ├─────────────────────────────────────────────────────────────┤ │
│  │ ...                                                         │ │
│  └─────────────────────────────────────────────────────────────┘ │
│                                                                │
│  ───────────────────────────────────────────────────────────  │
│                                                                │
│  姿态点管理:                                                   │
│  ┌─────────────────────────────────────────────────────────┐  │
│  │ [+添加] [-删除] [↑上移] [↓下移]                         │  │
│  │                                                          │  │
│  │ 姿态点列表:                                              │  │
│  │ ┌───┬───────────┬───────────┬───────────┐              │  │
│  │ │ # │    X      │    Y      │  yaw(°)  │              │  │
│  │ ├───┼───────────┼───────────┼───────────┤              │  │
│  │ │ 1 │  0.0     │  0.0     │   0.0    │ ◀ 选中      │  │
│  │ │ 2 │  10.5    │  5.2     │   15.0   │              │  │
│  │ │ 3 │  25.3    │  -1.8    │   10.0   │              │  │
│  │ └───┴───────────┴───────────┴───────────┘              │  │
│  │                                                          │  │
│  │ 选中姿态点坐标:                                          │  │
│  │ X: [────────●─────] 0.0    Y: [─────●──────] 0.0      │  │
│  │ yaw: [────────●─────] 0.0  度                         │  │
│  │                                          [更新选中点]   │  │
│  └─────────────────────────────────────────────────────────┘  │
│                                                                │
│  ───────────────────────────────────────────────────────────  │
│                                                                │
│  拟合选项:                                                     │
│  ┌─────────────────────────────────────────────────────────┐  │
│  │ ● 分段三次 Hermite 插值 (推荐)                          │  │
│  │ ○ 最小二乘法                                            │  │
│  │                                                          │  │
│  │ 采样点数: [100]                                         │  │
│  └─────────────────────────────────────────────────────────┘  │
│                                                                │
│  ───────────────────────────────────────────────────────────  │
│                                                                │
│  当前文件: /path/to/map.xodr                                  │
│                                                                │
│              [拟合曲线] [确定] [取消] [重置]                  │
│                                                                │
└────────────────────────────────────────────────────────────────┘
```

### 5.2 交互流程详细说明

#### 5.2.1 各控件功能说明

| 控件名称 | 类型 | 功能说明 |
|----------|------|----------|
| **道路选择下拉框** | QComboBox | 列出当前地图中所有 ParamPoly3 类型的道路，供用户选择 |
| **系数显示区域** | QGroupBox | 只读显示当前道路的各分段 ParamPoly3 系数，每段有8个系数 (aU-dU, aV-dV)，显示段号和 p 的范围 |
| **[+添加]** | QPushButton | 在姿态点列表末尾添加一个新的姿态点，默认坐标为 (0, 0, 0) |
| **[-删除]** | QPushButton | 删除当前选中的姿态点（高亮行），无选中时禁用 |
| **[↑上移]** | QPushButton | 将选中姿态点在列表中上移一行，用于调整顺序 |
| **[↓下移]** | QPushButton | 将选中姿态点在列表中下移一行，用于调整顺序 |
| **姿态点表格** | QTableWidget | 显示所有姿态点，列：序号(#)、X、Y、yaw(rad)，双击单元格可编辑 |
| **选中姿态点编辑** | QDoubleSpinBox | 当表格中有选中行时，显示并可编辑该点的 X、Y、yaw 值 |
| **[更新选中点]** | QPushButton | 将编辑框中的值更新到表格中选中的姿态点 |
| **拟合方法单选** | QRadioButton | 选择拟合算法：分段三次 Hermite 插值 或 最小二乘法 |
| **采样点数** | QSpinBox | 拟合时生成曲线的采样点数量，默认100 |
| **当前文件路径** | QLineEdit | 只读显示当前打开的 xodr 文件完整路径 |
| **[拟合曲线]** | QPushButton | 根据当前姿态点拟合曲线，触发地图预览 |
| **[确定]** | QPushButton | 确认保存修改，覆盖写回 xodr 文件并重新加载 |
| **[取消]** | QPushButton | 取消当前编辑，关闭面板，不保存任何修改 |
| **[重置]** | QPushButton | 重置为道路原始的 ParamPoly3 系数和原始姿态点 |

## 6. 技术设计

### 6.1 模块划分

#### 6.1.1 OdrGeometry (几何线形生成器)

**文件位置**: `common/odr_geometry.h`

```cpp
#include "odr_geometry.h"
#include "point.h"

// OdrGeometry 封装
class OdrGeometry {
public:
    // 使用分段三次 Hermite 插值进行拟合
    // poses: (x, y, yaw) 姿态点序列
    // 返回: OpenDriveParamPoly3 系数列表
    static std::vector<OpenDriveParamPoly3> GenerateFromPoses(
        const std::vector<Pose2d>& poses
    );
};
```

**实现**:

```cpp
std::vector<OpenDriveParamPoly3> OdrGeometry::GenerateFromPoses(
    const std::vector<Pose2d>& poses
) {
    std::vector<OpenDriveParamPoly3> result;

    if (poses.size() < 2) {
        return result;  // 需要至少2个姿态点
    }

    // 分段拟合：每对相邻姿态点之间用三次 Hermite 插值
    for (size_t i = 0; i < poses.size() - 1; i++) {
        const auto& p1 = poses[i];
        const auto& p2 = poses[i + 1];

        // 创建三次曲线拟合器
        CubicCurveFitter curve(p1.x, p1.y, p1.yaw, p2.x, p2.y, p2.yaw);

        // 弧长重参数化
        std::vector<double> s_vals;
        std::vector<CubicCurveFitter::Vec2> uv_vals;
        double total_length;
        curve.reparameterizeArcLength(s_vals, uv_vals, total_length);

        // 提取 U 和 V 分量并拟合
        auto u_vals = CubicCurveFitter::extractU(uv_vals);
        auto v_vals = CubicCurveFitter::extractV(uv_vals);
        auto coef_U = CubicCurveFitter::fitCubic(s_vals, u_vals);
        auto coef_V = CubicCurveFitter::fitCubic(s_vals, v_vals);

        // 设置系数
        OpenDriveParamPoly3 param_poly;
        param_poly.aU = coef_U[3]; param_poly.bU = coef_U[2];
        param_poly.cU = coef_U[1]; param_poly.dU = coef_U[0];
        param_poly.aV = coef_V[3]; param_poly.bV = coef_V[2];
        param_poly.cV = coef_V[1]; param_poly.dV = coef_V[0];

        result.push_back(param_poly);
    }

    return result;
}
```

#### 6.1.2 FormEditRoad 修改

```cpp
class FormEditRoad : public QWidget {
    Q_OBJECT

public:
    // ... 现有接口 ...

    // 设置道路的 ParamPoly3 系数
    void setParamPoly3Segments(const std::vector<common::OpenDriveParamPoly3>& segments);

    // 设置姿态点
    void setControlPoints(const std::vector<common::Pose2d>& poses);

    // 获取当前姿态点
    std::vector<common::Pose2d> getControlPoints() const;

    // 获取拟合后的分段
    std::vector<common::OpenDriveParamPoly3> getFittedSegments() const;

signals:
    void fitRequested(const std::vector<common::Pose2d>& poses);
    void coeffChanged(const std::vector<common::OpenDriveParamPoly3>& segments);

private slots:
    void onFitClicked();
    void onControlPointChanged();
    // ...

private:
    void updateCoeffDisplay();
    void updateControlPointTable();

    std::vector<common::Pose2d> poses_;  // 改为 Pose2d
    std::vector<common::OpenDriveParamPoly3> original_segments_;
    std::vector<common::OpenDriveParamPoly3> fitted_segments_;
};
```

### 6.2 数据流程

```
┌─────────────────────────────────────────────────────────────────┐
│                        数据流程图                                │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  xodr 文件加载                                                  │
│       │                                                         │
│       ▼                                                         │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │ RoadManager::Road                                       │   │
│  │  - GetPlanView()->GetGeometry(0)                      │   │
│  │  - 转换为 ParamPoly3*                                   │   │
│  │  - 获取 poly3U_, poly3V_ 系数                          │   │
│  └─────────────────────────────────────────────────────────┘   │
│       │                                                         │
│       ▼                                                         │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │ OdrGeometry::GenerateFromPoses() (反向)                  │   │
│  │  - 从系数生成采样点                                     │   │
│  │  - 转换为世界坐标 (x, y, yaw)                           │   │
│  │  - 生成姿态点序列                                       │   │
│  └─────────────────────────────────────────────────────────┘   │
│       │                                                         │
│       ▼                                                         │
│  FormEditRoad::setControlPoints()                              │
│       │                                                         │
│       ▼                                                         │
│  UI 显示姿态点表格                                             │
│                                                                  │
│  ───────────────────────────────────────────────────────────   │
│                                                                  │
│  用户编辑姿态点                                                 │
│       │                                                         │
│       ▼                                                         │
│  用户点击 "拟合曲线"                                           │
│       │                                                         │
│       ▼                                                         │
│  OdrGeometry::GenerateFromPoses()                              │
│       │                                                         │
│       ▼                                                         │
│  生成新的 ParamPoly3 系数                                       │
│       │                                                         │
│       ▼                                                         │
│  Display::onCoeffChanged()                                    │
│       │                                                         │
│       ▼                                                         │
│  地图上预览新曲线                                              │
│                                                                  │
│  ───────────────────────────────────────────────────────────   │
│                                                                  │
│  用户点击 "确定"                                               │
│       │                                                         │
│       ▼                                                         │
│  RoadManager 修改 ParamPoly3 系数                             │
│       │                                                         │
│       ▼                                                         │
│  OpenDrive::Save() 写入 xodr 文件                             │
│       │                                                         │
│       ▼                                                         │
│  Display::openXodr() 重新加载                                  │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

## 7. 实现步骤

### 阶段1: 几何线形生成器
1. 创建 `common/odr_geometry.h` 和 `.cpp`
2. 实现 `OdrGeometry::GenerateFromPoses()` 
3. 创建 `common/cubic_curve_fitter.h` 和 `.cpp`
4. 实现三次 Hermite 插值和弧长重参数化
5. 实现三次多项式最小二乘拟合

### 阶段2: UI 控件
1. 修改 `form_edit_road.ui` 
   - 移除原来的几何类型切换
   - 添加 ParamPoly3 系数显示区域
   - 添加姿态点表格（X, Y, yaw 列）
   - 添加拟合选项
2. 实现姿态点表格的增删改查
3. 实现系数显示的更新

### 阶段3: 预览功能
1. 在 Display 中实现预览模式
2. 实现拟合曲线的地图绘制
3. 实现姿态点的地图绘制
4. 实现原始曲线的虚线显示

### 阶段4: 保存功能
1. 实现 RoadManager 系数修改
2. 实现 xodr 文件写入
3. 实现重新加载

### 阶段5: 测试优化
1. 功能测试
2. 边界条件处理
3. UI 交互优化

## 8. 文件清单

| 文件 | 操作 | 说明 |
|------|------|------|
| `common/odr_geometry.h` | 新增 | 几何线形生成器头文件 |
| `common/odr_geometry.cc` | 新增 | 几何线形生成器实现 |
| `common/cubic_curve_fitter.h` | 新增 | 三次曲线拟合器头文件 |
| `common/cubic_curve_fitter.cc` | 新增 | 三次曲线拟合器实现 |
| `common/point.h` | 修改 | 添加 Pose2d 构造函数 |
| `common/CMakeLists.txt` | 修改 | 添加新文件 |
| `map_sdk/map_base/road.h` | 修改 | 添加参考线修改方法 |
| `display/form_edit/edit_road/form_edit_road.ui` | 修改 | 修改 UI |
| `display/form_edit/edit_road/form_edit_road.h` | 修改 | 添加信号槽，使用 Pose2d |
| `display/form_edit/edit_road/form_edit_road.cpp` | 修改 | 实现编辑逻辑 |
| `display/display.h` | 修改 | 添加编辑相关成员 |
| `display/display.cpp` | 修改 | 添加编辑相关实现 |
| `display/draw_map.cc` | 修改 | 添加曲线绘制 |
| `display/CMakeLists.txt` | 修改 | 添加新文件 |

## 9. 待确认问题

1. **RoadManager 写入**: xodr 文件写入时如何修改 ParamPoly3 的系数？
   - 需要了解 `roadmanager::ParamPoly3` 是否有 Set 方法
   - `OpenDrive::Save()` 是否可以直接保存

2. **姿态点数量**: 用户可以添加的姿态点数量有限制吗？
   - 2个姿态点：直线
   - 3个以上：曲线

3. **精度问题**: 拟合后的曲线与原始 xodr 加载的曲线有误差，是否需要优化？

4. **样条类型**: 是否只需要分段 Hermite 插值，还是需要支持多种拟合方法？

这些问题确认后，可以进一步细化实现方案。
