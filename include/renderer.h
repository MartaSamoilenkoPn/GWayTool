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

    void drawText(const std::string& text, int x, int y, double r, double g, double b, int size);
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
    void drawLineChart(const std::vector<int>& x_values, const std::vector<int>& y_values, int x, int y, int width, int height,
                                      double r, double g, double b,
                                      const std::optional<std::string> &title);
    void drawPieChart(const std::vector<int>& values, int x, int y, int radius,
                                     const std::vector<std::tuple<double, double, double>>& colors,
                                     const std::optional<std::vector<std::string>>& optionalLabels,
                                     const std::optional<std::string>& title);

    void drawLine(int x1, int y1, int x2, int y2, double r, double g, double b, double lineWidth);
    void drawTable(const std::vector<std::vector<std::string>>& data,
                                  int x, int y, int cellWidth, int cellHeight,
                                  int rows, int cols,
                                  double r, double g, double b,
                                  double lineR, double lineG, double lineB);


private:
    cairo_device_t* cairo_device;
    cairo_surface_t* cairo_surface;
    cairo_t* cairo_context;
    std::vector<Button> buttons;
    double bg_r = 0.5, bg_g = 0.5, bg_b = 0.5;
};


#endif //GWAYTOOL_RENDERER_H
