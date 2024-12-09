#ifndef CHECKBOX_H
#define CHECKBOX_H

#include <string>
#include <functional>
#include <cairo.h>

struct Checkbox {
    int x, y;             // Position
    int size;             // Checkbox size (width and height)
    std::string label;    // Label text
    bool isChecked;       // Current state
    std::function<void(bool)> onToggle; // Callback function

    Checkbox(int x, int y, int size, const std::string& label, std::function<void(bool)> onToggle)
        : x(x), y(y), size(size), label(label), isChecked(false), onToggle(onToggle) {}

    // Check if a click is within the checkbox bounds
    bool contains(int px, int py) const {
        return px >= x && px <= x + size && py >= y && py <= y + size;
    }

    // Toggle state and call the callback
    void toggle() {
        isChecked = !isChecked;
        if (onToggle) {
            onToggle(isChecked);
        }
    }

    // Draw the checkbox with Cairo
    void draw(cairo_t* cr) const {
        // Draw border
        cairo_set_source_rgb(cr, 1.0, 1.0, 1.0); // Black border
        cairo_rectangle(cr, x, y, size, size);
        cairo_stroke(cr);

        // Fill box if checked
        if (isChecked) {
            cairo_set_source_rgb(cr, 0.0, 0.8, 0.0); // Green for checked
            cairo_rectangle(cr, x + 2, y + 2, size - 4, size - 4);
            cairo_fill(cr);
        }

        // Draw label
        cairo_set_source_rgb(cr, 1.0, 1.0, 1.0); // Black text
        cairo_move_to(cr, x + size + 10, y + size / 1.5);
        cairo_show_text(cr, label.c_str());
    }
};

#endif // CHECKBOX_H
