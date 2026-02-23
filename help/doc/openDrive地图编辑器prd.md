# OpenDRIVE 地图编辑器完整方案

## 1. 项目现状分析

### 1.1 现有功能

| 功能 | 状态 | 说明 |
|------|------|------|
| 可视化地图 | ✅ | 使用Qt显示OpenDRIVE地图，支持缩放、平移 |
| ParamPoly3计算 | ✅ | 根据控制点（Pose2d）拟合三次曲线 |
| 手动修改XODR | ✅ | 手动编辑XML后重新加载查看效果 |
| 道路参考线编辑 | ✅ | 初步的道路编辑面板 |
| 显示配置 | ✅ | 基础的颜色、线型配置 |

### 1.2 当前架构问题

```
┌─────────────────────────────────────────────────────────────────────┐
│                         当前架构                                     │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  ┌──────────────┐    ┌──────────────┐    ┌──────────────┐        │
│  │   XODR文件    │───▶│  OpenDrive   │───▶│   MapBase    │        │
│  │   (XML格式)   │    │   Parser     │    │    Map       │        │
│  └──────────────┘    └──────────────┘    └──────────────┘        │
│                                                    │               │
│                                                    ▼               │
│  ┌──────────────┐    ┌──────────────┐    ┌──────────────┐        │
│  │   Qt 显示     │◀───│   Display    │◀───│    Lane      │        │
│  │   (渲染层)    │    │   (主窗口)   │    │  (离散点)    │        │
│  └──────────────┘    └──────────────┘    └──────────────┘        │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

**主要问题**：

1. **数据模型层缺陷**
   - 只存储解析后的离散采样点，丢失了原始几何参数
   - 缺少对OpenDRIVE原始几何元素（line, arc, spiral, poly3, paramPoly3）的存储
   - 道路的planView、lanes、objects等核心元素没有被完整建模

2. **编辑能力不足**
   - 只能通过外部编辑器修改XODR后重新加载
   - 没有GUI编辑功能（车道、路口、连接关系）
   - 缺少撤销/重做机制

3. **地图保存缺失**
   - 只能读取XODR，不能将编辑后的地图写回XODR
   - 缺少序列化/反序列化能力

4. **RoadManager强耦合**
   - 过度依赖roadmanager::Position::LoadOpenDrive
   - 没有构建独立的数据模型

---

## 2. 目标架构设计

### 2.1 整体架构

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                              目标架构                                            │
├─────────────────────────────────────────────────────────────────────────────────┤
│                                                                                 │
│  ┌──────────────────────────────────────────────────────────────────────────┐   │
│  │                              UI 层 (Qt)                                   │   │
│  │  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌─────────────────────┐ │   │
│  │  │ 主显示窗口   │ │ 道路编辑器  │ │ 车道编辑器  │ │ 路口/连接编辑器     │ │   │
│  │  │ MainWindow  │ │ RoadEditor  │ │ LaneEditor │ │ JunctionEditor      │ │   │
│  │  └─────────────┘ └─────────────┘ └─────────────┘ └─────────────────────┘ │   │
│  └──────────────────────────────────────────────────────────────────────────┘   │
│                                        │                                          │
│                                        ▼                                          │
│  ┌──────────────────────────────────────────────────────────────────────────┐   │
│  │                           核心业务层                                        │   │
│  │  ┌────────────────┐ ┌────────────────┐ ┌────────────────────────────────┐│   │
│  │  │ MapEditor      │ │ GeometryEngine │ │ ValidationEngine              ││   │
│  │  │ (地图编辑核心)  │ │ (几何计算)      │ │ (拓扑验证)                    ││   │
│  │  └────────────────┘ └────────────────┘ └────────────────────────────────┘│   │
│  │  ┌────────────────┐ ┌────────────────┐ ┌────────────────────────────────┐│   │
│  │  │ UndoRedoMgr    │ │ SelectionMgr   │ │ ExportManager                 ││   │
│  │  │ (撤销/重做)     │ │ (选择管理)      │ │ (导出管理)                    ││   │
│  │  └────────────────┘ └────────────────┘ └────────────────────────────────┘│   │
│  └──────────────────────────────────────────────────────────────────────────┘   │
│                                        │                                          │
│                                        ▼                                          │
│  ┌──────────────────────────────────────────────────────────────────────────┐   │
│  │                           数据模型层                                        │   │
│  │  ┌─────────────────────────────────────────────────────────────────────┐ │   │
│  │  │                        OpenDriveMap (地图根)                        │ │   │
│  │  │  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌───────────────┐ │ │   │
│  │  │  │  Header    │ │   Roads     │ │ Junctions  │ │ Controllers   │ │ │   │
│  │  │  └─────────────┘ └─────────────┘ └─────────────┘ └───────────────┘ │ │   │
│  │  └─────────────────────────────────────────────────────────────────────┘ │   │
│  │                                                                         │   │
│  │  ┌─────────────────────────────────────────────────────────────────────┐ │   │
│  │  │                            Road                                     │ │   │
│  │  │  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌───────────────┐ │ │   │
│  │  │  │  PlanView   │ │   Lanes     │ │  Objects    │ │  Signals      │ │ │   │
│  │  │  └─────────────┘ └─────────────┘ └─────────────┘ └───────────────┘ │ │   │
│  │  └─────────────────────────────────────────────────────────────────────┘ │   │
│  │                                                                         │   │
│  │  ┌─────────────────────────────────────────────────────────────────────┐ │   │
│  │  │                      Geometry (几何元素基类)                         │ │   │
│  │  │  ┌────────┐ ┌────────┐ ┌────────┐ ┌─────────┐ ┌───────────────┐    │ │   │
│  │  │  │  Line  │ │  Arc  │ │Spiral │ │ Poly3  │ │ ParamPoly3    │    │ │   │
│  │  │  └────────┘ └────────┘ └────────┘ └─────────┘ └───────────────┘    │ │   │
│  │  └─────────────────────────────────────────────────────────────────────┘ │   │
│  └──────────────────────────────────────────────────────────────────────────┘   │
│                                        │                                          │
│                                        ▼                                          │
│  ┌──────────────────────────────────────────────────────────────────────────┐   │
│  │                           I/O 层                                           │   │
│  │  ┌────────────────┐ ┌────────────────┐ ┌────────────────────────────────┐│   │
│  │  │ OpenDriveParser│ │ XodrWriter     │ │ MapSerializer (JSON/Binary) ││   │
│  │  └────────────────┘ └────────────────┘ └────────────────────────────────┘│   │
│  └──────────────────────────────────────────────────────────────────────────┘   │
│                                                                                 │
└─────────────────────────────────────────────────────────────────────────────────┘
```

