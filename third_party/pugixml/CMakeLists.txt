cmake_minimum_required(VERSION 3.10)

project(pugixml)

set(SOURCES
    pugixml.cpp)

add_library(pugixml STATIC ${SOURCES})
target_include_directories(pugixml PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})