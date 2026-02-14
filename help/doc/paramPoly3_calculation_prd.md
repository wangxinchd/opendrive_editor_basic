# OpenDRIVE ParamPoly3 参数计算方案 PRD

## 1. 需求概述

在 OpenDRIVE 地图编辑器中，需要实现根据已知条件计算 `paramPoly3` 曲线参数的功能。具体输入条件为：

- **两个姿态点**：起点 Pose0(x0, y0, yaw0) 和终点 Pose1(x1, y1, yaw1)
- 包含位置和航向角信息

输出为 OpenDRIVE 标准格式的 `paramPoly3` 曲线系数。

## 2. 数学背景

### 2.1 OpenDRIVE ParamPoly3 格式

OpenDRIVE 中的 `paramPoly3` 使用参数多项式表示曲线：

```
u(p) = aU + bU * p + cU * p^2 + dU * p^3
v(p) = aV + bV * p + cV * p^2 + dV * p^3
```

其中：
- p ∈ [0, 1] 为归一化参数
- u 为沿道路主方向的横向坐标
- v 为垂直于道路主方向的纵向坐标

### 2.2 坐标系说明

**局部坐标系**的定义：
- 原点：起点 P0
- U 轴：沿起点航向角 psi0 的方向
- V 轴：垂直于 U 轴（逆时针 90°）

```
世界坐标系 (X, Y)                    局部坐标系 (U, V)
                                        ↑
       Y                               |
       |                                | V 轴
       |                                |
       |    P0 ●─────────→ P1           |--------→ U 轴
       |                              (psi0 方向)
       └────────────→ X
```

## 3. 计算方案（分段三次 Hermite 插值）

### 3.1 核心数据结构

**文件**: `common/odr_geometry.h`

```cpp
#include "point.h"

namespace common {

// OpenDRIVE ParamPoly3 表示
struct OpenDriveParamPoly3 {
  double aU = 0.0;  // constant term
  double bU = 0.0;  // linear term
  double cU = 0.0;  // quadratic term
  double dU = 0.0;  // cubic term

  double aV = 0.0;  // constant term
  double bV = 0.0;  // linear term
  double cV = 0.0;  // quadratic term
  double dV = 0.0;  // cubic term
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

### 3.2 三次曲线拟合器

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

  // 三次多项式拟合（最小二乘）
  static std::array<double,4> fitCubic(const std::vector<double>& s_norm,
                                       const std::vector<double>& vals);
};
```

### 3.3 计算流程

```
输入: std::vector<Pose2d> poses = [Pose0, Pose1, Pose2, ...]
                           ↓
遍历相邻姿态点对: (Pose0, Pose1), (Pose1, Pose2), ...
                           ↓
对每对姿态点:
  1. 创建 CubicCurveFitter(pose_i, pose_{i+1})
  2. 弧长重参数化: s_vals[], uv_vals[]
  3. 提取 U, V 分量
  4. 拟合三次多项式: fitCubic(s_vals, u_vals), fitCubic(s_vals, v_vals)
  5. 转换为 OpenDriveParamPoly3
                           ↓
输出: std::vector<OpenDriveParamPoly3> segments
```

### 3.4 弧长重参数化

```cpp
void CubicCurveFitter::reparameterizeArcLength(...) {
    // 1. 采样 t ∈ [0, 1]
    for(int i = 0; i < n; i++) {
        double t = double(i) / (n - 1);
        auto p = evaluate(t);
        uv_vals[i] = p;

        // 速度 = |dp/dt|
        double dx = 3*a_[0]*t*t + 2*b_[0]*t + c_[0];
        double dy = 3*a_[1]*t*t + 2*b_[1]*t + c_[1];
        speed[i] = std::sqrt(dx*dx + dy*dy);
    }

    // 2. 累加弧长
    s_vals[0] = 0.0;
    for(int i = 1; i < n; i++) {
        double ds = 0.5 * (speed[i-1] + speed[i]) * (1.0 / (n - 1));
        s_vals[i] = s_vals[i-1] + ds;
    }

    // 3. 转换为局部坐标 (U, V)
    for(int i = 0; i < n; i++) {
        uv_vals[i] = worldToLocal(uv_vals[i][0], uv_vals[i][1], x0_, y0_, yaw0_);
    }
}
```

### 3.5 三次多项式拟合

