#include <iostream>
#include "functional"

struct Button {
    int x, y, width, height;
    std::string label;
    std::function<void()> onClick;

    bool isInside(int px, int py) const {
        return px >= x && px <= x + width && py >= y && py <= y + height;
    }
};
