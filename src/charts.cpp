#include "application.h"

void CairoRenderer::drawBarChart(const std::vector<int>& values, int x, int y, int width, int height,
                                 double r, double g, double b,
                                 const std::optional<std::vector<std::string>> &optionalLabels,
                                 const std::optional<std::string> &title) {
    if (values.empty()) return;
    if (optionalLabels && !optionalLabels->empty() && values.size() != optionalLabels->size()) return;

    cairo_t* cr = cairo_create(cairo_surface);

    int bar_count = values.size();
    int spacing = 10;
    int bar_width = (width - spacing * (bar_count - 1)) / bar_count;

    int max_value = *std::max_element(values.begin(), values.end());
    if (max_value == 0) max_value = 1;

    cairo_set_source_rgb(cr, r, g, b);

    for (int i = 0; i < bar_count; ++i) {
        double bar_height = (static_cast<double>(values[i]) / max_value) * height;

        int bar_x = x + i * (bar_width + spacing);
        int bar_y = y + (height - bar_height);
        cairo_rectangle(cr, bar_x, bar_y, bar_width, bar_height);
        cairo_fill(cr);
    }

    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_set_line_width(cr, 2.0);

    cairo_move_to(cr, x, y + height);
    cairo_line_to(cr, x + width, y + height);
    cairo_stroke(cr);

    cairo_move_to(cr, x, y);
    cairo_line_to(cr, x, y + height);
    cairo_stroke(cr);

    int y_ticks = 5;
    for (int i = 0; i <= y_ticks; ++i) {
        int tick_y = y + height - (height * i / y_ticks);
        cairo_move_to(cr, x - 5, tick_y);
        cairo_line_to(cr, x + 5, tick_y);
        cairo_stroke(cr);

        std::string label = std::to_string(i * max_value / y_ticks);
        cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
        cairo_move_to(cr, x - 30, tick_y + 5);
        cairo_show_text(cr, label.c_str());
    }

    for (int i = 0; i < bar_count; ++i) {
        int tick_x = x + i * (bar_width + spacing) + bar_width / 2;
        cairo_move_to(cr, tick_x, y + height + 5);
        cairo_line_to(cr, tick_x, y + height - 5);
        cairo_stroke(cr);

        std::string label;
        if (optionalLabels && !optionalLabels->empty()) {
            label = (*optionalLabels)[i];
        } else {
            label = std::to_string(i + 1);
        }

        cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
        cairo_move_to(cr, tick_x - 5, y + height + 20);
        cairo_show_text(cr, label.c_str());
    }

    if (title && !title->empty()) {
        cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
        cairo_select_font_face(cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, 16);

        cairo_text_extents_t extents;
        cairo_text_extents(cr, title->c_str(), &extents);

        double title_x = x + (width / 2.0) - (extents.width / 2.0);
        double title_y = y - 20;

        cairo_move_to(cr, title_x, title_y);
        cairo_show_text(cr, title->c_str());
    }

    cairo_gl_surface_swapbuffers(cairo_surface);
    cairo_destroy(cr);
}


void CairoRenderer::drawLineChart(const std::vector<int>& x_values, const std::vector<int>& y_values, int x, int y, int width, int height,
                                  double r, double g, double b,
                                  const std::optional<std::string> &title) {
    if (x_values.empty() || y_values.empty() || x_values.size() != y_values.size()) return;

    cairo_t* cr = cairo_create(cairo_surface);

    double max_value = *std::max_element(y_values.begin(), y_values.end());
    if (max_value == 0) max_value = 1;

    int point_count = x_values.size();
    int spacing = width / (point_count - 1);

    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_set_line_width(cr, 2.0);
    cairo_move_to(cr, x, y + height);
    cairo_line_to(cr, x + width, y + height);
    cairo_move_to(cr, x, y);
    cairo_line_to(cr, x, y + height);
    cairo_stroke(cr);

    cairo_set_source_rgb(cr, r, g, b);
    cairo_set_line_width(cr, 2.0);

    for (int i = 0; i < point_count; ++i) {
        double value_height = (y_values[i] / max_value) * height;
        double point_x = x + i * spacing;
        double point_y = y + height - value_height;

        if (i == 0) {
            cairo_move_to(cr, point_x, point_y);
        } else {
            cairo_line_to(cr, point_x, point_y);
        }
    }
    cairo_stroke(cr);

    for (int i = 0; i < point_count; ++i) {
        double value_height = (y_values[i] / max_value) * height;
        double point_x = x + i * spacing;
        double point_y = y + height - value_height;

        cairo_arc(cr, point_x, point_y, 3, 0, 2 * M_PI);
        cairo_fill(cr);

        cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
        cairo_move_to(cr, point_x, y + height);
        cairo_line_to(cr, point_x, y + height + 5);
        cairo_stroke(cr);

        std::string label = std::to_string(x_values[i]);
        cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
        cairo_select_font_face(cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_font_size(cr, 12);

        cairo_text_extents_t extents;
        cairo_text_extents(cr, label.c_str(), &extents);
        double text_x = point_x - extents.width / 2.0;
        double text_y = y + height + 20;

        cairo_move_to(cr, text_x, text_y);
        cairo_show_text(cr, label.c_str());

        cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
        cairo_move_to(cr, x - 5, point_y);
        cairo_line_to(cr, x + 5, point_y);
        cairo_stroke(cr);

        std::string y_label = std::to_string(y_values[i]);
        cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
        cairo_select_font_face(cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_font_size(cr, 12);

        cairo_text_extents(cr, y_label.c_str(), &extents);
        double y_label_x = x - 10 - extents.width;
        double y_label_y = point_y + extents.height / 2.0;

        cairo_move_to(cr, y_label_x, y_label_y);
        cairo_show_text(cr, y_label.c_str());
    }


    if (title && !title->empty()) {
        cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
        cairo_select_font_face(cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, 16);

        cairo_text_extents_t extents;
        cairo_text_extents(cr, title->c_str(), &extents);

        double title_x = x + (width / 2.0) - (extents.width / 2.0);
        double title_y = y - 20;

        cairo_move_to(cr, title_x, title_y);
        cairo_show_text(cr, title->c_str());
    }

    cairo_gl_surface_swapbuffers(cairo_surface);
    cairo_destroy(cr);
}
