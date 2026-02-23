cmake_minimum_required(VERSION 3.10)

project(common_mini)
set(SOURCES
    CommonMini.cpp
    version.cpp)

add_library(${PROJECT_NAME}_lib STATIC ${SOURCES})