#include "WaylandFramework.h"
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <cstdlib>
#include <xdg-shell-client-protocol.h>
#include <linux/input-event-codes.h>

static int pointer_x = 0;
static int pointer_y = 0;

// -------------------- WaylandDisplay Implementation --------------------
const struct wl_registry_listener WaylandDisplay::registry_listener = {
        .global = WaylandDisplay::registryHandler,
        .global_remove = WaylandDisplay::registryRemoveHandler,
};

static void pointerEnterHandler(void* data, struct wl_pointer* pointer, uint32_t serial,
                                struct wl_surface* surface, wl_fixed_t x, wl_fixed_t y) {
    pointer_x = wl_fixed_to_int(x);
    pointer_y = wl_fixed_to_int(y);
    std::cout << "Pointer entered surface at: (" << pointer_x << ", " << pointer_y << ")\n";
}

// Handler for pointer leave event
static void pointerLeaveHandler(void* data, struct wl_pointer* pointer, uint32_t serial,
                                struct wl_surface* surface) {
    std::cout << "Pointer left the surface.\n";
}

// Handler for pointer button event
static void pointerButtonHandler(void* data, struct wl_pointer* pointer, uint32_t serial,
                                 uint32_t time, uint32_t button, uint32_t state) {
    const char* state_str = (state == WL_POINTER_BUTTON_STATE_PRESSED) ? "pressed" : "released";
    std::cout << "Button " << button << " " << state_str << " at (" << pointer_x << ", " << pointer_y << ")\n";
}

// Handler for pointer axis (scroll) event
static void pointerAxisHandler(void* data, struct wl_pointer* pointer, uint32_t time,
                               uint32_t axis, wl_fixed_t value) {
    const char* axis_str = (axis == WL_POINTER_AXIS_VERTICAL_SCROLL) ? "vertical" : "horizontal";
    std::cout << "Scroll " << axis_str << " by " << wl_fixed_to_double(value) << "\n";
}

// Handler for pointer motion event
static void pointerMotionHandler(void* data, struct wl_pointer* pointer, uint32_t time,
                                 wl_fixed_t x, wl_fixed_t y) {
    std::cout << "Raw motion coordinates: x=" << x << ", y=" << y << "\n";

    pointer_x = wl_fixed_to_int(x);
    pointer_y = wl_fixed_to_int(y);
    std::cout << "Pointer moved to: (" << pointer_x << ", " << pointer_y << ")\n";
}

// Pointer listener
static const struct wl_pointer_listener pointer_listener = {
        .enter = pointerEnterHandler,
        .leave = pointerLeaveHandler,
        .motion = pointerMotionHandler,
        .button = pointerButtonHandler,
        .axis = pointerAxisHandler
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

        if (pointer) {
            wl_pointer_add_listener(pointer, &pointer_listener, nullptr);
        } else {
            std::cerr << "Failed to get pointer for seat " << id << "\n";
        }
    }

}

void WaylandDisplay::pointerButtonHandler(void* data, struct wl_pointer* pointer,
                                 uint32_t serial, uint32_t time, uint32_t button,
                                 uint32_t state) {
    if (state == WL_POINTER_BUTTON_STATE_PRESSED && button == BTN_LEFT) {
        auto* app = static_cast<WaylandApplication*>(data);
        app->onMouseClick(pointer_x, pointer_y);
    }
}

void WaylandDisplay::registryRemoveHandler(void* data, struct wl_registry* registry, uint32_t id) {
    std::cout << "Global object removed: ID = " << id << "\n";
}

void WaylandDisplay::roundtrip() {
    wl_display_roundtrip(display);
}

// -------------------- WaylandSurface Implementation --------------------
const struct xdg_surface_listener WaylandSurface::xdg_surface_listener = {
        .configure = WaylandSurface::xdgSurfaceConfigureHandler,
};

const struct xdg_toplevel_listener WaylandSurface::xdg_toplevel_listener = {
        .configure = WaylandSurface::xdgToplevelConfigureHandler,
        .close = WaylandSurface::xdgToplevelCloseHandler,
};


