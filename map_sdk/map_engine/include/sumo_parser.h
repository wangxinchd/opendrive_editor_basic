#ifndef SUMO_PARSER_H
#define SUMO_PARSER_H

#include "map_parser_base.h"

namespace map_sdk {

// SUMO 格式地图解析器（目前为占位实现）
class SumoMapParser : public MapParserBase {
public:
  bool Parse(const std::string &file_path, map_base::Map &out_map) override;
};

} // namespace map_sdk

#endif // SUMO_PARSER_H