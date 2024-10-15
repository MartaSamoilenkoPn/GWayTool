#include "Window.h"
#include "Events.h"
#include <iostream>
#include <cstring>
#include <stdexcept>
#include <cstdlib>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <syscall.h>
#include <unistd.h>
#include <sys/mman.h>

static int createAnonymousFile(off_t size) {
    char templateName[] = "/wl_shm-XXXXXX";
    const char* path = getenv("XDG_RUNTIME_DIR");
    if (!path) {
        throw std::runtime_error("XDG_RUNTIME_DIR not set");
    }

    std::string fullPath = std::string(path) + templateName;
    int fd = mkstemp(&fullPath[0]);
    if (fd < 0) {
        throw std::runtime_error("Failed to create a temporary file");
    }

    unlink(fullPath.c_str());

    if (ftruncate(fd, size) < 0) {
        close(fd);
        throw std::runtime_error("Failed to set size on temporary file");
    }

    return fd;
}

void Window::registryHandler(void* data, struct wl_registry* registry, unsigned int id, const char* interface, unsigned int version) {
    Window* window = static_cast<Window*>(data);
    if (strcmp(interface, wl_compositor_interface.name) == 0) {
        window->compositor = static_cast<wl_compositor*>(wl_registry_bind(registry, id, &wl_compositor_interface, 1));
    } else if (strcmp(interface, wl_shm_interface.name) == 0) {
        window->shm = static_cast<wl_shm*>(wl_registry_bind(registry, id, &wl_shm_interface, 1));
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

Window::Window(int width, int height, wl_display* display)
    : width(width), height(height), display(display), surface(nullptr),
      compositor(nullptr), shm(nullptr), seat(nullptr), pointer(nullptr),
      shmData(nullptr), shmSize(0), buffer(nullptr),
      hoveredControl(nullptr), pressedControl(nullptr) {

    if (!display) {
        throw std::runtime_error("Invalid Wayland display");
    }

    wl_registry* registry = wl_display_get_registry(display);

    static const wl_registry_listener registryListener = {
        registryHandler,
        nullptr
    };

    wl_registry_add_listener(registry, &registryListener, this);
    wl_display_roundtrip(display);

    if (!compositor) {
        throw std::runtime_error("Failed to get wl_compositor");
    }
    if (!shm) {
        throw std::runtime_error("Failed to get wl_shm");
    }

    surface = wl_compositor_create_surface(compositor);

    int stride = width * 4;
    shmSize = stride * height;

    // int fd = createAnonymousFile(shmSize);

    int fd = syscall(SYS_memfd_create, "buffer", 0);
    ftruncate(fd, shmSize);

    shmData = mmap(nullptr, shmSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shmData == MAP_FAILED) {
        close(fd);
        throw std::runtime_error("Failed to mmap buffer");
    }

    wl_shm_pool* pool = wl_shm_create_pool(shm, fd, shmSize);

    buffer = wl_shm_pool_create_buffer(pool, 0, width, height, stride, WL_SHM_FORMAT_ARGB8888);

    wl_surface_attach(surface, buffer, 0, 0);
    wl_surface_commit(surface);

    wl_shm_pool_destroy(pool);
    close(fd);

    memset(shmData, 0, shmSize);
}

Window::~Window() {
    if (pointer) {
        wl_pointer_destroy(pointer);
    }
    if (seat) {
        wl_seat_destroy(seat);
    }
    if (buffer) {
        wl_buffer_destroy(buffer);
    }
    if (shmData) {
        munmap(shmData, shmSize);
    }
    if (surface) {
        wl_surface_destroy(surface);
    }
    if (compositor) {
        wl_compositor_destroy(compositor);
    }
}

void Window::addControl(Control* control) {
    controls.push_back(control);
}

void Window::show() {
    std::cout << __FUNCTION__ << std::endl;

    unsigned int* pixels = static_cast<unsigned int*>(shmData);
    int totalPixels = width * height;
    unsigned int color = 0xFFFFFFFF;

    for (int i = 0; i < totalPixels; ++i) {
        pixels[i] = color;
    }

    for (auto control : controls) {
        control->draw(shmData, width, height);
    }

    wl_surface_damage(surface, 0, 0, width, height);
    wl_surface_commit(surface);
    wl_display_flush(display);

    while (1) {
        wl_display_dispatch(display);
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

void Window::pointerEnter(void* data, struct wl_pointer* wl_pointer, unsigned int serial,
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

void Window::pointerLeave(void* data, struct wl_pointer* wl_pointer, unsigned int serial,
                          struct wl_surface* surface) {
    Window* window = static_cast<Window*>(data);
    if (window->hoveredControl) {
        MouseEvent event{window->pointerX, window->pointerY, 0, 0};
        window->hoveredControl->onMouseLeave(event);
        window->hoveredControl = nullptr;
    }
}

void Window::pointerMotion(void* data, struct wl_pointer* wl_pointer, unsigned int time,
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

void Window::pointerButton(void* data, struct wl_pointer* wl_pointer, unsigned int serial,
                           unsigned int time, unsigned int button, unsigned int state) {
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

void Window::pointerAxis(void* data, struct wl_pointer* wl_pointer, unsigned int time,
                         unsigned int axis, wl_fixed_t value) {
}
