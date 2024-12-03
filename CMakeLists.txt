cmake_minimum_required(VERSION 3.20)
project(GWayTool)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)

# Define directories for generated files
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Path to xdg-shell protocol file
set(XDG_PROTOCOL_PATH "/usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml")

# Output paths for generated files
set(XDG_HEADER "${INCLUDE_DIR}/xdg-shell-client-protocol.h")
set(XDG_CODE "${SOURCE_DIR}/xdg-shell-protocol.c")

# Add custom command to generate protocol files
add_custom_command(
        OUTPUT ${XDG_HEADER} ${XDG_CODE}
        COMMAND wayland-scanner client-header ${XDG_PROTOCOL_PATH} ${XDG_HEADER}
        COMMAND wayland-scanner private-code ${XDG_PROTOCOL_PATH} ${XDG_CODE}
        DEPENDS ${XDG_PROTOCOL_PATH}
        COMMENT "Generating xdg-shell-client-protocol.h and xdg-shell-protocol.c"
)

# Use PkgConfig to find Wayland and related libraries
find_package(PkgConfig REQUIRED)
pkg_check_modules(WAYLAND REQUIRED wayland-client wayland-server wayland-protocols wayland-egl egl)
pkg_check_modules(XKBCOMMON REQUIRED xkbcommon)  # Add xkbcommon

# Define the executable
add_executable(GWayTool
        examples/main.cpp
        ${XDG_CODE}
        src/WaylandFramework.cpp
)

# Include directories
target_include_directories(GWayTool PRIVATE
        ${INCLUDE_DIR}
        ${WAYLAND_INCLUDE_DIRS}
        ${XKBCOMMON_INCLUDE_DIRS}  # Add xkbcommon include directories
        /usr/local/include/cairo
)

# Link libraries explicitly
target_link_libraries(GWayTool PRIVATE
        ${WAYLAND_LIBRARIES}
        ${XKBCOMMON_LIBRARIES}  # Link xkbcommon
        /usr/local/lib/libcairo.so
)

# Compiler and linker flags from pkg-config
target_compile_options(GWayTool PRIVATE ${WAYLAND_CFLAGS})
target_compile_options(GWayTool PRIVATE ${XKBCOMMON_CFLAGS})  # Add xkbcommon flags
target_link_options(GWayTool PRIVATE ${WAYLAND_LDFLAGS})
target_link_options(GWayTool PRIVATE ${XKBCOMMON_LDFLAGS})  # Add xkbcommon linker flags
