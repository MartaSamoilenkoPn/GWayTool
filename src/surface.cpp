#include "surface.h"

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