void WaylandSurface::xdgSurfaceConfigureHandler(void* data, struct xdg_surface* xdg_surface, uint32_t serial) {
    xdg_surface_ack_configure(xdg_surface, serial);
}

void WaylandSurface::xdgToplevelConfigureHandler(void* data, struct xdg_toplevel* toplevel,
                                                 int32_t width, int32_t height, struct wl_array* states) {
    std::cout << "Resize event: " << width << "x" << height << "\n";
}

WaylandSurface::WaylandSurface(WaylandDisplay& display) {
    surface = wl_compositor_create_surface(display.getCompositor());
    if (!surface) {
        throw std::runtime_error("Failed to create Wayland surface");
    }

    xdg_surface = xdg_wm_base_get_xdg_surface(display.getXdgWmBase(), surface);
    xdg_surface_add_listener(xdg_surface, &xdg_surface_listener, this);

    xdg_toplevel = xdg_surface_get_toplevel(xdg_surface);
    xdg_toplevel_add_listener(xdg_toplevel, &xdg_toplevel_listener, this);

    wl_surface_commit(surface);
    display.roundtrip();

    std::cout << "Wayland surface and XDG shell setup complete.\n";
}

WaylandSurface::~WaylandSurface() {
    xdg_toplevel_destroy(xdg_toplevel);
    xdg_surface_destroy(xdg_surface);
    wl_surface_destroy(surface);
}



void WaylandSurface::xdgToplevelCloseHandler(void* data, struct xdg_toplevel* toplevel) {
    std::cerr << "Window close event received.\n";
}

// -------------------- EGLContext Implementation --------------------

MyEGLContext::MyEGLContext(struct wl_display* display) {
    initialize(display);
}

MyEGLContext::~MyEGLContext() {
    eglTerminate(egl_display);
    std::cout << "EGL terminated.\n";
}

void MyEGLContext::initialize(struct wl_display* display) {
    EGLint major, minor, count, n;
    EGLConfig* configs;
    EGLint size;

    EGLint config_attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_NONE
    };

    static const EGLint context_attribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
    };

    // Get the EGL display
    egl_display = eglGetDisplay((EGLNativeDisplayType)display);
    if (egl_display == EGL_NO_DISPLAY) {
        throw std::runtime_error("Failed to get EGL display");
    }
    std::cout << "Created EGL display.\n";

    // Initialize EGL
    if (eglInitialize(egl_display, &major, &minor) != EGL_TRUE) {
        throw std::runtime_error("Failed to initialize EGL");
    }
    std::cout << "EGL initialized: version " << major << "." << minor << "\n";

    // Bind the EGL API
    if (!eglBindAPI(EGL_OPENGL_API)) {
        throw std::runtime_error("Failed to bind EGL API");
    }

    // Get the EGL configurations
    eglGetConfigs(egl_display, NULL, 0, &count);
    std::cout << "EGL has " << count << " configurations.\n";

    configs = static_cast<EGLConfig*>(calloc(count, sizeof(EGLConfig)));
    if (!configs) {
        throw std::runtime_error("Failed to allocate memory for EGL configurations");
    }

    eglChooseConfig(egl_display, config_attribs, configs, count, &n);

    for (int i = 0; i < n; i++) {
        eglGetConfigAttrib(egl_display, configs[i], EGL_BUFFER_SIZE, &size);
        std::cout << "Buffer size for config " << i << " is " << size << "\n";

        eglGetConfigAttrib(egl_display, configs[i], EGL_RED_SIZE, &size);
        std::cout << "Red size for config " << i << " is " << size << "\n";

        // Use the first configuration
        egl_conf = configs[i];
        break;
    }

    free(configs);

    // Create the EGL context
    egl_context = eglCreateContext(egl_display, egl_conf, EGL_NO_CONTEXT, context_attribs);
    if (egl_context == EGL_NO_CONTEXT) {
        throw std::runtime_error("Failed to create EGL context");
    }
    std::cout << "EGL context created successfully.\n";
}

