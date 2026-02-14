cmake_minimum_required(VERSION 3.10)

project(common)

set(SOURCES
    geometry.cc
    odr_geometry.cc
    cubic_curve_fitter.cc
    )

add_library(${PROJECT_NAME}_lib STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}_lib
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(test_cubic_curve_fitter tests/test_cubic_curve_fitter.cc)
target_link_libraries(test_cubic_curve_fitter PRIVATE ${PROJECT_NAME}_lib)
target_include_directories(test_cubic_curve_fitter PRIVATE ${CMAKE_SOURCE_DIR})