### 2.2 核心类设计

#### 2.2.1 几何元素类层次

```cpp
// common/geometry/geometry_base.h

namespace common {

// 几何元素类型枚举
enum class GeometryType {
  LINE,       // 直线
  ARC,        // 圆弧
  SPIRAL,     // 回旋线（螺旋）
  POLY3,      // 三次多项式 (x(s), y(s))
  PARAMPOLY3  // 参数三次多项式 (u(p), v(p))
};

// 几何元素基类
class Geometry {
public:
  virtual ~Geometry() = default;
  
  virtual GeometryType type() const = 0;
  
  // 在指定s位置计算坐标
  virtual double GetX(double s) const = 0;
  virtual double GetY(double s) const = 0;
  
  // 在指定s位置计算航向角
  virtual double GetHeading(double s) const = 0;
  
  // 获取几何元素长度
  virtual double GetLength() const = 0;
  
  // 克隆
  virtual std::unique_ptr<Geometry> Clone() const = 0;
  
  // 序列化为XML属性
  virtual std::string ToXmlAttributes() const = 0;
};

// 直线
class LineGeometry : public Geometry {
public:
  LineGeometry(double x, double y, double heading, double length);
  
  GeometryType type() const override { return GeometryType::LINE; }
  double GetX(double s) const override;
  double GetY(double s) const override;
  double GetHeading(double s) const override;
  double GetLength() const override { return length_; }
  std::unique_ptr<Geometry> Clone() const override;
  std::string ToXmlAttributes() const override;
  
private:
  double x_, y_;        // 起点坐标
  double heading_;      // 航向角 (rad)
  double length_;      // 长度
};

// 圆弧
class ArcGeometry : public Geometry {
public:
  ArcGeometry(double x, double y, double heading, double length, double curvature);
  
  GeometryType type() const override { return GeometryType::ARC; }
  double GetX(double s) const override;
  double GetY(double s) const override;
  double GetHeading(double s) const override;
  double GetLength() const override { return length_; }
  std::unique_ptr<Geometry> Clone() const override;
  std::string ToXmlAttributes() const override;
  
private:
  double x_, y_;        // 起点坐标
  double heading_;      // 起始航向角
  double length_;      // 弧长
  double curvature_;   // 曲率 (1/radius)
};

// ParamPoly3 (最常用)
class ParamPoly3Geometry : public Geometry {
public:
  ParamPoly3Geometry(double x, double y, double heading, double length,
                     double aU, double bU, double cU, double dU,
                     double aV, double bV, double cV, double dV,
                     bool pRangeIsArcLength = true);
  
  GeometryType type() const override { return GeometryType::PARAMPOLY3; }
  double GetX(double s) const override;
  double GetY(double s) const override;
  double GetHeading(double s) const override;
  double GetLength() const override { return length_; }
  std::unique_ptr<Geometry> Clone() const override;
  std::string ToXmlAttributes() const override;
  
  // 参数访问
  void SetCoefficients(double aU, double bU, double cU, double dU,
                       double aV, double bV, double cV, double dV);
  std::array<double, 8> GetCoefficients() const;
  
private:
  double x_, y_;        // 起点坐标
  double heading_;      // 起始航向角
  double length_;      // 长度
  
  // ParamPoly3系数: u(p) = aU + bU*p + cU*p^2 + dU*p^3
  double aU_, bU_, cU_, dU_;
  double aV_, bV_, cV_, dV_;
  
  bool pRangeIsArcLength_;  // p范围是弧长还是归一化
};

} // namespace common
```