EGLSurface MyEGLContext::createWindowSurface(struct wl_egl_window* egl_window) {
    EGLSurface surface = eglCreateWindowSurface(egl_display, egl_conf, egl_window, NULL);
    if (surface == EGL_NO_SURFACE) {
        throw std::runtime_error("Failed to create EGL window surface");
    }
    std::cout << "EGL window surface - Created" <<  "\n";
    return surface;
}

// -------------------- CairoRenderer Implementation --------------------

CairoRenderer::CairoRenderer(MyEGLContext& egl, EGLSurface egl_surface) {

    cairo_device = cairo_egl_device_create(egl.getEGLDisplay(), egl.getEGLContext());
    if (!cairo_device || cairo_device_status(cairo_device) != CAIRO_STATUS_SUCCESS) {
        cairo_status_t status = cairo_device_status(cairo_device);
        std::cerr << "Cairo device error: " << cairo_status_to_string(status) << "\n";
        throw std::runtime_error("Failed to create Cairo EGL device");
    }

    cairo_surface = cairo_gl_surface_create_for_egl(cairo_device, egl_surface, 720, 510);
    if (!cairo_surface) {
        throw std::runtime_error("Failed to create Cairo surface");
    }
}

CairoRenderer::~CairoRenderer() {
    cairo_surface_destroy(cairo_surface);
    cairo_device_destroy(cairo_device);
    std::cout << "Cairo resources released.\n";
}

void CairoRenderer::drawText(const std::string& text, int x, int y, double r, double g, double b) {
    cairo_t* cr = cairo_create(cairo_surface);

    cairo_set_source_rgb(cr, r, g, b);
    cairo_select_font_face(cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 40);
    cairo_move_to(cr, x, y);
    cairo_show_text(cr, text.c_str());

    cairo_gl_surface_swapbuffers(cairo_surface);
    cairo_destroy(cr);
}

void CairoRenderer::addButton(const Button& button) {
    buttons.push_back(button);
}

void CairoRenderer::draw() {
    cairo_t* cr = cairo_create(cairo_surface);

    // Clear background
    cairo_set_source_rgb(cr, 0.5, 0.5, 0.5); // Gray
    cairo_paint(cr);

    // Draw buttons
    for (const auto& button : buttons) {
        button.draw(cr);
    }

    cairo_gl_surface_swapbuffers(cairo_surface);
    cairo_destroy(cr);
}

void CairoRenderer::handleClick(int x, int y) {
    for (const auto& button : buttons) {
        if (button.contains(x, y)) {
            if (button.onClick) {
                button.onClick();
            }
            break;
        }
    }
}

// -------------------- WaylandApplication Implementation --------------------

WaylandApplication::WaylandApplication()
        : display(), surface(display), egl(display.getDisplay()),
          egl_window(wl_egl_window_create(surface.getSurface(), 720, 510)),
          egl_surface(egl.createWindowSurface(egl_window)),
          renderer(egl, egl_surface) {
    if (!egl_window) {
        throw std::runtime_error("Failed to create Wayland EGL window");
    }
    std::cout << "WaylandApplication initialized successfully.\n";
}

void WaylandApplication::onMouseClick(int x, int y) {
    renderer.handleClick(x, y);
}


WaylandApplication::~WaylandApplication() {
    wl_egl_window_destroy(egl_window);
    std::cout << "WaylandApplication resources cleaned up.\n";
}

void WaylandApplication::run() {
    std::cout << "Application running...\n";

    renderer.drawText("Hello, Wayland!", 100, 250, 1.0, 1.0, 1.0);

    renderer.addButton({100, 100, 150, 50, "Click Me!", []() {
        std::cout << "Button clicked!" << std::endl;
    }});

    renderer.addButton({300, 100, 150, 50, "Quit", [this]() {
        std::cout << "Exiting application." << std::endl;
        wl_display_disconnect(display.getDisplay());
        exit(0);
    }});

    renderer.draw();

    while (wl_display_dispatch(display.getDisplay()) != -1) {
        // Main event loop
    }
}
