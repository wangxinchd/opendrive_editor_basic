#ifndef OPENDRIVE_PARSER_H
#define OPENDRIVE_PARSER_H

#include "map_parser_base.h"

namespace map_sdk {

// OpenDRIVE 格式地图解析器
class OpenDriveMapParser : public MapParserBase {
public:
  bool Parse(const std::string &file_path, map_base::Map &out_map) override;
};

} // namespace map_sdk

#endif // OPENDRIVE_PARSER_H