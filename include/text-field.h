//
// Created by anna on 12/7/24.
//

#ifndef GWAYTOOL_TEXT_FIELD_H
#define GWAYTOOL_TEXT_FIELD_H
#include "application.h"
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <cstdlib>
#include <xdg-shell-client-protocol.h>
#include <linux/input-event-codes.h>
#include "application.h"
#include <xkbcommon/xkbcommon.h>
#include <xkbcommon/xkbcommon-keysyms.h>
#include <fstream>
struct TextInput {
    int x, y, width, height;
    bool isFocused = false;
    int lineHeight = 20;
    std::vector<std::string> lines = {""};
    std::ofstream textFile;

    TextInput(int x, int y, int width, int height)
            : x(x), y(y), width(width), height(height) {
        textFile.open("output.txt", std::ios::out | std::ios::trunc);
        if (!textFile.is_open()) {
            throw std::runtime_error("Can't open file");
        }
    }
    ~TextInput() {
        if (textFile.is_open()) {
            textFile.close();
        }}

    bool contains(int px, int py) const {
        return px >= x && px <= x + width && py >= y && py <= y + height;
    }

    void draw(cairo_t* cr) const {
        cairo_set_source_rgb(cr, isFocused ? 0.8 : 0.9, 0.9, 0.9);
        cairo_rectangle(cr, x, y, width, height);
        cairo_fill(cr);

        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_rectangle(cr, x, y, width, height);
        cairo_stroke(cr);

        cairo_set_source_rgb(cr, 0, 0, 0);
        for (size_t i = 0; i < lines.size(); ++i) {
            cairo_move_to(cr, x + 10, y + (i + 1) * lineHeight - 5);
            cairo_show_text(cr, lines[i].c_str());
        }
    }

    void handleKeyPress(uint32_t keysym) {
        if (keysym == XKB_KEY_BackSpace) {
            if (!lines.empty() && !lines.back().empty()) {
                lines.back().pop_back();
            } else if (lines.size() > 1) {
                lines.pop_back();
            }
        } else if (keysym == XKB_KEY_Return) {
            lines.push_back("");
        } else {
            char buffer[8];
            int size = xkb_keysym_to_utf8(keysym, buffer, sizeof(buffer));
            if (size > 0) {
                buffer[size] = '\0';
                lines.back() += buffer;

                auto cr = cairo_create(cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 1, 1));
                cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
                cairo_set_font_size(cr, 14);
                cairo_text_extents_t extents;
                cairo_text_extents(cr, lines.back().c_str(), &extents);
                cairo_destroy(cr);

                if (extents.width > width - 20) {
                    std::string overflow = lines.back();
                    size_t cutPoint = lines.back().size() - 1;
                    while (cutPoint > 0) {
                        cairo_text_extents(cr, lines.back().substr(0, cutPoint).c_str(), &extents);
                        if (extents.width <= width - 20) {
                            break;
                        }
                        cutPoint--;
                    }
                    lines.back() = lines.back().substr(0, cutPoint);
                    lines.push_back(overflow.substr(cutPoint));
                }
            }
        }

        height = std::max(40, static_cast<int>(lines.size() * lineHeight));

        if (textFile.is_open()) {
            textFile.seekp(0, std::ios::beg);
            textFile.clear();
            for (const auto& line : lines) {
                textFile << line << std::endl;
            }
            textFile.flush();
        }
    }
    void setX(int x);
    void setY(int x);
    int getX() const;
    int getY()const;

};

#endif //GWAYTOOL_TEXT_FIELD_H
