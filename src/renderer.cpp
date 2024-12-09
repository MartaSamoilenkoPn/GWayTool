#include "renderer.h"

CairoRenderer::CairoRenderer(MyEGLContext& egl, EGLSurface egl_surface) {

    cairo_device = cairo_egl_device_create(egl.getEGLDisplay(), egl.getEGLContext());
    if (!cairo_device || cairo_device_status(cairo_device) != CAIRO_STATUS_SUCCESS) {
        cairo_status_t status = cairo_device_status(cairo_device);
        std::cerr << "Cairo device error: " << cairo_status_to_string(status) << "\n";
        throw std::runtime_error("Failed to create Cairo EGL device");
    }

    cairo_surface = cairo_gl_surface_create_for_egl(cairo_device, egl_surface, 720, 510);
    if (!cairo_surface) {
        throw std::runtime_error("Failed to create Cairo surface");
    }
}

CairoRenderer::~CairoRenderer() {
    cairo_surface_destroy(cairo_surface);
    cairo_device_destroy(cairo_device);
    std::cout << "Cairo resources released.\n";
}

void CairoRenderer::clearArea(int x, int y, int width, int height) {
    cairo_t* cr = cairo_create(cairo_surface);

    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);

    cairo_rectangle(cr, x, y, width, height);
    cairo_fill(cr);

    cairo_destroy(cr);
    cairo_gl_surface_swapbuffers(cairo_surface);
}

void CairoRenderer::drawTextInput(TextInput& textInput) {
    textInput.textInputAdded = true;
    cairo_t* cr = cairo_create(cairo_surface);
    textInput.draw(cr);
    cairo_gl_surface_swapbuffers(cairo_surface);
    cairo_destroy(cr);
}


void CairoRenderer::drawText(const std::string& text, int x, int y, double r, double g, double b) {
    cairo_t* cr = cairo_create(cairo_surface);

    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_rectangle(cr, x - 10, y - 30, 300, 40);
    cairo_fill(cr);

    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_select_font_face(cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 20);
    cairo_move_to(cr, x, y);
    cairo_show_text(cr, text.c_str());

    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_rectangle(cr, x - 10, y - 30, 300, 40);
    cairo_stroke(cr);

    cairo_gl_surface_swapbuffers(cairo_surface);
    cairo_destroy(cr);
}

void CairoRenderer::drawImage(const std::string& imagePath, int x, int y, double scaleX, double scaleY) {
    cairo_surface_t* image_surface = cairo_image_surface_create_from_png(imagePath.c_str());

    if (cairo_surface_status(image_surface) != CAIRO_STATUS_SUCCESS) {
        std::cerr << "Check the path to the image and check if it is a .png format" << std::endl;
        return;
    }

    cairo_t* cr = cairo_create(cairo_surface);

    cairo_save(cr);
    cairo_translate(cr, x, y);
    cairo_scale(cr, scaleX, scaleY);

    cairo_set_source_surface(cr, image_surface, 0, 0);
    cairo_paint(cr);

    cairo_restore(cr);

    cairo_gl_surface_swapbuffers(cairo_surface);
    cairo_destroy(cr);
    cairo_surface_destroy(image_surface);
}

void CairoRenderer::handleClick(int x, int y) {
        // just debugging
//    std::cout << "Number of buttons: " << buttons.size() << std::endl;
//    std::cout << "in handle click" << std::endl;

    for (const Button& button : buttons) {
//        std::cout << "it buttons" << std::endl;
        if (button.contains(x, y)) {
//            std::cout << "button contains" << std::endl;
            if (button.onClick) {
                button.onClick();
            }
            break;
        }
    }
}

void CairoRenderer::drawButton() {
    cairo_t* cr = cairo_create(cairo_surface);

    for (const Button& button : buttons) {
        button.draw(cr);
    }

    cairo_gl_surface_swapbuffers(cairo_surface);
    cairo_destroy(cr);

}
