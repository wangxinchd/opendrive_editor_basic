#include "opendrive_parser.h"
#include "RoadManager/RoadManager.hpp"
#include "map_sdk/map_base/map.h"
#include <algorithm>
#include <fstream>
#include <memory>

namespace map_sdk {

bool OpenDriveMapParser::Parse(const std::string &file_path,
                               map_base::Map &out_map) {

  out_map.clear();
  if (!roadmanager::Position::LoadOpenDrive(file_path.c_str())) {
    return false;
  }
  // 得到file_path的文件名
  std::string file_name = file_path.substr(file_path.find_last_of('/') + 1);
  out_map.setName(file_name);

  roadmanager::Position *pos = new roadmanager::Position();
  roadmanager::OpenDrive *od = roadmanager::Position::GetOpenDrive();
  roadmanager::GeoReference *geo_ref = od->GetGeoReference();

  map_base::Map::Metadata metadata;
  int odr_road_size = od->GetNumOfRoads();
  for (int i = 0; i < odr_road_size; i++) {
    roadmanager::Road *odr_road = od->GetRoadByIdx(i);
    int odr_lane_section_size = odr_road->GetNumberOfLaneSections();
    if (odr_lane_section_size > 1) {
      printf("仅仅只能处理只有1个lane_section的road\n");
      return false;
    }

    std::shared_ptr<map_base::Road> road_ptr =
        std::make_shared<map_base::Road>();
    road_ptr->setId(odr_road->GetId());
    for (int j = 0; j < odr_lane_section_size; j++) {
      roadmanager::LaneSection *odr_lane_section =
          odr_road->GetLaneSectionByIdx(j);

      // 忽略车道中心线车道
      for (int k = 1; k < odr_lane_section->GetNumberOfLanes(); k++) {
        roadmanager::Lane *odr_lane = odr_lane_section->GetLaneByIdx(k);

        std::shared_ptr<map_base::Lane> lane_ptr =
            std::make_shared<map_base::Lane>();
        std::shared_ptr<map_base::LaneBoundary> left_lane_boundary =
            std::make_shared<map_base::LaneBoundary>();
        std::shared_ptr<map_base::LaneBoundary> right_lane_boundary =
            std::make_shared<map_base::LaneBoundary>();
        double lane_start = odr_lane_section->GetS();
        double lane_end =
            odr_lane_section->GetS() + odr_lane_section->GetLength();
        int steps =
            std::max(1, static_cast<int>((lane_end - lane_start) / 2.0));

        lane_ptr->setId({odr_road->GetId(), 0, odr_lane->GetId()});
        lane_ptr->setLaneLength(odr_lane_section->GetLength());

        for (int m = 0; m < steps + 1; m++) {

          double lane_s = std::min(lane_end, lane_start + m * 2);

          pos->SetLanePos(odr_road->GetId(), odr_lane->GetId(), lane_s, 0, j);
          lane_ptr->addCenterlinePoint({pos->GetX(), pos->GetY(), pos->GetH()});

          metadata.min_x = std::min(metadata.min_x, pos->GetX());
          metadata.min_y = std::min(metadata.min_y, pos->GetY());

          metadata.max_x = std::max(metadata.max_x, pos->GetX());
          metadata.max_y = std::max(metadata.max_y, pos->GetY());

          double lane_width =
              odr_lane_section->GetWidth(lane_s, odr_lane->GetId());
          {
            common::Pose2d left_boundary_pose;
            left_boundary_pose.x =
                pos->GetX() - lane_width / 2.0 * std::sin(pos->GetH());
            left_boundary_pose.y =
                pos->GetY() + lane_width / 2.0 * std::cos(pos->GetH());
            left_boundary_pose.yaw = pos->GetH();
            left_lane_boundary->addPose(left_boundary_pose);
          }

          {
            common::Pose2d right_boundary_pose;
            right_boundary_pose.x =
                pos->GetX() + lane_width / 2.0 * std::sin(pos->GetH());
            right_boundary_pose.y =
                pos->GetY() - lane_width / 2.0 * std::cos(pos->GetH());
            right_boundary_pose.yaw = pos->GetH();
            right_lane_boundary->addPose(right_boundary_pose);
          }
        }

        lane_ptr->setLeftLaneBoundary(left_lane_boundary);
        lane_ptr->setRightLaneBoundary(right_lane_boundary);

        road_ptr->addLane(lane_ptr);
        out_map.addLane(lane_ptr);
      }

      metadata.center_x = (metadata.max_x + metadata.min_x) / 2.0;
      metadata.center_y = (metadata.max_y + metadata.min_y) / 2.0;

      out_map.addRoad(road_ptr);
      out_map.setMetadata(metadata);
    }
  }

#if 1
  // debug output map info in file.log
  std::ofstream file_log("file.log");
  // print metadata
  file_log << "metadata: " << std::endl;
  file_log << "center_x: " << metadata.center_x << std::endl;
  file_log << "center_y: " << metadata.center_y << std::endl;
  file_log << "min_x: " << metadata.min_x << std::endl;
  file_log << "max_x: " << metadata.max_x << std::endl;
  file_log << "min_y: " << metadata.min_y << std::endl;
  file_log << "max_y: " << metadata.max_y << std::endl;
  file_log << "min_z: " << metadata.min_z << std::endl;
  file_log << "max_z: " << metadata.max_z << std::endl;
  file_log << "x_bias: " << metadata.x_bias << std::endl;
  file_log << "y_bias: " << metadata.y_bias << std::endl;
  file_log << "z_bias: " << metadata.z_bias << std::endl;

  for (const auto &road : out_map.roads()) {
    file_log << "road id: " << road->id() << std::endl;
  }
  for (const auto &lane : out_map.lanes()) {
    file_log << "lane id: " << lane->id().road_id << ","
             << lane->id().section_id << "," << lane->id().lane_id << std::endl;

    int size = lane->centerline().size();
    file_log << "centerline size: " << size << std::endl;

    for (int i = 0; i < size; i++) {
      file_log << "centerline point: " << lane->centerline()[i].x << ","
               << lane->centerline()[i].y << ", ";
      file_log << "left boundary point: "
               << lane->leftLaneBoundary()->poses()[i].x << ","
               << lane->leftLaneBoundary()->poses()[i].y << ", ";
      file_log << "right boundary point: "
               << lane->rightLaneBoundary()->poses()[i].x << ","
               << lane->rightLaneBoundary()->poses()[i].y << ", ";
      file_log << std::endl;
    }
    file_log << std::endl;
  }

  file_log.close();
#endif

  return true;
}

} // namespace map_sdk