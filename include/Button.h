#ifndef BUTTON_H
#define BUTTON_H

#include "Control.h"
#include <string>
#include <functional>

class Button : public Control {
public:
    Button(int x, int y, int width, int height, const std::string& label);
    ~Button() override = default;

    void draw(void* shmData, int windowWidth, int windowHeight) override;

    void onMouseEnter(const MouseEvent& event) override;
    void onMouseLeave(const MouseEvent& event) override;
    void onMouseDown(const MouseEvent& event) override;
    void onMouseUp(const MouseEvent& event) override;
    void onClick(const MouseEvent& event) override;

    void setOnClick(std::function<void()> callback);

private:
    std::string label;
    std::function<void()> onClickCallback;
    bool pressed;
};

#endif // BUTTON_H
