#ifndef WAYLAND_FRAMEWORK_H
#define WAYLAND_FRAMEWORK_H

#include <wayland-client.h>
#include <wayland-client-protocol.h>
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <cairo/cairo-gl.h>
#include <xdg-shell-client-protocol.h>
#include <string>
#include "button.h"
#include <linux/input-event-codes.h>

class WaylandDisplay {
public:
    WaylandDisplay();
    ~WaylandDisplay();

    struct wl_display* getDisplay() const { return display; }
    struct wl_compositor* getCompositor() const { return compositor; }
    struct xdg_wm_base* getXdgWmBase() const { return xdg_wm_base; }

    void roundtrip();

private:
    struct wl_display* display;
    struct wl_registry* registry;
    struct wl_compositor* compositor;
    struct xdg_wm_base* xdg_wm_base;
    static const struct wl_pointer_listener pointer_listener;
    wl_seat* seat = nullptr;
    wl_pointer* pointer = nullptr;

    static void registryHandler(void* data, struct wl_registry* registry,
                                uint32_t id, const char* interface, uint32_t version);
    static void registryRemoveHandler(void* data, struct wl_registry* registry, uint32_t id);
    static void pointerMotionHandler(void* data, struct wl_pointer* pointer,
                                     uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y);
    static void pointerButtonHandler(void* data, struct wl_pointer* pointer,
                                     uint32_t serial, uint32_t time, uint32_t button, uint32_t state);

    static void pointerEnterHandler(void* data, struct wl_pointer* pointer,
                                    uint32_t serial, struct wl_surface* surface,
                                    wl_fixed_t surface_x, wl_fixed_t surface_y);
    static void pointerLeaveHandler(void* data, struct wl_pointer* pointer,
                                    uint32_t serial, struct wl_surface* surface);

    static void pointerAxisHandler(void* data, struct wl_pointer* pointer,
                                   uint32_t time, uint32_t axis, wl_fixed_t value);


    static const struct wl_registry_listener registry_listener;



};

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

class MyEGLContext {
public:
    MyEGLContext(struct wl_display* display);
    ~MyEGLContext();

    EGLDisplay getEGLDisplay() const { return egl_display; }
    EGLContext getEGLContext() const { return egl_context; }
    EGLConfig getEGLConfig() const { return egl_conf; }

    EGLSurface createWindowSurface(struct wl_egl_window* egl_window);

private:
    EGLDisplay egl_display;
    EGLContext egl_context;
    EGLConfig egl_conf;

    void initialize(struct wl_display* display);
};

class CairoRenderer {
public:
    CairoRenderer(MyEGLContext& egl, EGLSurface egl_surface);
    ~CairoRenderer();

    void drawText(const std::string& text, int x, int y, double r, double g, double b);
    void drawButton(const Button& button);

private:
    cairo_device_t* cairo_device;
    cairo_surface_t* cairo_surface;
};

class WaylandApplication {
public:
    WaylandApplication();
    ~WaylandApplication();

    void run();
    void handlePointerClick(int px, int py);
    std::vector<Button> buttons;


    void updatePointerPosition(int x, int y) {
        pointer_x = x;
        pointer_y = y;
    }

    int getPointerX() const { return pointer_x; }
    int getPointerY() const { return pointer_y; }


private:
    WaylandDisplay display;
    WaylandSurface surface;
    MyEGLContext egl;
    struct wl_egl_window* egl_window;
    EGLSurface egl_surface;
    CairoRenderer renderer;
    int pointer_x = 0;
    int pointer_y = 0;

};

#endif // WAYLAND_FRAMEWORK_H
