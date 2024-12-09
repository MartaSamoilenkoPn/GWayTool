#ifndef WAYLAND_FRAMEWORK_H
#define WAYLAND_FRAMEWORK_H


#include <wayland-client.h>
#include <wayland-client-protocol.h>
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <cairo/cairo-gl.h>
#include <xdg-shell-client-protocol.h>
#include <string>
#include "display.h"


class WaylandSurface {
public:
    WaylandSurface(WaylandDisplay& display);
    ~WaylandSurface();

    struct wl_surface* getSurface() const { return surface; }
    struct xdg_surface* getXdgSurface() const { return xdg_surface; }
    struct xdg_toplevel* getXdgToplevel() const { return xdg_toplevel; }

private:
    struct wl_surface* surface;
    struct xdg_surface* xdg_surface;
    struct xdg_toplevel* xdg_toplevel;

    static void xdgSurfaceConfigureHandler(void* data, struct xdg_surface* xdg_surface, uint32_t serial);
    static void xdgToplevelConfigureHandler(void* data, struct xdg_toplevel* toplevel, int32_t width,
                                            int32_t height, struct wl_array* states);
    static void xdgToplevelCloseHandler(void* data, struct xdg_toplevel* toplevel);

    static const struct xdg_surface_listener xdg_surface_listener;
    static const struct xdg_toplevel_listener xdg_toplevel_listener;
};


#endif // WAYLAND_FRAMEWORK_H
