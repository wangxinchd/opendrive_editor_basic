#ifndef MAP_PARSER_FACTORY_H
#define MAP_PARSER_FACTORY_H

#include <memory>
#include <mutex>
#include <string>

#include "map_defs.h"
#include "map_parser_base.h"

namespace map_sdk {

/**
 * @brief 地图解析器工厂，根据 MapType 生成对应格式的解析器实例。
 * 采用单例模式，确保全局只有一个工厂实例。
 */
class MapParserFactory {
public:
  /**
   * @brief 获取单例实例
   * @return MapParserFactory 的单例引用
   */
  static MapParserFactory &GetInstance();

  /**
   * @brief 创建对应类型的地图解析器
   * @param type 地图类型（OpenDRIVE / SUMO 等）
   * @return 解析器智能指针；如果类型不支持则返回 nullptr
   */
  std::unique_ptr<MapParserBase> CreateMapParser(MapType type);

  // 禁止拷贝和赋值
  MapParserFactory(const MapParserFactory &) = delete;
  MapParserFactory &operator=(const MapParserFactory &) = delete;

private:
  // 私有构造函数，防止外部创建实例
  MapParserFactory() = default;
  ~MapParserFactory() = default;

  // 静态单例指针
  static MapParserFactory *instance_;
  // 互斥锁，用于线程安全
  static std::mutex mutex_;
};

} // namespace map_sdk

#endif // MAP_PARSER_FACTORY_H
