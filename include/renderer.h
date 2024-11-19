#ifndef GWAYTOOL_RENDERER_H
#define GWAYTOOL_RENDERER_H
#include "context.h"

class CairoRenderer {
public:
    CairoRenderer(MyEGLContext& egl, EGLSurface egl_surface);
    ~CairoRenderer();

    void drawText(const std::string& text, int x, int y, double r, double g, double b);


    void addButton(const Button& button) {
        buttons.push_back(button);
    }

    void handleClick(int x, int y) {
        for (const auto& button : buttons) {
            if (button.contains(x, y)) {
                if (button.onClick) {
                    button.onClick();
                }
                break;
            }
        }
    }

    void drawButton();

//    void handleClick(int x, int y);

private:
    cairo_device_t* cairo_device;
    cairo_surface_t* cairo_surface;
    std::vector<Button> buttons;
    double bg_r = 0.5, bg_g = 0.5, bg_b = 0.5;
};

#endif //GWAYTOOL_RENDERER_H
