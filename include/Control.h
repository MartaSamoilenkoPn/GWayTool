#ifndef CONTROL_H
#define CONTROL_H

#include <functional>
#include "Color.h"
#include <cairo/cairo.h>
#include <wayland-client-protocol.h>

struct MouseEvent;
struct KeyboardEvent;

class Control {
public:
    Control(int x, int y, int width, int height);
    virtual ~Control() = default;

    virtual void draw(cairo_t* cr, cairo_surface_t* surface) = 0;

    virtual void setColor(Color color);

    virtual void onMouseEnter(const MouseEvent& event);
    virtual void onMouseLeave(const MouseEvent& event);
    virtual void onMouseMove(const MouseEvent& event);
    virtual void onMouseDown(const MouseEvent& event);
    virtual void onMouseUp(const MouseEvent& event);
    virtual void onClick(const MouseEvent& event);

    virtual void onKeyDown(const KeyboardEvent& event);
    virtual void onKeyUp(const KeyboardEvent& event);

    int getX() const;
    int getY() const;
    int getWidth() const;
    int getHeight() const;

    bool containsPoint(int px, int py) const;

protected:
    int x, y, width, height;
    bool hovered;
};

#endif // CONTROL_H
