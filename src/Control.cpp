#include "Control.h"

Control::Control(int x, int y, int width, int height)
    : x(x), y(y), width(width), height(height), hovered(false) {}

int Control::getX() const { return x; }
int Control::getY() const { return y; }
int Control::getWidth() const { return width; }
int Control::getHeight() const { return height; }

bool Control::containsPoint(int px, int py) const {
    return px >= x && px <= x + width && py >= y && py <= y + height;
}

void Control::onMouseEnter(const MouseEvent& event) {}
void Control::onMouseLeave(const MouseEvent& event) {}
void Control::onMouseMove(const MouseEvent& event) {}
void Control::onMouseDown(const MouseEvent& event) {}
void Control::onMouseUp(const MouseEvent& event) {}
void Control::onClick(const MouseEvent& event) {}
void Control::onKeyDown(const KeyboardEvent& event) {}
void Control::onKeyUp(const KeyboardEvent& event) {}
