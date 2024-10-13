#include "Window.h"
#include "Events.h"
#include <iostream>
#include <cstring>
#include <stdexcept>


void Window::registryHandler(void* data, struct wl_registry* registry, uint32_t id, const char* interface, uint32_t version) {
    Window* window = static_cast<Window*>(data);
    if (strcmp(interface, wl_compositor_interface.name) == 0) {
        window->compositor = static_cast<wl_compositor*>(wl_registry_bind(registry, id, &wl_compositor_interface, 1));
    } else if (strcmp(interface, wl_seat_interface.name) == 0) {
        window->seat = static_cast<wl_seat*>(wl_registry_bind(registry, id, &wl_seat_interface, 1));
        window->pointer = wl_seat_get_pointer(window->seat);

        static const wl_pointer_listener pointerListener = {
            pointerEnter,
            pointerLeave,
            pointerMotion,
            pointerButton,
            pointerAxis
        };
        wl_pointer_add_listener(window->pointer, &pointerListener, window);
    }
}

Window::Window(int width, int height)
    : width(width), height(height), display(nullptr), surface(nullptr),
      compositor(nullptr), seat(nullptr), pointer(nullptr),
      cairoSurface(nullptr), hoveredControl(nullptr), pressedControl(nullptr) {
    display = wl_display_connect(nullptr);
    if (!display) {
        throw std::runtime_error("Failed to connect to Wayland display");
    }

    struct wl_registry* registry = wl_display_get_registry(display);
    static const wl_registry_listener registryListener = {
        registryHandler,
        nullptr
    };
    wl_registry_add_listener(registry, &registryListener, this);
    wl_display_roundtrip(display);

    if (!compositor) {
        throw std::runtime_error("Failed to get compositor");
    }

    surface = wl_compositor_create_surface(compositor);

}

Window::~Window() {
    if (pointer) {
        wl_pointer_destroy(pointer);
    }
    if (seat) {
        wl_seat_destroy(seat);
    }
    if (surface) {
        wl_surface_destroy(surface);
    }
    if (compositor) {
        wl_compositor_destroy(compositor);
    }
    if (display) {
        wl_display_disconnect(display);
    }
}

void Window::addControl(Control* control) {
    controls.push_back(control);
}

void Window::show() {
    while (true) {
        cairo_t* cr = cairo_create(cairoSurface);
        cairo_set_source_rgb(cr, 1, 1, 1); // White background
        cairo_paint(cr);

        for (auto control : controls) {
            control->draw(cr);
        }

        cairo_destroy(cr);

        wl_display_flush(display);
        wl_display_dispatch(display);

        break;
    }
}

Control* Window::findControlAt(int x, int y) {
    for (auto it = controls.rbegin(); it != controls.rend(); ++it) {
        if ((*it)->containsPoint(x, y)) {
            return *it;
        }
    }
    return nullptr;
}

void Window::pointerEnter(void* data, struct wl_pointer* wl_pointer, uint32_t serial,
                          struct wl_surface* surface, wl_fixed_t sx, wl_fixed_t sy) {
    Window* window = static_cast<Window*>(data);
    window->pointerX = wl_fixed_to_int(sx);
    window->pointerY = wl_fixed_to_int(sy);
    window->hoveredControl = window->findControlAt(window->pointerX, window->pointerY);
    if (window->hoveredControl) {
        MouseEvent event{window->pointerX, window->pointerY, 0, 0};
        window->hoveredControl->onMouseEnter(event);
    }
}

void Window::pointerLeave(void* data, struct wl_pointer* wl_pointer, uint32_t serial,
                          struct wl_surface* surface) {
    Window* window = static_cast<Window*>(data);
    if (window->hoveredControl) {
        MouseEvent event{window->pointerX, window->pointerY, 0, 0};
        window->hoveredControl->onMouseLeave(event);
        window->hoveredControl = nullptr;
    }
}

void Window::pointerMotion(void* data, struct wl_pointer* wl_pointer, uint32_t time,
                           wl_fixed_t sx, wl_fixed_t sy) {
    Window* window = static_cast<Window*>(data);
    int x = wl_fixed_to_int(sx);
    int y = wl_fixed_to_int(sy);
    window->pointerX = x;
    window->pointerY = y;

    Control* newHoveredControl = window->findControlAt(x, y);
    if (newHoveredControl != window->hoveredControl) {
        if (window->hoveredControl) {
            MouseEvent event{x, y, 0, time};
            window->hoveredControl->onMouseLeave(event);
        }
        if (newHoveredControl) {
            MouseEvent event{x, y, 0, time};
            newHoveredControl->onMouseEnter(event);
        }
        window->hoveredControl = newHoveredControl;
    }

    if (window->hoveredControl) {
        MouseEvent event{x, y, 0, time};
        window->hoveredControl->onMouseMove(event);
    }
}

void Window::pointerButton(void* data, struct wl_pointer* wl_pointer, uint32_t serial,
                            unsigned int time, unsigned int button, uint32_t state) {
    Window* window = static_cast<Window*>(data);
    if (window->hoveredControl) {
        MouseEvent event{window->pointerX, window->pointerY, button, time};
        if (state == WL_POINTER_BUTTON_STATE_PRESSED) {
            window->pressedControl = window->hoveredControl;
            window->pressedControl->onMouseDown(event);
        } else if (state == WL_POINTER_BUTTON_STATE_RELEASED) {
            window->pressedControl->onMouseUp(event);
            if (window->pressedControl == window->hoveredControl) {
                window->pressedControl->onClick(event);
            }
            window->pressedControl = nullptr;
        }
    }
}

void Window::pointerAxis(void* data, struct wl_pointer* wl_pointer, uint32_t time,
                         uint32_t axis, wl_fixed_t value) {
}
