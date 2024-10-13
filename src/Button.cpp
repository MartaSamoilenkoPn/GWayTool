#include "Button.h"
#include "Events.h"
#include <cairo/cairo.h>

Button::Button(int x, int y, int width, int height, const std::string& label)
    : Control(x, y, width, height), label(label), pressed(false) {}

void Button::draw(cairo_t* cr) {
    cairo_set_source_rgb(cr, pressed ? 0.6 : 0.8, 0.8, 0.8);
    cairo_rectangle(cr, x, y, width, height);
    cairo_fill(cr);

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_rectangle(cr, x, y, width, height);
    cairo_stroke(cr);

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 14);

    cairo_text_extents_t extents;
    cairo_text_extents(cr, label.c_str(), &extents);
    double text_x = x + (width - extents.width) / 2 - extents.x_bearing;
    double text_y = y + (height - extents.height) / 2 - extents.y_bearing;

    cairo_move_to(cr, text_x, text_y);
    cairo_show_text(cr, label.c_str());
}

void Button::onMouseEnter(const MouseEvent& event) {
    hovered = true;
}

void Button::onMouseLeave(const MouseEvent& event) {
    hovered = false;
}

void Button::onMouseDown(const MouseEvent& event) {
    if (event.button == 1) { // Left mouse button
        pressed = true;
    }
}

void Button::onMouseUp(const MouseEvent& event) {
    if (event.button == 1 && pressed) {
        pressed = false;
        onClick(event);
    }
}

void Button::onClick(const MouseEvent& event) {
    if (onClickCallback) {
        onClickCallback();
    }
}

void Button::setOnClick(std::function<void()> callback) {
    onClickCallback = callback;
}
