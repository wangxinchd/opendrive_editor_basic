#include "map_parser_factory.h"
#include "opendrive_parser.h"
#include "sumo_parser.h"
#include <utility>

namespace map_sdk {

// 静态成员初始化
MapParserFactory *MapParserFactory::instance_ = nullptr;
std::mutex MapParserFactory::mutex_;

MapParserFactory &MapParserFactory::GetInstance() {
  // 双重检查锁定（Double-Checked Locking）
  if (instance_ == nullptr) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (instance_ == nullptr) {
      instance_ = new MapParserFactory();
    }
  }
  return *instance_;
}

std::unique_ptr<MapParserBase> MapParserFactory::CreateMapParser(MapType type) {
  switch (type) {
  case MAP_TYPE_OPENDRIVE:
    return std::unique_ptr<MapParserBase>(new OpenDriveMapParser());
  case MAP_TYPE_SUMO:
    return std::unique_ptr<MapParserBase>(new SumoMapParser());
  default:
    return nullptr;
  }
}

} // namespace map_sdk
