//
// Created by anna on 12/7/24.
//

#ifndef GWAYTOOL_TEXT_FIELD_H
#define GWAYTOOL_TEXT_FIELD_H
#include "application.h"
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <cstdlib>
#include <xdg-shell-client-protocol.h>
#include <linux/input-event-codes.h>
#include "application.h"
#include <xkbcommon/xkbcommon.h>
#include <xkbcommon/xkbcommon-keysyms.h>

struct TextInput {
    int x, y, width, height;
    bool isFocused = false;
    std::string text;

    bool contains(int px, int py) const {
        return px >= x && px <= x + width && py >= y && py <= y + height;
    }

    void draw(cairo_t* cr) const {
        cairo_set_source_rgb(cr, isFocused ? 0.8 : 0.9, 0.9, 0.9);
        cairo_rectangle(cr, x, y, width, height);
        cairo_fill(cr);

        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_rectangle(cr, x, y, width, height);
        cairo_stroke(cr);

        cairo_move_to(cr, x + 10, y + height / 2);
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_show_text(cr, text.c_str());
    }

    void handleKeyPress(uint32_t keysym) {
        if (keysym == XKB_KEY_BackSpace && !text.empty()) {
            text.pop_back();
        } else if (keysym == XKB_KEY_Return) {
        } else {
            char buffer[8];
            int size = xkb_keysym_to_utf8(keysym, buffer, sizeof(buffer));
            if (size > 0) {
                buffer[size] = '\0';
                text += buffer;
            }
        }
    }
};

#endif //GWAYTOOL_TEXT_FIELD_H
