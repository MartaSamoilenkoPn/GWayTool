#ifndef GWAYTOOL_APPLICATION_H
#define GWAYTOOL_APPLICATION_H

#include "renderer.h"

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

#endif //GWAYTOOL_APPLICATION_H
