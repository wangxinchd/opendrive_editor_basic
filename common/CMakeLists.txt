cmake_minimum_required(VERSION 3.10)

project(common)

set(SOURCES
    geometry.cpp
    polynomial.cpp
    )

add_library(${PROJECT_NAME}_lib STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}_lib
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)