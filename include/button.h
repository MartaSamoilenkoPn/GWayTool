#include <iostream>
#include "functional"

struct Button {
    int x, y, width, height;
    std::string label;
    std::function<void()> onClick;

    void draw(cairo_t* cr) const {
        // Draw button background
        cairo_set_source_rgb(cr, 0.2, 0.2, 0.8); // Blue
        cairo_rectangle(cr, x, y, width, height);
        cairo_fill(cr);

        // Draw button border
        cairo_set_source_rgb(cr, 0, 0, 0); // Black
        cairo_rectangle(cr, x, y, width, height);
        cairo_stroke(cr);

        // Draw button label
        cairo_set_source_rgb(cr, 1, 1, 1); // White
        cairo_set_font_size(cr, 20);
        cairo_move_to(cr, x + 10, y + height / 2 + 10);
        cairo_show_text(cr, label.c_str());
    }

    bool contains(int px, int py) const {
        return px >= x && px <= x + width && py >= y && py <= y + height;
    }
};