#### 2.2.2 PlanView - 道路几何中心线

```cpp
// map_base/plan_view.h

namespace map_base {

class PlanView {
public:
  PlanView() = default;
  
  // 添加几何元素（自动计算累计长度）
  void AddGeometry(std::unique_ptr<common::Geometry> geometry);
  
  // 获取几何元素数量
  size_t GetNumGeometries() const { return geometries_.size(); }
  
  // 获取第i个几何元素
  const common::Geometry* GetGeometry(size_t i) const;
  
  // 获取总长度
  double GetLength() const;
  
  // 在全局s位置计算坐标
  common::Point2d GetPoint(double globalS) const;
  
  // 在全局s位置计算姿态
  common::Pose2d GetPose(double globalS) const;
  
  // 在全局s位置计算航向角
  double GetHeading(double globalS) const;
  
  // 在全局s位置计算曲率
  double GetCurvature(double globalS) const;
  
  // 射线查询：找到指定点最近的参数s和局部偏移
  std::pair<double, double> FindNearestS(const common::Point2d& point) const;
  
  // 序列化
  std::string ToXml() const;
  
  // 反序列化（解析XML）
  bool FromXml(const pugi::xml_node& node);
  
private:
  std::vector<std::unique_ptr<common::Geometry>> geometries_;
  std::vector<double> accumulated_lengths_;  // 累计长度
};

} // namespace map_base
```

#### 2.2.3 LaneSection & Lane - 车道