使用最小二乘法拟合：

```cpp
// 拟合: vals = a*t^3 + b*t^2 + c*t + d
// 返回系数 [a, b, c, d]
std::array<double,4> CubicCurveFitter::fitCubic(
    const std::vector<double>& s_norm,
    const std::vector<double>& vals
) {
    // 构建法方程: (X^T * X) * coeffs = X^T * y
    // 高斯消元求解 4x4 线性方程组
    // ...
    return {a, b, c, d};
}
```

### 3.6 输出 OpenDRIVE 格式

```cpp
void CubicCurveFitter::printOpenDRIVE(
    std::ostream& os,
    const std::array<double,4>& coef_U,
    const std::array<double,4>& coef_V,
    double total_length
) {
    // coef_U = {a, b, c, d} 对应 a*t^3 + b*t^2 + c*t + d
    // OpenDRIVE: aU=常数项, bU=一次项, cU=二次项, dU=三次项
    os << "<paramPoly3 "
       << "aU=\"" << coef_U[3] << "\" "
       << "bU=\"" << coef_U[2] << "\" "
       << "cU=\"" << coef_U[1] << "\" "
       << "dU=\"" << coef_U[0] << "\" "
       << "aV=\"" << coef_V[3] << "\" "
       << "bV=\"" << coef_V[2] << "\" "
       << "cV=\"" << coef_V[1] << "\" "
       << "dV=\"" << coef_V[0] << "\" "
       << "pRange=\"arcLength\"/>\n";
}
```

## 4. 使用示例

```cpp
#include "odr_geometry.h"
#include "cubic_curve_fitter.h"

int main() {
    // 定义姿态序列
    std::vector<common::Pose2d> poses = {
        common::Pose2d(100.0, 0.0, 0.0),
        common::Pose2d(320.0, 3.7, 0.0)
    };

    // 生成几何线形
    auto segments = common::OdrGeometry::GenerateFromPoses(poses);

    // 输出 OpenDRIVE 格式
    for (size_t i = 0; i < segments.size(); i++) {
        const auto& seg = segments[i];
        std::cout << "段 " << i << ": "
                  << "aU=" << seg.aU << " bU=" << seg.bU
                  << " cU=" << seg.cU << " dU=" << seg.dU
                  << " aV=" << seg.aV << " bV=" << seg.bV
                  << " cV=" << seg.cV << " dV=" << seg.dV << "\n";
    }

    return 0;
}
```

## 5. 文件清单

| 文件 | 说明 |
|------|------|
| `common/odr_geometry.h` | OpenDRIVE 几何线形生成器头文件 |
| `common/odr_geometry.cc` | OpenDRIVE 几何线形生成器实现 |
| `common/cubic_curve_fitter.h` | 三次曲线拟合器头文件 |
| `common/cubic_curve_fitter.cc` | 三次曲线拟合器实现 |
| `common/point.h` | 基础类型定义 (Point2d, Pose2d, ...) |
| `common/geometry.h/cc` | 几何辅助函数 |

## 6. 验证方法

### 6.1 边界条件验证

1. **直线情况**：当所有 yaw 相同时，应退化为直线
2. **180度转弯**：当相邻点 yaw 差为 π 时，应能正确处理
3. **零偏移**：当所有 y 相同时，V(p) 应接近 0

### 6.2 数值精度验证

- 验证曲线起点 U(0) ≈ 0, V(0) ≈ 0
- 验证曲线终点 U(1) ≈ U1, V(1) ≈ V1
- 验证曲线在起点的导数符合输入 yaw0
- 验证曲线在终点的导数符合输入 yaw1

## 7. 应用场景

1. **道路中心线生成**：根据道路的起点、终点和方向生成道路中心线
2. **车道边界生成**：根据车道边界上的控制点生成平滑的车道边界
3. **匝道连接**：在匝道与主路连接处生成平滑过渡曲线

## 8. 总结

本方案的核心思路是：

- **分段三次 Hermite 插值**：每对相邻姿态点之间用三次曲线连接
- **弧长重参数化**：将参数 t 映射到实际弧长，提高数值稳定性
- **最小二乘法拟合**：对采样点进行三次多项式拟合，生成 OpenDRIVE ParamPoly3 系数
- **统一使用 Pose2d**：控制点统一使用 `common::Pose2d` 类型
