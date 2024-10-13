#ifndef WINDOW_H
#define WINDOW_H

#include <wayland-client.h>
#include <cairo/cairo.h>
#include <vector>
#include "Control.h"
#include "Events.h"

class Window {
public:
    Window(int width, int height);
    ~Window();

    void show();
    void addControl(Control* control);

private:
    int width;
    int height;
    struct wl_display* display;
    struct wl_surface* surface;
    struct wl_compositor* compositor;
    struct wl_seat* seat;
    struct wl_pointer* pointer;

    cairo_surface_t* cairoSurface;
    std::vector<Control*> controls;

    static void registryHandler(void* data, struct wl_registry* registry, uint32_t id, const char* interface, uint32_t version);
    static void pointerEnter(void* data, struct wl_pointer* wl_pointer, uint32_t serial, struct wl_surface* surface, wl_fixed_t sx, wl_fixed_t sy);
    static void pointerLeave(void* data, struct wl_pointer* wl_pointer, uint32_t serial, struct wl_surface* surface);
    static void pointerMotion(void* data, struct wl_pointer* wl_pointer, uint32_t time, wl_fixed_t sx, wl_fixed_t sy);
    static void pointerButton(void* data, struct wl_pointer* wl_pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state);
    static void pointerAxis(void* data, struct wl_pointer* wl_pointer, uint32_t time, uint32_t axis, wl_fixed_t value);

    Control* findControlAt(int x, int y);

    int pointerX;
    int pointerY;
    Control* hoveredControl;
    Control* pressedControl;
};

#endif // WINDOW_H