```cpp
// map_base/lane_section.h

namespace map_base {

enum class LaneType {
  NONE,       // 中心车道（不参与交通）
  DRIVING,    // 行驶车道
  STOPPING,   // 停车车道
  SHOULDER,   // 路肩
  BIKING,     // 自行车道
  SIDEWALK,   // 人行道
  BORDER,     // 边界
  RESTRICTED, // 限制使用
  PARKING     // 停车
};

enum class LaneRoadMarkType {
  NONE,
  SOLID,
  BROKEN,
  SOLID_SOLID,
  SOLID_BROKEN,
  BROKEN_SOLID,
  BROKEN_BROKEN
};

// 车道边界（可包含多个record）
class LaneBoundary {
public:
  LaneBoundary() = default;
  
  void SetType(LaneRoadMarkType type) { type_ = type; }
  void SetWidth(double width) { width_ = width; }
  void SetOffset(double offset) { offset_ = offset; }
  
  LaneRoadMarkType GetType() const { return type_; }
  double GetWidth() const { return width_; }
  double GetOffset() const { return offset_; }
  
  std::string ToXml() const;
  
private:
  LaneRoadMarkType type_ = LaneRoadMarkType::BROKEN;
  double width_ = 0.0;     // 边界宽度
  double offset_ = 0.0;    // 相对于参考线的偏移
};

// 车道
class Lane {
public:
  using LaneId = std::tuple<int64_t, int64_t, int64_t>;  // (road_id, section_id, lane_id)
  
  Lane() = default;
  Lane(int64_t road_id, int64_t section_id, int64_t lane_id);
  
  // ID
  LaneId GetId() const { return id_; }
  int64_t GetLaneId() const { return std::get<2>(id_); }
  
  // 车道类型
  void SetLaneType(LaneType type) { lane_type_ = type; }
  LaneType GetLaneType() const { return lane_type_; }
  
  // 车道宽度（可分段）
  void AddWidthRecord(double sOffset, double width, double innerOffset = 0, double outerOffset = 0);
  double GetWidth(double s) const;
  
  // 边界
  void SetLeftBoundary(std::unique_ptr<LaneBoundary> boundary);
  void SetRightBoundary(std::unique_ptr<LaneBoundary> boundary);
  const LaneBoundary* GetLeftBoundary() const { return left_boundary_.get(); }
  const LaneBoundary* GetRightBoundary() const { return right_boundary_.get(); }
  
  // 前驱/后继
  void AddPredecessor(int64_t roadId, int64_t laneId);
  void AddSuccessor(int64_t roadId, int64_t laneId);
  
  // 计算车道中心线
  std::vector<common::Pose2d> SampleCenterline(double step = 1.0) const;
  
  // 计算车道边界
  std::vector<common::Pose2d> SampleLeftBoundary(double step = 1.0) const;
  std::vector<common::Pose2d> SampleRightBoundary(double step = 1.0) const;
  
  std::string ToXml() const;
  
private:
  LaneId id_;
  LaneType lane_type_ = LaneType::DRIVING;
  
  // 宽度记录: sOffset -> WidthInfo
  struct WidthRecord {
    double sOffset;
    double width;
    double innerOffset;
    double outerOffset;
  };
  std::vector<WidthRecord> width_records_;
  
  std::unique_ptr<LaneBoundary> left_boundary_;
  std::unique_ptr<LaneBoundary> right_boundary_;
  
  // 连接关系
  std::vector<std::pair<int64_t, int64_t>> predecessors_;  // (roadId, laneId)
  std::vector<std::pair<int64_t, int64_t>> successors_;
};

// 车道段
class LaneSection {
public:
  LaneSection() = default;
  explicit LaneSection(double s);
  
  double GetS() const { return s_; }
  double GetLength() const { return length_; }
  void SetLength(double length) { length_ = length; }
  
  // 车道操作
  void AddLane(std::unique_ptr<Lane> lane);
  Lane* GetLane(int64_t laneId);
  const Lane* GetLane(int64_t laneId) const;
  void RemoveLane(int64_t laneId);
  
  // 遍历所有车道
  const std::vector<std::unique_ptr<Lane>>& GetLanes() const { return lanes_; }
  
  std::string ToXml() const;
  
private:
  double s_ = 0.0;           // 起始s
  double length_ = 0.0;     // 长度
  
  // 车道: lane_id -> Lane (lane_id可正可负，0为中心车道)
  std::vector<std::unique_ptr<Lane>> lanes_;
};

} // namespace map_base
```

#### 2.2.4 Road - 道路

```cpp
// map_base/road.h (扩展版本)

namespace map_base {

class Road {
public:
  Road() = default;
  Road(int64_t id, std::string name = "");
  
  // 基本信息
  int64_t GetId() const { return id_; }
  void SetId(int64_t id) { id_ = id; }
  const std::string& GetName() const { return name_; }
  void SetName(const std::string& name) { name_ = name; }
  
  // PlanView
  PlanView& GetPlanView() { return plan_view_; }
  const PlanView& GetPlanView() const { return plan_view_; }
  
  // 车道
  void AddLaneSection(std::unique_ptr<LaneSection> section);
  LaneSection* GetLaneSection(double s);
  const std::vector<std::unique_ptr<LaneSection>>& GetLaneSections() const;
  
  // 链接
  void SetPredecessor(int64_t roadId, std::string elementType = "road");
  void SetSuccessor(int64_t roadId, std::string elementType = "road");
  const std::pair<int64_t, std::string>& GetPredecessor() const;
  const std::pair<int64_t, std::string>& GetSuccessor() const;
  
  // 道路类型（速度限制等）
  void AddRoadType(double s, std::string type, double speed);
  
  // 对象
  void AddObject(...);  // 路边物体
  void AddSignal(...);  // 信号灯
  
  // 采样
  std::vector<common::Pose2d> SampleReferenceLine(double step = 1.0) const;
  
  // 序列化
  std::string ToXml() const;
  
private:
  int64_t id_ = 0;
  std::string name_;
  
  PlanView plan_view_;
  std::vector<std::unique_ptr<LaneSection>> lane_sections_;
  
  // 链接
  std::pair<int64_t, std::string> predecessor_;   // (roadId, elementType)
  std::pair<int64_t, std::string> successor_;
  
  // 道路类型记录
  struct RoadType {
    double s;
    std::string type;
    double speed;  // m/s
  };
  std::vector<RoadType> road_types_;
};

} // namespace map_base
```

