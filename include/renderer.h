#ifndef GWAYTOOL_RENDERER_H
#define GWAYTOOL_RENDERER_H
#include "context.h"
#include "text-field.h"
#include <optional>
#pragma once

class CairoRenderer {
public:
    CairoRenderer(MyEGLContext& egl, EGLSurface egl_surface);
    ~CairoRenderer();

    void drawText(const std::string& text, int x, int y, double r, double g, double b);
    void drawImage(const std::string& imagePath, int x, int y, double scaleX, double scaleY);
    void addButton(Button button) {
        std::cout << "checking move" << std::endl;
        buttons.emplace_back(std::move(button));
    }


    void handleClick(int x, int y);
    void drawButton();
    void drawTextInput(const TextInput &textInput);
    void clearArea(int x, int y, int width, int height);
    void drawBarChart(const std::vector<int>& values, int x, int y, int width, int height,
                      double r, double g, double b,
                      const std::optional<std::vector<std::string>>& optionalLabels = std::nullopt,
                      const std::optional<std::string> &title = std::nullopt);

private:
    cairo_device_t* cairo_device;
    cairo_surface_t* cairo_surface;
    cairo_t* cairo_context;
    std::vector<Button> buttons;
    double bg_r = 0.5, bg_g = 0.5, bg_b = 0.5;
};


#endif //GWAYTOOL_RENDERER_H
