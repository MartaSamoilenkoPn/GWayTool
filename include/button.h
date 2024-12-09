#include <functional>
#include <string>
#include <iostream>
#pragma once

struct Button {
    int x, y, width, height;
    std::string label;
    std::function<void()> onClick;

    Button(int x, int y, int width, int height, const std::string& label, std::function<void()> onClick)
            : x(x), y(y), width(width), height(height), label(label), onClick(onClick) {
        std::cout << "Button created" << std::endl;
    }
    Button(const Button& other)
            : x(other.x), y(other.y), width(other.width), height(other.height), label(other.label), onClick(other.onClick) {
        std::cout << "Button copied\n";
    }
    Button(Button&& other) noexcept
            : x(other.x), y(other.y), width(other.width), height(other.height), label(std::move(other.label)), onClick(other.onClick) {
        std::cout << "Button moved\n";
    }
    ~Button() {
        std::cout << "Button destroyed\n";
    }

    bool contains(int px, int py) const {
        return px >= x && px <= x + width && py >= y && py <= y + height;
    }

    void draw(cairo_t* cr) const {
        cairo_set_source_rgb(cr, 1.0, 1.0, 0.0);
        cairo_rectangle(cr, x, y, width, height);
        cairo_fill(cr);

        cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
        cairo_move_to(cr, x + 10, y + height / 2);
        cairo_show_text(cr, label.c_str());
    }
};
