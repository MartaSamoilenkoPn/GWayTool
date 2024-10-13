#ifndef EVENTS_H
#define EVENTS_H

struct MouseEvent {
    int pointer_x;
    int pointer_y;
    int button;
    unsigned int time;
    // MouseEvent() = default;
    MouseEvent(int pointer_x, int pointer_y, int button, unsigned int time);
};

struct KeyboardEvent {
    unsigned int keycode;
    unsigned int time;
};

#endif
