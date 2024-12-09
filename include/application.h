#ifndef GWAYTOOL_APPLICATION_H
#define GWAYTOOL_APPLICATION_H

#include "renderer.h"
#include <wayland-client.h>
#include <wayland-client-protocol.h>
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <cairo/cairo-gl.h>
#include <xdg-shell-client-protocol.h>
#include <string>
#include <xkbcommon/xkbcommon.h>
#include <xkbcommon/xkbcommon-compose.h>
#include <vector>
#include "button.h"
#include "text-field.h"
#include <sstream>
#include <fstream>

class WaylandApplication {
public:
    WaylandApplication();
    ~WaylandApplication();


    void run();
    void onMouseClick(int x, int y);
    static const struct wl_keyboard_listener keyboard_listener;
    void onMouseMove(int x, int y);
    void onMouseRelease(int x, int y);
private:
    WaylandDisplay display;
    WaylandSurface surface;
    MyEGLContext egl;
    struct wl_egl_window* egl_window;
    EGLSurface egl_surface;
    CairoRenderer renderer;
    std::vector<std::string> lines;


    std::string input_text;

    struct xkb_context* xkbContext = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    struct xkb_keymap* xkbKeymap = nullptr;
    struct xkb_state* xkbState = nullptr;
    TextInput textInput{230, 30, 400, 20};



    static void keyboardKeyHandler(void* data, struct wl_keyboard* keyboard,
                                   uint32_t serial, uint32_t time, uint32_t key,
                                   uint32_t state);

};
#endif //GWAYTOOL_APPLICATION_H
