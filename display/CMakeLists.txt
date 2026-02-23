cmake_minimum_required(VERSION 3.10)

project(display)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt 自动处理
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(${PROJECT_NAME} display.cpp
                               main.cpp
                               display.ui

                               # draw
                               QPainter_wrapper.cc
                               draw_map.cc
                               display_common.cc

                               # display config
                               display_config/display_config.cc
                               display_config/display_config_dialog.cc
                               display_config/display_config_dialog.ui

                               uviz_keybroad.cc
                               uviz_mouse.cc

                               form_edit/edit_road/form_edit_road.ui
                               form_edit/edit_road/form_edit_road.cpp
                               )

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets
                                              map_engine_lib
                                              common_lib
)