#include "Application.h"
#include <stdexcept>

Application::Application() {
    display = wl_display_connect(NULL);
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
    std::cout << "Running..." << std::endl;
    while (true) {
        wl_display_dispatch(display);
        wl_display_flush(display);
    }
    return 0;
}

wl_display* Application::getDisplay() const {
    return display;
}
