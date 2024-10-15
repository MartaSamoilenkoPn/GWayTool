#ifndef EVENTS_H
#define EVENTS_H

struct MouseEvent {
    int x;
    int y;
    unsigned int button; // 1: Left, 2: Right, 3: Middle
    unsigned int time;
};

struct KeyboardEvent {
    unsigned int keycode;
    unsigned int time;
};

#endif // EVENTS_H
