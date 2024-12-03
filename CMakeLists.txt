cmake_minimum_required(VERSION 3.20)
project(GWayTool)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)

# Define the custom directory for generated files
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
        COMMENT "Generating xdg-shell-client-protocol.h and xdg-shell-protocol.c in ${INCLUDE_DIR} and ${SOURCE_DIR}"
)

# Use PkgConfig to find Wayland and Cairo libraries
find_package(PkgConfig REQUIRED)
pkg_check_modules(WAYLAND REQUIRED wayland-client wayland-server wayland-egl egl)
pkg_check_modules(XKBCOMMON REQUIRED xkbcommon)

# Include directories for Wayland, Cairo, and xkbcommon
include_directories(${WAYLAND_INCLUDE_DIRS})
include_directories(${XKBCOMMON_INCLUDE_DIRS})
include_directories("/usr/local/include/cairo" ${INCLUDE_DIR})

# Link directories for Wayland, Cairo, and xkbcommon
link_directories(${WAYLAND_LIBRARY_DIRS} ${XKBCOMMON_LIBRARY_DIRS})
link_directories("/usr/local/lib/")

# Define the executable
add_executable(GWayTool
        examples/main.cpp
        ${XDG_CODE}
        ${XDG_HEADER}
        include/application.h
        include/surface.h
        include/context.h
        include/renderer.h
        include/display.h
        src/WaylandFramework.cpp
)

# Link necessary libraries
target_link_libraries(GWayTool
        ${WAYLAND_LIBRARIES}
        ${XKBCOMMON_LIBRARIES}  # Link xkbcommon
        /usr/local/lib/libcairo.so
)

# Set compiler and linker flags from pkg-config
target_compile_options(GWayTool PRIVATE ${WAYLAND_CFLAGS_OTHER} ${XKBCOMMON_CFLAGS_OTHER})
target_link_options(GWayTool PRIVATE ${WAYLAND_LDFLAGS_OTHER} ${XKBCOMMON_LDFLAGS_OTHER})
