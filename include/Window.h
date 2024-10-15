#ifndef WINDOW_H
#define WINDOW_H

#include <wayland-client.h>
#include <vector>
#include "Control.h"

class Window {
public:
    Window(int width, int height, wl_display* display);
    ~Window();

    void show();
    void addControl(Control* control);

private:
    int width;
    int height;
    wl_display* display;
    wl_surface* surface;
    wl_buffer* buffer;

    wl_compositor* compositor;
    wl_shm* shm;
    wl_seat* seat;
    wl_pointer* pointer;

    void* shmData;
    int shmSize;

    std::vector<Control*> controls;

    static void registryHandler(void* data, struct wl_registry* registry, unsigned int id, const char* interface, unsigned int version);
    static void pointerEnter(void* data, struct wl_pointer* wl_pointer, unsigned int serial,
                             struct wl_surface* surface, wl_fixed_t sx, wl_fixed_t sy);
    static void pointerLeave(void* data, struct wl_pointer* wl_pointer, unsigned int serial,
                             struct wl_surface* surface);
    static void pointerMotion(void* data, struct wl_pointer* wl_pointer, unsigned int time,
                              wl_fixed_t sx, wl_fixed_t sy);
    static void pointerButton(void* data, struct wl_pointer* wl_pointer, unsigned int serial,
                              unsigned int time, unsigned int button, unsigned int state);
    static void pointerAxis(void* data, struct wl_pointer* wl_pointer, unsigned int time,
                            unsigned int axis, wl_fixed_t value);

    Control* findControlAt(int x, int y);

    int pointerX;
    int pointerY;
    Control* hoveredControl;
    Control* pressedControl;
};

#endif // WINDOW_H