#### 2.2.5 OpenDriveMap - 地图根

```cpp
// map_base/open_drive_map.h

namespace map_base {

class Junction {
public:
  Junction() = default;
  Junction(int64_t id, std::string name);
  
  int64_t GetId() const { return id_; }
  const std::string& GetName() const { return name_; }
  
  void AddConnection(...);  // 添加连接关系
  const std::vector<Connection>& GetConnections() const;
  
  std::string ToXml() const;
  
private:
  int64_t id_;
  std::string name_;
  std::vector<Connection> connections_;
};

class OpenDriveMap {
public:
  OpenDriveMap() = default;
  explicit OpenDriveMap(std::string name);
  
  // Header
  struct Header {
    std::string revMajor = "1";
    std::string revMinor = "4";
    std::string name;
    std::string version;
    std::string date;
    double north = 0, south = 0, east = 0, west = 0;
    std::string vendor;
    double xOffset = 0, yOffset = 0, zOffset = 0;
    double headingOffset = 0;
  };
  
  Header& GetHeader() { return header_; }
  const Header& GetHeader() const { return header_; }
  
  // 道路
  void AddRoad(std::unique_ptr<Road> road);
  Road* GetRoad(int64_t id);
  const Road* GetRoad(int64_t id) const;
  std::vector<Road*> GetAllRoads();
  const std::vector<std::unique_ptr<Road>>& GetRoads() const { return roads_; }
  void RemoveRoad(int64_t id);
  
  // 交叉口
  void AddJunction(std::unique_ptr<Junction> junction);
  Junction* GetJunction(int64_t id);
  const std::vector<std::unique_ptr<Junction>>& GetJunctions() const { return junctions_; }
  
  // 控制器
  void AddController(...);
  
  // 地理参考
  void SetGeoReference(const std::string& geoRef);
  const std::string& GetGeoReference() const { return geo_reference_; }
  
  // 文件操作
  bool LoadFromFile(const std::string& filePath);
  bool SaveToFile(const std::string& filePath) const;
  
  // 验证
  ValidationResult Validate() const;
  
private:
  Header header_;
  std::vector<std::unique_ptr<Road>> roads_;
  std::vector<std::unique_ptr<Junction>> junctions_;
  std::vector<Controller> controllers_;
  std::string geo_reference_;
  
  // ID索引
  std::unordered_map<int64_t, Road*> road_index_;
  std::unordered_map<int64_t, Junction*> junction_index_;
};

} // namespace map_base
```

### 2.3 编辑器核心类

