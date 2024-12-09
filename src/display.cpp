#include "display.h"

#include "application.h"


static void pointerEnterHandler(void* data, struct wl_pointer* pointer, uint32_t serial,
                                struct wl_surface* surface, wl_fixed_t x, wl_fixed_t y) {
    pointer_x = wl_fixed_to_int(x);
    pointer_y = wl_fixed_to_int(y);
    std::cout << "Pointer entered surface at: (" << pointer_x << ", " << pointer_y << ")\n";
}

static void pointerLeaveHandler(void* data, struct wl_pointer* pointer, uint32_t serial,
                                struct wl_surface* surface) {
    std::cout << "Pointer left the surface.\n";
}

static void pointerButtonHandler(void* data, struct wl_pointer* pointer, uint32_t serial,
                                 uint32_t time, uint32_t button, uint32_t state) {
    const char* state_str = (state == WL_POINTER_BUTTON_STATE_PRESSED) ? "pressed" : "released";
//    if (!textInputAdded){
    if (state_str == "pressed") {
        auto* app = static_cast<WaylandApplication*>(data);
        app->onMouseClick(pointer_x, pointer_y);
    }
    std::cout << "Button " << state_str << " at (" << pointer_x << ", " << pointer_y << ")\n";
//    }
//    else {
        WaylandApplication *app = static_cast<WaylandApplication *>(data);
        if (state == WL_POINTER_BUTTON_STATE_PRESSED) {
            app->onMouseClick(pointer_x, pointer_y);
        }
        else if (state == WL_POINTER_BUTTON_STATE_RELEASED) {
            app->onMouseRelease(pointer_x, pointer_y);
//    }
}}


static void pointerAxisHandler(void* data, struct wl_pointer* pointer, uint32_t time,
                               uint32_t axis, wl_fixed_t value) {
    const char* axis_str = (axis == WL_POINTER_AXIS_VERTICAL_SCROLL) ? "vertical" : "horizontal";
    std::cout << "Scroll " << axis_str << " by " << wl_fixed_to_double(value) << "\n";
}


static void pointerMotionHandler(void* data, struct wl_pointer* pointer, uint32_t time,
                                 wl_fixed_t x, wl_fixed_t y) {
    pointer_x = wl_fixed_to_int(x);
    pointer_y = wl_fixed_to_int(y);

    WaylandApplication* app = static_cast<WaylandApplication*>(data);
    app->onMouseMove(pointer_x, pointer_y);
}


static void pointerFrameHandler(void* data, struct wl_pointer* pointer) {
//    std::cout << "Pointer frame event received.\n";
    std::cout << std::endl;
}


static const struct wl_pointer_listener pointer_listener = {
        .enter = pointerEnterHandler,
        .leave = pointerLeaveHandler,
        .motion = pointerMotionHandler,
        .button = pointerButtonHandler,
        .axis = pointerAxisHandler,
        .frame = pointerFrameHandler,
};

WaylandDisplay::WaylandDisplay() {
    display = wl_display_connect(NULL);
    if (!display) {
        throw std::runtime_error("Failed to connect to Wayland display");
    }
    std::cout << "Connected to Wayland display.\n";

    registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &WaylandDisplay::registry_listener, this);

    wl_display_dispatch(display);
    wl_display_roundtrip(display);

    if (!compositor || !xdg_wm_base) {
        throw std::runtime_error("Failed to initialize Wayland compositor or XDG shell");
    }
    std::cout << "Wayland compositor and XDG shell initialized.\n";
}

WaylandDisplay::~WaylandDisplay() {
    wl_display_disconnect(display);
    std::cout << "Disconnected from Wayland display.\n";
}

void WaylandDisplay::registryHandler(void* data, struct wl_registry* registry,
                                      uint32_t id, const char* interface, uint32_t version) {
    auto* self = static_cast<WaylandDisplay*>(data);

    if (strcmp(interface, "wl_compositor") == 0) {
        self->compositor = static_cast<wl_compositor*>(
            wl_registry_bind(registry, id, &wl_compositor_interface, 4));
    } else if (strcmp(interface, "xdg_wm_base") == 0) {
        self->xdg_wm_base = static_cast<struct xdg_wm_base*>(
                wl_registry_bind(registry, id, &xdg_wm_base_interface, 1));
    } else if (strcmp(interface, "wl_seat") == 0) {
        struct wl_seat* seat = static_cast<wl_seat*>(wl_registry_bind(registry, id, &wl_seat_interface, version));
        struct wl_pointer* pointer = wl_seat_get_pointer(seat);

        if (seat) {
            self->keyboard = wl_seat_get_keyboard(seat);
            wl_keyboard_add_listener(self->keyboard, &WaylandApplication::keyboard_listener, self);
        }
        if (pointer) {
            wl_pointer_add_listener(pointer, &pointer_listener, self);
        } else {
            std::cerr << "Failed to get pointer for seat " << id << "\n";
        }
    }

}


void WaylandDisplay::registryRemoveHandler(void* data, struct wl_registry* registry, uint32_t id) {
    std::cout << "Global object removed: ID = " << id << "\n";
}

void WaylandDisplay::roundtrip() {
    wl_display_roundtrip(display);
}

const struct wl_registry_listener WaylandDisplay::registry_listener = {
    .global = WaylandDisplay::registryHandler,
    .global_remove = WaylandDisplay::registryRemoveHandler,
};