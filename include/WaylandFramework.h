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
    void draw() {
        cairo_t* cr = cairo_create(cairo_surface);

        // Clear background with the current color
        cairo_set_source_rgb(cr, bg_r, bg_g, bg_b);
        cairo_paint(cr);

        // Draw buttons
        for (const auto& button : buttons) {
            button.draw(cr);
        }

        cairo_gl_surface_swapbuffers(cairo_surface);
        cairo_destroy(cr);
    }

    void setBackgroundColor(double r, double g, double b) {
        bg_r = r;
        bg_g = g;
        bg_b = b;
    }
    void addButton(const Button& button);
    void handleClick(int x, int y);

private:
    cairo_device_t* cairo_device;
    cairo_surface_t* cairo_surface;
    std::vector<Button> buttons;
    double bg_r = 0.5, bg_g = 0.5, bg_b = 0.5;
};

class WaylandApplication {
public:
    WaylandApplication();
    ~WaylandApplication();

    void run();
    void onMouseClick(int x, int y);

private:
    WaylandDisplay display;
    WaylandSurface surface;
    MyEGLContext egl;
    struct wl_egl_window* egl_window;
    EGLSurface egl_surface;
    CairoRenderer renderer;
};

#endif // WAYLAND_FRAMEWORK_H
