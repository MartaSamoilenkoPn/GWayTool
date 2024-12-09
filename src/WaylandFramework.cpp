#include <iostream>
#include <stdexcept>
#include <cstring>
#include <cstdlib>
#include <xdg-shell-client-protocol.h>
#include <linux/input-event-codes.h>
#include "application.h"
#include <xkbcommon/xkbcommon.h>

WaylandApplication::WaylandApplication()
    : display(), surface(display), egl(display.getDisplay()),
      egl_window(wl_egl_window_create(surface.getSurface(), 720, 510)),
      egl_surface(egl.createWindowSurface(egl_window)),
      renderer(egl, egl_surface) {
    if (!egl_window) {
        throw std::runtime_error("Failed to create Wayland EGL window");
    }
    std::cout << "WaylandApplication initialized successfully.\n";
}

//void WaylandApplication::onMouseClick(int x, int y) {
//    std::cout << "it goes to on mouse click" << std::endl;
//    renderer.handleClick(x, y);
//}

const struct wl_keyboard_listener WaylandApplication::keyboard_listener = {
        .keymap = [](void* data, struct wl_keyboard* keyboard, uint32_t format, int fd, uint32_t size) {
        },
        .enter = [](void* data, struct wl_keyboard* keyboard, uint32_t serial, struct wl_surface* surface, struct wl_array* keys) {
        },
        .leave = [](void* data, struct wl_keyboard* keyboard, uint32_t serial, struct wl_surface* surface) {
        },
        .key = WaylandApplication::keyboardKeyHandler,
        .modifiers = [](void* data, struct wl_keyboard* keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group) {
        },
        .repeat_info = [](void* data, struct wl_keyboard* keyboard, int32_t rate, int32_t delay) {
        }
};


void WaylandApplication::onMouseRelease(int x, int y) {
    isDragging = false;
}
void WaylandApplication::onMouseMove(int x, int y) {
    if (isDragging) {
        std::cout << "Dragging started. Mouse position: (" << x << ", " << y << ")\n";

        int oldX = textInput.getX();
        int oldY = textInput.getY();
        std::cout << "Old position: (" << oldX << ", " << oldY << ")\n";

        textInput.setX(x - dragOffsetX);
        textInput.setY(y - dragOffsetY);

        std::cout << "Clearing area: (" << oldX << ", " << oldY << ")\n";
//        renderer.clearArea(oldX, oldY, textInput.width, textInput.height);
        renderer.clearArea(oldX - 4, oldY - 4, textInput.width + 8, textInput.height + 8);


        std::cout << "Drawing new position: (" << textInput.getX() << ", " << textInput.getY() << ")\n";
        renderer.drawTextInput(textInput);
    }
}

void WaylandApplication::onMouseClick(int x, int y) {
    if (textInput.textInputAdded) {
        if (textInput.contains(x, y)) {
            textInput.isFocused = true;
            std::cout << "TextInput focused.\n";

            if (!isDragging) {
                isDragging = true;
                std::cout << "Dragging initialized.\n";
                dragOffsetX = x - textInput.getX();
                dragOffsetY = y - textInput.getY();
            }
        } else {
            textInput.isFocused = false;
        }
        renderer.drawTextInput(textInput);
    }
    else {
        std::cout << "it goes to on mouse click" << std::endl;
        renderer.handleClick(x, y);
    }
}



void WaylandApplication::keyboardKeyHandler(void* data, struct wl_keyboard* keyboard,
                                            uint32_t serial, uint32_t time, uint32_t key,
                                            uint32_t state) {
    auto* app = static_cast<WaylandApplication*>(data);

    std::cout << "Keyboard event: key=" << key
              << ", state=" << (state == WL_KEYBOARD_KEY_STATE_PRESSED ? "PRESSED" : "RELEASED")
              << ", time=" << time << std::endl;

    if (state == WL_KEYBOARD_KEY_STATE_PRESSED) {
        struct xkb_context* ctx = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
        if (!ctx) {
            std::cerr << "Failed to create XKB context." << std::endl;
            return;
        }

        struct xkb_keymap* keymap = xkb_keymap_new_from_names(ctx, nullptr, XKB_KEYMAP_COMPILE_NO_FLAGS);
        if (!keymap) {
            std::cerr << "Failed to create XKB keymap." << std::endl;
            xkb_context_unref(ctx);
            return;
        }

        struct xkb_state* xkb_state = xkb_state_new(keymap);
        if (!xkb_state) {
            std::cerr << "Failed to create XKB state." << std::endl;
            xkb_keymap_unref(keymap);
            xkb_context_unref(ctx);
            return;
        }

        uint32_t keysym = xkb_state_key_get_one_sym(xkb_state, key + 8); // Додаємо 8 для коректного keycode
        char buffer[64];
        int size = xkb_keysym_to_utf8(keysym, buffer, sizeof(buffer));
        buffer[size] = '\0';

        std::cout << "Key pressed: keysym=" << keysym
                  << ", utf8='" << buffer << "'"
                  << ", keycode=" << key << std::endl;
//        app->textInput.isFocused = true;
        if (app->textInput.isFocused) {
            if (keysym == XKB_KEY_Escape) {
                std::cout << "Escape key detected. Unfocusing text input." << std::endl;
                app->textInput.isFocused = false;
            } else {
                app->textInput.handleKeyPress(keysym);
                app->renderer.drawTextInput(app->textInput);
            }
        }

        xkb_state_unref(xkb_state);
        xkb_keymap_unref(keymap);
        xkb_context_unref(ctx);
    }
}

WaylandApplication::~WaylandApplication() {
    wl_egl_window_destroy(egl_window);
    std::cout << "WaylandApplication resources cleaned up.\n";
}

void sayHelloWorld(){
    // just for testing
    std::cout << "hello world" << std::endl;
}

void WaylandApplication::run() {
    std::cout << "Application running...\n";

//    renderer.drawText("Hello, Wayland!", 100, 250, 1.0, 1.0, 1.0);

//    renderer.drawText("", 100, 250, 1.0, 1.0, 1.0);
//    renderer.drawTextInput(textInput);
    Button button1(400, 400, 150, 50, "button number 1", sayHelloWorld);
    renderer.addButton(button1);

    renderer.drawButton();

    Button button2(300, 100, 150, 50, "button number 2", sayHelloWorld);
    renderer.addButton(button2);
    renderer.drawButton();
//    button1.click();
//    renderer.drawImage("../sun.png", 100, 100, 0.5, 0.5);


    while (wl_display_dispatch(display.getDisplay()) != -1) {
        // Main event loop
    }
}

