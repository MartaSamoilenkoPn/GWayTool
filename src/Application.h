#ifndef APPLICATION_H
#define APPLICATION_H

#include <wayland-client.h>
#include <iostream>

class Application {
public:
    Application();
    ~Application();

    int run();

private:
    struct wl_display* display;
    struct wl_registry* registry;

};

#endif // APPLICATION_H