```cpp
// editor/map_editor.h

namespace editor {

// 编辑操作基类
class EditOperation {
public:
  virtual ~EditOperation() = default;
  virtual void Execute(OpenDriveMap* map) = 0;
  virtual void Undo(OpenDriveMap* map) = 0;
  virtual std::string GetDescription() const = 0;
};

// 添加道路
class AddRoadOperation : public EditOperation {
public:
  AddRoadOperation(std::unique_ptr<Road> road);
  
  void Execute(OpenDriveMap* map) override;
  void Undo(OpenDriveMap* map) override;
  std::string GetDescription() const override { return "Add Road"; }
  
private:
  std::unique_ptr<Road> road_;
  int64_t road_id_;
};

// 修改道路几何
class ModifyRoadGeometryOperation : public EditOperation {
public:
  ModifyRoadGeometryOperation(int64_t roadId, 
                               std::unique_ptr<PlanView> newPlanView);
  
  void Execute(OpenDriveMap* map) override;
  void Undo(OpenDriveMap* map) override;
  std::string GetDescription() const override { return "Modify Road Geometry"; }
  
private:
  int64_t road_id_;
  std::unique_ptr<PlanView> new_plan_view_;
  std::unique_ptr<PlanView> old_plan_view_;
};

// 地图编辑器
class MapEditor {
public:
  MapEditor();
  
  // 地图操作
  void SetMap(std::shared_ptr<map_base::OpenDriveMap> map);
  std::shared_ptr<map_base::OpenDriveMap> GetMap() const { return map_; }
  
  // 编辑操作
  void ExecuteOperation(std::unique_ptr<EditOperation> operation);
  void Undo();
  void Redo();
  bool CanUndo() const { return undo_stack_.size() > 0; }
  bool CanRedo() const { return redo_stack_.size() > 0; }
  
  // 选择管理
  enum class SelectableType {
    ROAD, LANE, JUNCTION, LANE_SECTION, OBJECT, SIGNAL
  };
  
  struct Selection {
    SelectableType type;
    int64_t id;
    // 对于Lane还有: section_index, lane_id
  };
  
  void Select(const Selection& sel);
  void ClearSelection();
  const std::vector<Selection>& GetSelection() const { return selection_; }
  
  // 验证
  map_base::ValidationResult Validate() const;
  
  // 导出
  bool ExportToXodr(const std::string& path) const;
  bool ExportToJson(const std::string& path) const;
  
signals:
  void MapChanged();
  void SelectionChanged();
  void OperationExecuted(const std::string& description);
  
private:
  std::shared_ptr<map_base::OpenDriveMap> map_;
  
  // 撤销/重做栈
  std::vector<std::unique_ptr<EditOperation>> undo_stack_;
  std::vector<std::unique_ptr<EditOperation>> redo_stack_;
  
  // 选择
  std::vector<Selection> selection_;
};

} // namespace editor
```

---

## 3. 核心功能模块

### 3.1 几何计算引擎

```cpp
// common/geometry/geometry_engine.h

namespace common {

class GeometryEngine {
public:
  // 从控制点拟合ParamPoly3
  static std::unique_ptr<ParamPoly3Geometry> FitParamPoly3(
      const std::vector<Pose2d>& controlPoints,
      double length,
      FitMethod method = FitMethod::HERMITE);
  
  // 从离散点拟合ParamPoly3（最小二乘）
  static std::unique_ptr<ParamPoly3Geometry> FitParamPoly3FromPoints(
      const std::vector<Point2d>& points,
      FitMethod method = FitMethod::LEAST_SQUARES);
  
  // 计算两条曲线的偏差
  static double CalculateDeviation(
      const Geometry* geom1, 
      const Geometry* geom2,
      double step = 0.5);
  
  // 曲线平滑（Catmull-Rom样条）
  static std::vector<Pose2d> SmoothCurve(
      const std::vector<Pose2d>& points,
      double tension = 0.5);
  
  // 弧长参数化
  static std::vector<double> ComputeArcLengths(
      const std::vector<Point2d>& points);
  
  // 重新参数化为弧长
  static std::vector<Point2d> ReparameterizeByArcLength(
      const std::vector<Point2d>& points);
  
  // 曲线段连接（C1连续）
  static std::pair<Geometry*, Geometry*> ConnectCurvesC1(
      const Geometry* curve1,
      const Geometry* curve2,
      const Pose2d& connectionPose);
};

} // namespace common
```

### 3.2 拓扑验证引擎

```cpp
// editor/validation_engine.h

namespace editor {

struct ValidationError {
  enum class Severity { INFO, WARNING, ERROR };
  
  Severity severity;
  std::string message;
  std::string location;  // e.g., "Road:1/LaneSection:0/Lane:1"
  std::string code;      // e.g., "E001", "W002"
};

class ValidationEngine {
public:
  struct ValidationResult {
    std::vector<ValidationError> errors;
    std::vector<ValidationError> warnings;
    std::vector<ValidationError> infos;
    
    bool IsOk() const { return errors.empty(); }
    int GetErrorCount() const { return errors.size(); }
    int GetWarningCount() const { return warnings.size(); }
  };
  
  // 完整验证
  ValidationResult Validate(const map_base::OpenDriveMap& map);
  
  // 单项验证
  ValidationResult ValidateRoad(const map_base::Road& road);
  ValidationResult ValidateLaneConnectivity(const map_base::Road& road);
  ValidationResult ValidateJunction(const map_base::Junction& junction);
  ValidationResult ValidateGeometry(const map_base::PlanView& planView);
  
private:
  // 内部检查方法...
};

} // namespace editor
```

### 3.3 交互式编辑工具

