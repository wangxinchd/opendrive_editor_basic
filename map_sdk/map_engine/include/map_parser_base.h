#ifndef MAP_PARSER_BASE_H
#define MAP_PARSER_BASE_H

#include "map_sdk/map_base/map.h"
#include <string>

namespace map_sdk {

class MapParserBase {
public:
  virtual ~MapParserBase() = default;

  virtual bool Parse(const std::string &file_path, map_base::Map &out_map) = 0;
};

} // namespace map_sdk

#endif // MAP_PARSER_BASE_H
