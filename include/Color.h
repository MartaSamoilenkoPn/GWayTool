#pragma once

class Color
{
public:
    float r, g, b, a;

    Color(const float r, const float g, const float b, const float a = 1) : r {r}, g {g}, b {b}, a {a} {}
    Color() : Color {0, 0, 0} {}

    static Color white() { return {1, 1, 1}; }
    static Color black() { return {0, 0, 0}; }
    static Color red() { return {1, 0, 0}; }
    static Color green() { return {0, 1, 0}; }
    static Color blue() { return {0, 0, 1}; }
    static Color yellow() { return {1, 1, 0}; }
    static Color cyan() { return {0, 1, 1}; }
    static Color magenta() { return {1, 0, 1}; }
    static Color gray() { return {0.2f, 0.2f, 0.2f}; }

    static Color darkGreen() { return {0, 0.5f, 0}; }
    static Color skyBlue() { return {.529f, .808f, .922f}; }
    static Color gold() { return {1.f, .843f, 0}; }
};