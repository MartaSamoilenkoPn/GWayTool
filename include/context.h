#ifndef GWAYTOOL_CONTEXT_H
#define GWAYTOOL_CONTEXT_H

#include "surface.h"

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


#endif //GWAYTOOL_CONTEXT_H
