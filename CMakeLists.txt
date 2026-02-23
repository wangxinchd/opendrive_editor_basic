cmake_minimum_required(VERSION 3.10)

project(opendrive_editor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Third party libraries
add_subdirectory(third_party/pugixml)

# Common libraries
add_subdirectory(common)

# esmini
add_subdirectory(CommonMini)
add_subdirectory(RoadManager)

# map_sdk
add_subdirectory(map_sdk/map_engine)

# display
add_subdirectory(display)
