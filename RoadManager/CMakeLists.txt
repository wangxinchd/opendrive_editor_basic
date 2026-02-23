cmake_minimum_required(VERSION 3.10)

project(road_manager)

set(SOURCES
    RoadManager.cpp
    odrSpiral.cpp
    LaneIndependentRouter.cpp)

add_library(${PROJECT_NAME}_lib STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME}_lib PRIVATE
    pugixml
    common_mini_lib
)

target_include_directories(${PROJECT_NAME}_lib
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/pugixml
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../CommonMini
)


