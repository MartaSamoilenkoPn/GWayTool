#ifndef APPLICATION_H
#define APPLICATION_H

#include <wayland-client.h>
#include <iostream>

class Application {
public:
    Application();
    ~Application();

    int run();

    wl_display* getDisplay() const;

private:
    struct wl_display* display;
};

#endif // APPLICATION_H
