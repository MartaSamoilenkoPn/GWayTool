#ifndef WAYLAND_FRAMEWORK_H
#define WAYLAND_FRAMEWORK_H

#include <wayland-client.h>
#include <wayland-client-protocol.h>
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <cairo/cairo-gl.h>
#include <xdg-shell-client-protocol.h>
#include <string>
#include <xkbcommon/xkbcommon.h>
#include <xkbcommon/xkbcommon-compose.h>


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
    void drawImage(const std::string& imagePath, int x, int y, double scaleX, double scaleY);

private:
    cairo_device_t* cairo_device;
    cairo_surface_t* cairo_surface;
};

class WaylandApplication {
public:
    WaylandApplication();
    ~WaylandApplication();

    void run();
    static const struct wl_keyboard_listener keyboard_listener;


private:
    WaylandDisplay display;
    WaylandSurface surface;
    MyEGLContext egl;
    struct wl_egl_window* egl_window;
    EGLSurface egl_surface;
    CairoRenderer renderer;

    std::string input_text;

    struct xkb_context* xkbContext = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    struct xkb_keymap* xkbKeymap = nullptr;
    struct xkb_state* xkbState = nullptr;


    static void keyboardKeyHandler(void* data, struct wl_keyboard* keyboard,
                                   uint32_t serial, uint32_t time, uint32_t key,
                                   uint32_t state);


};

#endif // WAYLAND_FRAMEWORK_H