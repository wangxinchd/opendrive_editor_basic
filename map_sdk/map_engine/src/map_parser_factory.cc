#include "map_parser_factory.h"
#include "opendrive_parser.h"
#include "sumo_parser.h"
#include <utility>

namespace map_sdk {

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
