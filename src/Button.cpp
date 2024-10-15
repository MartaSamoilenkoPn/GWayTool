#include "Button.h"
#include <cstring>

Button::Button(int x, int y, int width, int height, const std::string& label)
    : Control(x, y, width, height), label(label), pressed(false) {}

void Button::draw(void* shmData, int windowWidth, int windowHeight) {
    unsigned int* pixels = static_cast<unsigned int*>(shmData);
    int stride = windowWidth;

    unsigned int color = pressed ? 0xFFAAAAAA : (hovered ? 0xFFCCCCCC : 0xFFEEEEEE);

    for (int py = y; py < y + height; ++py) {
        for (int px = x; px < x + width; ++px) {
            if (px >= 0 && px < windowWidth && py >= 0 && py < windowHeight) {
                pixels[py * stride + px] = color;
            }
        }
    }

}

void Button::onMouseEnter(const MouseEvent& event) {
    hovered = true;
}

void Button::onMouseLeave(const MouseEvent& event) {
    hovered = false;
}

void Button::onMouseDown(const MouseEvent& event) {
    if (event.button == 1) {
        pressed = true;
    }
}

void Button::onMouseUp(const MouseEvent& event) {
    if (event.button == 1 && pressed) {
        pressed = false;
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
