cmake_minimum_required(VERSION 3.15)
project(GWayTool)

set(CMAKE_CXX_STANDARD 20)

include_directories(include ${CAIRO_INCLUDE_DIRS})
include_directories(src)

find_package(PkgConfig REQUIRED)

add_executable(GWayTool
    src/Application.cpp
    src/Application.h
        src/Control.h
        src/Button.h
        src/Button.cpp
        examples/main.cpp
        src/Window.cpp
        src/TextField.h
        src/Control.cpp
        src/Events.h)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")
find_package(Wayland)
include_directories(${Wayland_INCLUDE_DIR})
target_link_libraries(GWayTool ${WAYLAND_LIBRARIES})

find_package(Cairo)
include_directories(${CAIRO_INCLUDE_DIRS})
target_link_libraries(GWayTool ${CAIRO_LIBRARIES})

target_link_libraries(GWayTool EGL)

