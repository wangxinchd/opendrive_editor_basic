cmake_minimum_required(VERSION 3.10)

project(map_engine)

set(SOURCES
    src/map_parser_factory.cc
    src/opendrive_parser.cc
    src/sumo_parser.cc
)


message("wangxin0000  ${CMAKE_SOURCE_DIR}/map_sdk/map_base")
add_library(${PROJECT_NAME}_lib STATIC ${SOURCES})



target_include_directories(${PROJECT_NAME}_lib
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}
)

