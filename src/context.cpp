#include "context.h"


MyEGLContext::MyEGLContext(struct wl_display* display) {
    initialize(display);
}

MyEGLContext::~MyEGLContext() {
    eglTerminate(egl_display);
    std::cout << "EGL terminated.\n";
}

void MyEGLContext::initialize(struct wl_display* display) {
    EGLint major, minor, count, n;
    EGLConfig* configs;
    EGLint size;

    EGLint config_attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };

    static const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    // Get the EGL display
    egl_display = eglGetDisplay((EGLNativeDisplayType)display);
    if (egl_display == EGL_NO_DISPLAY) {
        throw std::runtime_error("Failed to get EGL display");
    }
    std::cout << "Created EGL display.\n";

    // Initialize EGL
    if (eglInitialize(egl_display, &major, &minor) != EGL_TRUE) {
        throw std::runtime_error("Failed to initialize EGL");
    }
    std::cout << "EGL initialized: version " << major << "." << minor << "\n";

    // Bind the EGL API
    if (!eglBindAPI(EGL_OPENGL_API)) {
        throw std::runtime_error("Failed to bind EGL API");
    }

    // Get the EGL configurations
    eglGetConfigs(egl_display, NULL, 0, &count);
    std::cout << "EGL has " << count << " configurations.\n";

    configs = static_cast<EGLConfig*>(calloc(count, sizeof(EGLConfig)));
    if (!configs) {
        throw std::runtime_error("Failed to allocate memory for EGL configurations");
    }

    eglChooseConfig(egl_display, config_attribs, configs, count, &n);

    for (int i = 0; i < n; i++) {
        eglGetConfigAttrib(egl_display, configs[i], EGL_BUFFER_SIZE, &size);
        std::cout << "Buffer size for config " << i << " is " << size << "\n";

        eglGetConfigAttrib(egl_display, configs[i], EGL_RED_SIZE, &size);
        std::cout << "Red size for config " << i << " is " << size << "\n";

        // Use the first configuration
        egl_conf = configs[i];
        break;
    }

    free(configs);

    // Create the EGL context
    egl_context = eglCreateContext(egl_display, egl_conf, EGL_NO_CONTEXT, context_attribs);
    if (egl_context == EGL_NO_CONTEXT) {
        throw std::runtime_error("Failed to create EGL context");
    }
    std::cout << "EGL context created successfully.\n";
}

EGLSurface MyEGLContext::createWindowSurface(struct wl_egl_window* egl_window) {
    EGLSurface surface = eglCreateWindowSurface(egl_display, egl_conf, egl_window, NULL);
    if (surface == EGL_NO_SURFACE) {
        throw std::runtime_error("Failed to create EGL window surface");
    }
    std::cout << "EGL window surface - Created" <<  "\n";
    return surface;
}