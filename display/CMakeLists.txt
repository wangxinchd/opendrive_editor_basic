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
                               )

target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
