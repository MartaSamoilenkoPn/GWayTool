#ifndef EVENTS_H
#define EVENTS_H

struct MouseEvent {
    int pointer_x;
    int pointer_y;
    unsigned int button;
    unsigned int time;
    MouseEvent() = default;
    MouseEvent(int pointer_x, int pointer_y, unsigned int button, unsigned int time) :
                pointer_x(pointer_x), pointer_y(pointer_y), button(button), time(time) {};
};

struct KeyboardEvent {
    unsigned int keycode;
    unsigned int time;
};

#endif
