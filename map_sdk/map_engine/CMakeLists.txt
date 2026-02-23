cmake_minimum_required(VERSION 3.10)

project(map_engine)

set(SOURCES
    src/map_parser_factory.cc
    src/opendrive_parser.cc
    src/sumo_parser.cc
)

add_library(${PROJECT_NAME}_lib STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}_lib
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/third_party/pugixml
        ${CMAKE_SOURCE_DIR}/CommonMini
)
target_link_libraries(${PROJECT_NAME}_lib PRIVATE road_manager_lib
)