| 工具 | 功能 | 快捷键 |
|------|------|--------|
| SelectTool | 选择道路/车道/路口 | V |
| PanTool | 平移视图 | H / 鼠标中键 |
| ZoomTool | 缩放视图 | Z / 滚轮 |
| MovePointTool | 拖拽控制点 | M |
| AddRoadTool | 绘制新道路 | R |
| AddLaneTool | 添加车道 | L |
| SplitTool | 分割道路/车道 | S |
| ConnectTool | 连接道路 | C |
| MeasureTool | 测量距离/角度 | Ctrl+M |

---

## 4. UI 设计

### 4.1 主窗口布局

```
┌─────────────────────────────────────────────────────────────────────────────┐
│  菜单栏: 文件 | 编辑 | 视图 | 工具 | 帮助                                      │
├─────────────────────────────────────────────────────────────────────────────┤
│  工具栏: [打开] [保存] [撤销] [重做] | [选择] [平移] [缩放] | [添加道路] [添加车道] │
├──────────┬───────────────────────────────────────────────────┬──────────────┤
│          │                                                   │              │
│  道路树   │                                                   │   属性面板   │
│          │                                                   │              │
│  ├ Road 1 │           主显示区域                              │  ID: 1      │
│  │  ├ Sec │           (地图视图)                              │  Name:      │
│  │  │  -1 │                                                   │  Length:    │
│  │  │   1 │                                                   │              │
│  │  └ Sec │                                                   │  ─────────  │
│  └ Road 2 │                                                   │  PlanView   │
│          │                                                   │  Lanes: 3   │
│          │                                                   │  Objects: 0 │
│          │                                                   │              │
├──────────┴───────────────────────────────────────────────────┴──────────────┤
│  状态栏: 当前工具: 选择 | 缩放: 100% | 坐标: (100, 200) | 已选择: Road:1     │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 4.2 编辑对话框

#### 道路编辑器
- 道路基本信息（ID、名称、级别）
- 道路类型与速度限制
- PlanView几何编辑（可视化拖拽控制点）
- 链接关系（前驱/后继）
- 交叉口分配

#### 车道编辑器
- 车道ID与类型
- 宽度编辑（表格/图形）
- 边界类型（线型、颜色）
- 车道链接
- 限高/限重

---

## 5. 文件结构

```
opendrive_editor/
├── CMakeLists.txt
├── README.md
│
├── common/                           # 公共基础库
│   ├── CMakeLists.txt
│   ├── point.h/cc                    # 基础类型
│   ├── geometry/                     # 几何计算
│   │   ├── geometry_base.h/cc        # 几何元素基类
│   │   ├── line_geometry.h/cc
│   │   ├── arc_geometry.h/cc
│   │   ├── spiral_geometry.h/cc
│   │   ├── poly3_geometry.h/cc
│   │   ├── parampoly3_geometry.h/cc
│   │   └── geometry_engine.h/cc     # 几何计算引擎
│   ├── cubic_curve_fitter.h/cc       # 曲线拟合
│   └── odr_geometry.h/cc             # OpenDRIVE几何助手
│
├── map_base/                         # 地图数据模型
│   ├── CMakeLists.txt
│   ├── open_drive_map.h/cc           # 地图根
│   ├── road.h/cc                     # 道路
│   ├── plan_view.h/cc                # 道路几何中心线
│   ├── lane_section.h/cc             # 车道段
│   ├── lane.h/cc                     # 车道
│   ├── lane_boundary.h/cc            # 车道边界
│   ├── junction.h/cc                 # 交叉口
│   └── validation_types.h            # 验证类型定义
│
├── map_io/                           # 地图I/O
│   ├── CMakeLists.txt
│   ├── xodr_reader.h/cc              # XODR读取
│   ├── xodr_writer.h/cc              # XODR写入
│   └── map_serializer.h/cc           # 通用序列化
│
├── editor/                           # 编辑器核心
│   ├── CMakeLists.txt
│   ├── map_editor.h/cc               # 地图编辑器
│   ├── edit_operation.h/cc           # 编辑操作基类
│   ├── undo_redo_manager.h/cc        # 撤销/重做
│   ├── selection_manager.h/cc        # 选择管理
│   ├── validation_engine.h/cc        # 验证引擎
│   └── tools/                        # 编辑工具
│       ├── edit_tool.h
│       ├── select_tool.h
│       ├── road_tool.h
│       └── lane_tool.h
│
├── display/                          # Qt显示/UI
│   ├── CMakeLists.txt
│   ├── main_window.h/cc              # 主窗口
│   ├── map_view.h/cc                 # 地图视图组件
│   ├── road_tree_widget.h/cc         # 道路树
│   ├── property_panel.h/cc           # 属性面板
│   ├── dialogs/                      # 对话框
│   │   ├── road_editor_dialog.h/cc
│   │   ├── lane_editor_dialog.h/cc
│   │   ├── junction_editor_dialog.h/cc
│   │   └── validation_result_dialog.h/cc
│   ├── config/                       # 显示配置
│   │   └── display_config.h/cc
│   └── resources/                    # 资源文件
│
└── tests/                            # 测试
    ├── test_geometry/
    ├── test_map_model/
    └── test_editor/
