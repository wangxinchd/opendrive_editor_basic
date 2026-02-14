#ifndef MAP_PARSER_FACTORY_H
#define MAP_PARSER_FACTORY_H

#include <memory>
#include <string>

#include "map_defs.h"
#include "map_parser_base.h"

namespace map_sdk {

/**
 * @brief 地图解析器工厂，根据 MapType 生成对应格式的解析器实例。
 */
class MapParserFactory {
public:
  /**
   * @brief 创建对应类型的地图解析器
   * @param type 地图类型（OpenDRIVE / SUMO 等）
   * @return 解析器智能指针；如果类型不支持则返回 nullptr
   */
  static std::unique_ptr<MapParserBase> CreateMapParser(MapType type);
};

} // namespace map_sdk

#endif // MAP_PARSER_FACTORY_H
