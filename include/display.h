#ifndef GWAYTOOL_DISPLAY_H
#define GWAYTOOL_DISPLAY_H

#include <wayland-client.h>
#include <wayland-client-protocol.h>
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <cairo/cairo-gl.h>
#include <xdg-shell-client-protocol.h>
#include <string>
#include "button.h"

class WaylandDisplay {
public:
    WaylandDisplay();
    ~WaylandDisplay();

    struct wl_display* getDisplay() const { return display; }
    struct wl_compositor* getCompositor() const { return compositor; }
    struct xdg_wm_base* getXdgWmBase() const { return xdg_wm_base; }

    void roundtrip();
    static void pointerButtonHandler(void* data, struct wl_pointer* pointer,
                                     uint32_t serial, uint32_t time, uint32_t button,
                                     uint32_t state);

private:
    struct wl_display* display;
    struct wl_registry* registry;
    struct wl_compositor* compositor;
    struct xdg_wm_base* xdg_wm_base;
    static void registryHandler(void* data, struct wl_registry* registry,
                                uint32_t id, const char* interface, uint32_t version);
    static void registryRemoveHandler(void* data, struct wl_registry* registry, uint32_t id);
    static const struct wl_registry_listener registry_listener;



};

#endif //GWAYTOOL_DISPLAY_H