```

---

## 6. 实施路线

### Phase 1: 基础设施 (1-2周)

| 任务 | 描述 | 预计工时 |
|------|------|----------|
| 1.1 | 重构 `map_base` 数据模型，添加几何元素基类 | 3天 |
| 1.2 | 实现 `PlanView` + 各种 `Geometry` 子类 | 3天 |
| 1.3 | 完善 `LaneSection` / `Lane` / `Road` 模型 | 2天 |
| 1.4 | 实现 `OpenDriveMap` 根类 | 2天 |

### Phase 2: I/O 能力 (1周)

| 任务 | 描述 | 预计工时 |
|------|------|----------|
| 2.1 | 实现 `XodrWriter` - 地图保存功能 | 3天 |
| 2.2 | 完善 `OpenDriveParser` 支持新模型 | 2天 |
| 2.3 | 添加文件格式验证 | 1天 |

### Phase 3: 编辑核心 (2-3周)

| 任务 | 描述 | 预计工时 |
|------|------|----------|
| 3.1 | 实现 `MapEditor` + 撤销/重做 | 3天 |
| 3.2 | 实现基本 `EditOperation` | 3天 |
| 3.3 | 实现 `ValidationEngine` | 2天 |
| 3.4 | 实现 `SelectionManager` | 2天 |

### Phase 4: UI 功能 (2-3周)

| 任务 | 描述 | 预计工时 |
|------|------|----------|
| 4.1 | 完善主窗口 + 道路树 | 2天 |
| 4.2 | 实现属性面板 | 2天 |
| 4.3 | 实现道路编辑器对话框 | 3天 |
| 4.4 | 实现车道编辑器对话框 | 3天 |
| 4.5 | 实现交叉口编辑器 | 2天 |

### Phase 5: 高级功能 (2周)

| 任务 | 描述 | 预计工时 |
|------|------|----------|
| 5.1 | 实现交互式几何编辑（拖拽控制点） | 4天 |
| 5.2 | 实现道路/车道连接工具 | 3天 |
| 5.3 | 添加更多验证规则 | 2天 |
| 5.4 | 性能优化 | 3天 |

---

## 7. 关键设计决策

### 7.1 为什么需要独立的数据模型？

**现状问题**：
- 当前直接依赖 `RoadManager` 库解析XODR
- `RoadManager` 的数据结构不适合编辑场景
- 无法保存编辑后的地图

**解决方案**：
- 构建独立的 `map_base` 数据模型
- 完全掌控数据结构和序列化逻辑
- 支持双向转换：`XODR ↔ map_base ↔ UI`

### 7.2 为什么不直接编辑RoadManager数据？

1. **数据所有权**：编辑需要修改数据结构，`RoadManager` 是只读的
2. **序列化**：`RoadManager` 不支持将修改写回XODR
3. **灵活性**：自定义数据结构更方便添加编辑器特有字段

### 7.3 编辑器架构选择

采用 **Command Pattern** + **Undo/Redo Stack**：
- 每个编辑操作封装为一个 `EditOperation`
- 操作执行后压入Undo栈
- 支持任意层级撤销

---

## 8. 总结

本方案的核心改进点：

1. **独立数据模型** - 摆脱对RoadManager的强依赖
2. **完整几何表示** - 支持OpenDRIVE所有几何类型
3. **双向I/O** - 不仅能读取XODR，还能保存修改
4. **撤销/重做** - 完整的编辑历史支持
5. **拓扑验证** - 自动检查地图一致性
6. **模块化设计** - UI、编辑逻辑、数据模型解耦

实施后，地图编辑器将具备完整的创建、编辑、验证、保存能力。

