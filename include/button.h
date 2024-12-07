#include <functional>
#include <string>
#include <iostream>
#pragma once

struct Button {
    int x, y, width, height;
    std::string label;
    std::function<void()> onClick;

    bool contains(int px, int py) const {
        return px >= x && px <= x + width && py >= y && py <= y + height;
    }

    void draw(cairo_t* cr) const {
        cairo_set_source_rgb(cr, 0.8, 0.6, 0.9);
        cairo_rectangle(cr, x, y, width, height);
        cairo_fill(cr);

        cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
        cairo_move_to(cr, x + 10, y + height / 2);
        cairo_show_text(cr, label.c_str());
    }
};
