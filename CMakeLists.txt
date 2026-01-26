cmake_minimum_required(VERSION 3.10)

project(opendrive_editor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# # Add subdirectories
add_subdirectory(third_party/pugixml)

add_subdirectory(common)
add_subdirectory(CommonMini)
add_subdirectory(RoadManager)

add_subdirectory(display)
