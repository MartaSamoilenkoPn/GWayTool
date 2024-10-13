#include "Application.h"
#include <stdexcept>

Application::Application() {
    display = wl_display_connect(nullptr);
    if (!display) {
        throw std::runtime_error("Failed to connect to Wayland display");
    }
}

Application::~Application() {
    if (display) {
        wl_display_disconnect(display);
    }
}

int Application::run() {
    while (wl_display_dispatch(display) != -1) {
        // Main event loop
    }
    return 0;
}
