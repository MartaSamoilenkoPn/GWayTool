#include <iostream>
#include <stdexcept>
#include <cstring>
#include <cstdlib>
#include <xdg-shell-client-protocol.h>
#include <linux/input-event-codes.h>
#include "application.h"
#include <xkbcommon/xkbcommon.h>

static int pointer_x = 0;
static int pointer_y = 0;
bool textInputAdded = false;
bool isDragging = false;
int dragOffsetX = 0;
int dragOffsetY = 0;
bool buttonAdded = true;

struct wl_seat* seat = nullptr;
struct wl_keyboard* keyboard = nullptr;
// -------------------- WaylandDisplay Implementation --------------------
const struct wl_registry_listener WaylandDisplay::registry_listener = {
    .global = WaylandDisplay::registryHandler,
    .global_remove = WaylandDisplay::registryRemoveHandler,
};

static void pointerEnterHandler(void* data, struct wl_pointer* pointer, uint32_t serial,
                                struct wl_surface* surface, wl_fixed_t x, wl_fixed_t y) {
    pointer_x = wl_fixed_to_int(x);
    pointer_y = wl_fixed_to_int(y);
    std::cout << "Pointer entered surface at: (" << pointer_x << ", " << pointer_y << ")\n";
}

static void pointerLeaveHandler(void* data, struct wl_pointer* pointer, uint32_t serial,
                                struct wl_surface* surface) {
    std::cout << "Pointer left the surface.\n";
}

static void pointerButtonHandler(void* data, struct wl_pointer* pointer, uint32_t serial,
                                 uint32_t time, uint32_t button, uint32_t state) {
    const char* state_str = (state == WL_POINTER_BUTTON_STATE_PRESSED) ? "pressed" : "released";
    auto* app = static_cast<WaylandApplication*>(data);

    if (state_str == "pressed" && button == BTN_LEFT) {
        app->onMouseClick(pointer_x, pointer_y);
    }   else if (state == WL_POINTER_BUTTON_STATE_RELEASED) {
            app->onMouseRelease(pointer_x, pointer_y);
//    }
}}


static void pointerAxisHandler(void* data, struct wl_pointer* pointer, uint32_t time,
                               uint32_t axis, wl_fixed_t value) {
    const char* axis_str = (axis == WL_POINTER_AXIS_VERTICAL_SCROLL) ? "vertical" : "horizontal";
    std::cout << "Scroll " << axis_str << " by " << wl_fixed_to_double(value) << "\n";
}


static void pointerMotionHandler(void* data, struct wl_pointer* pointer, uint32_t time,
                                 wl_fixed_t x, wl_fixed_t y) {
    pointer_x = wl_fixed_to_int(x);
    pointer_y = wl_fixed_to_int(y);

    std::cout << "[DEBUG] Pointer moved to (" << pointer_x << ", " << pointer_y << ")\n";

    WaylandApplication* app = static_cast<WaylandApplication*>(data);
    app->onMouseMove(pointer_x, pointer_y);
}


static void pointerFrameHandler(void* data, struct wl_pointer* pointer) {
//    std::cout << "Pointer frame event received.\n";
    std::cout << std::endl;
}


static const struct wl_pointer_listener pointer_listener = {
        .enter = pointerEnterHandler,
        .leave = pointerLeaveHandler,
        .motion = pointerMotionHandler,
        .button = pointerButtonHandler,
        .axis = pointerAxisHandler,
        .frame = pointerFrameHandler,
};

WaylandDisplay::WaylandDisplay() {
    display = wl_display_connect(NULL);
    if (!display) {
        throw std::runtime_error("Failed to connect to Wayland display");
    }
    std::cout << "Connected to Wayland display.\n";

    registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &WaylandDisplay::registry_listener, this);

    wl_display_dispatch(display);
    wl_display_roundtrip(display);

    if (!compositor || !xdg_wm_base) {
        throw std::runtime_error("Failed to initialize Wayland compositor or XDG shell");
    }
    std::cout << "Wayland compositor and XDG shell initialized.\n";
}

WaylandDisplay::~WaylandDisplay() {
    wl_display_disconnect(display);
    std::cout << "Disconnected from Wayland display.\n";
}

void WaylandDisplay::registryHandler(void* data, struct wl_registry* registry,
                                      uint32_t id, const char* interface, uint32_t version) {
    auto* self = static_cast<WaylandDisplay*>(data);

    if (strcmp(interface, "wl_compositor") == 0) {
        self->compositor = static_cast<wl_compositor*>(
            wl_registry_bind(registry, id, &wl_compositor_interface, 4));
    } else if (strcmp(interface, "xdg_wm_base") == 0) {
        self->xdg_wm_base = static_cast<struct xdg_wm_base*>(
                wl_registry_bind(registry, id, &xdg_wm_base_interface, 1));
    } else if (strcmp(interface, "wl_seat") == 0) {
        struct wl_seat* seat = static_cast<wl_seat*>(wl_registry_bind(registry, id, &wl_seat_interface, 1));
        struct wl_pointer* pointer = wl_seat_get_pointer(seat);

        if (seat) {
            keyboard = wl_seat_get_keyboard(seat);
            wl_keyboard_add_listener(keyboard, &WaylandApplication::keyboard_listener, self);
        }
        if (pointer) {
            wl_pointer_add_listener(pointer, &pointer_listener, self);
        } else {
            std::cerr << "Failed to get pointer for seat " << id << "\n";
        }
    }

}


void WaylandDisplay::registryRemoveHandler(void* data, struct wl_registry* registry, uint32_t id) {
    std::cout << "Global object removed: ID = " << id << "\n";
}

void WaylandDisplay::roundtrip() {
    wl_display_roundtrip(display);
}

// -------------------- WaylandSurface Implementation --------------------
const struct xdg_surface_listener WaylandSurface::xdg_surface_listener = {
    .configure = WaylandSurface::xdgSurfaceConfigureHandler,
};

const struct xdg_toplevel_listener WaylandSurface::xdg_toplevel_listener = {
    .configure = WaylandSurface::xdgToplevelConfigureHandler,
    .close = WaylandSurface::xdgToplevelCloseHandler,
};


void WaylandSurface::xdgSurfaceConfigureHandler(void* data, struct xdg_surface* xdg_surface, uint32_t serial) {
    xdg_surface_ack_configure(xdg_surface, serial);
}

void WaylandSurface::xdgToplevelConfigureHandler(void* data, struct xdg_toplevel* toplevel,
                                                 int32_t width, int32_t height, struct wl_array* states) {
    std::cout << "Resize event: " << width << "x" << height << "\n";
}

WaylandSurface::WaylandSurface(WaylandDisplay& display) {
    surface = wl_compositor_create_surface(display.getCompositor());
    if (!surface) {
        throw std::runtime_error("Failed to create Wayland surface");
    }

    xdg_surface = xdg_wm_base_get_xdg_surface(display.getXdgWmBase(), surface);
    xdg_surface_add_listener(xdg_surface, &xdg_surface_listener, this);

    xdg_toplevel = xdg_surface_get_toplevel(xdg_surface);
    xdg_toplevel_add_listener(xdg_toplevel, &xdg_toplevel_listener, this);

    wl_surface_commit(surface);
    display.roundtrip();

    std::cout << "Wayland surface and XDG shell setup complete.\n";
}

WaylandSurface::~WaylandSurface() {
    xdg_toplevel_destroy(xdg_toplevel);
    xdg_surface_destroy(xdg_surface);
    wl_surface_destroy(surface);
}



void WaylandSurface::xdgToplevelCloseHandler(void* data, struct xdg_toplevel* toplevel) {
    std::cerr << "Window close event received.\n";
}

// -------------------- EGLContext Implementation --------------------

MyEGLContext::MyEGLContext(struct wl_display* display) {
    initialize(display);
}

MyEGLContext::~MyEGLContext() {
    eglTerminate(egl_display);
    std::cout << "EGL terminated.\n";
}

void MyEGLContext::initialize(struct wl_display* display) {
    EGLint major, minor, count, n;
    EGLConfig* configs;
    EGLint size;

    EGLint config_attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };

    static const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    // Get the EGL display
    egl_display = eglGetDisplay((EGLNativeDisplayType)display);
    if (egl_display == EGL_NO_DISPLAY) {
        throw std::runtime_error("Failed to get EGL display");
    }
    std::cout << "Created EGL display.\n";

    // Initialize EGL
    if (eglInitialize(egl_display, &major, &minor) != EGL_TRUE) {
        throw std::runtime_error("Failed to initialize EGL");
    }
    std::cout << "EGL initialized: version " << major << "." << minor << "\n";

    // Bind the EGL API
    if (!eglBindAPI(EGL_OPENGL_API)) {
        throw std::runtime_error("Failed to bind EGL API");
    }

    // Get the EGL configurations
    eglGetConfigs(egl_display, NULL, 0, &count);
    std::cout << "EGL has " << count << " configurations.\n";

    configs = static_cast<EGLConfig*>(calloc(count, sizeof(EGLConfig)));
    if (!configs) {
        throw std::runtime_error("Failed to allocate memory for EGL configurations");
    }

    eglChooseConfig(egl_display, config_attribs, configs, count, &n);

    for (int i = 0; i < n; i++) {
        eglGetConfigAttrib(egl_display, configs[i], EGL_BUFFER_SIZE, &size);
        std::cout << "Buffer size for config " << i << " is " << size << "\n";

        eglGetConfigAttrib(egl_display, configs[i], EGL_RED_SIZE, &size);
        std::cout << "Red size for config " << i << " is " << size << "\n";

        // Use the first configuration
        egl_conf = configs[i];
        break;
    }

    free(configs);

    // Create the EGL context
    egl_context = eglCreateContext(egl_display, egl_conf, EGL_NO_CONTEXT, context_attribs);
    if (egl_context == EGL_NO_CONTEXT) {
        throw std::runtime_error("Failed to create EGL context");
    }
    std::cout << "EGL context created successfully.\n";
}

EGLSurface MyEGLContext::createWindowSurface(struct wl_egl_window* egl_window) {
    EGLSurface surface = eglCreateWindowSurface(egl_display, egl_conf, egl_window, NULL);
    if (surface == EGL_NO_SURFACE) {
        throw std::runtime_error("Failed to create EGL window surface");
    }
    std::cout << "EGL window surface - Created" <<  "\n";
    return surface;
}

// -------------------- CairoRenderer Implementation --------------------

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


void CairoRenderer::drawText(const std::string& text, int x, int y, double r, double g, double b, int size) {
    cairo_t* cr = cairo_create(cairo_surface);

//    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
//    cairo_rectangle(cr, x - 10, y - 30, 150, 40);
//    cairo_fill(cr);

    cairo_set_source_rgb(cr, r, g, b);
    cairo_select_font_face(cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, size);
    cairo_move_to(cr, x, y);
    cairo_show_text(cr, text.c_str());

//    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
//    cairo_rectangle(cr, x - 10, y - 30, 150, 30);
//    cairo_stroke(cr);

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
    for (const Button& button : buttons) {
        if (button.contains(x, y)) {
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



// -------------------- WaylandApplication Implementation --------------------
void CairoRenderer::drawTextInput(const TextInput& textInput) {
    textInputAdded = true;
    cairo_t* cr = cairo_create(cairo_surface);
    textInput.draw(cr);
    cairo_gl_surface_swapbuffers(cairo_surface);
    cairo_destroy(cr);
}

WaylandApplication::WaylandApplication()
    : display(), surface(display), egl(display.getDisplay()),
      egl_window(wl_egl_window_create(surface.getSurface(), 720, 510)),
      egl_surface(egl.createWindowSurface(egl_window)),
      renderer(egl, egl_surface) {
    if (!egl_window) {
        throw std::runtime_error("Failed to create Wayland EGL window");
    }
    std::cout << "WaylandApplication initialized successfully.\n";
}

const struct wl_keyboard_listener WaylandApplication::keyboard_listener = {
        .keymap = [](void* data, struct wl_keyboard* keyboard, uint32_t format, int fd, uint32_t size) {
        },
        .enter = [](void* data, struct wl_keyboard* keyboard, uint32_t serial, struct wl_surface* surface, struct wl_array* keys) {
        },
        .leave = [](void* data, struct wl_keyboard* keyboard, uint32_t serial, struct wl_surface* surface) {
        },
        .key = WaylandApplication::keyboardKeyHandler,
        .modifiers = [](void* data, struct wl_keyboard* keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group) {
        },
        .repeat_info = [](void* data, struct wl_keyboard* keyboard, int32_t rate, int32_t delay) {
        }
};

void TextInput::setX(int x) {
    this->x = x;
}

void TextInput::setY(int y) {
    this->y = y;
}

int TextInput::getX() const {
    return x;
}

int TextInput::getY() const {
    return y;
}

void WaylandApplication::onMouseRelease(int x, int y) {
    isDragging = false;
}

void WaylandApplication::onMouseMove(int x, int y) {
    if (isDragging) {
        std::cout << "Dragging started. Mouse position: (" << x << ", " << y << ")\n";

        int oldX = textInput.getX();
        int oldY = textInput.getY();
        std::cout << "Old position: (" << oldX << ", " << oldY << ")\n";

        textInput.setX(x - dragOffsetX);
        textInput.setY(y - dragOffsetY);

        std::cout << "Clearing area: (" << oldX << ", " << oldY << ")\n";
//        renderer.clearArea(oldX, oldY, textInput.width, textInput.height);
        renderer.clearArea(oldX - 4, oldY - 4, textInput.width + 8, textInput.height + 8);


        std::cout << "Drawing new position: (" << textInput.getX() << ", " << textInput.getY() << ")\n";
        renderer.drawTextInput(textInput);
    }
}

void WaylandApplication::onMouseClick(int x, int y) {
    if (textInputAdded) {
        if (textInput.contains(x, y)) {
            textInput.isFocused = true;
            std::cout << "TextInput focused.\n";

            if (!isDragging) {
                isDragging = true;
                std::cout << "Dragging initialized.\n";
                dragOffsetX = x - textInput.getX();
                dragOffsetY = y - textInput.getY();
            }
        } else {
            textInput.isFocused = false;
        }
        renderer.drawTextInput(textInput);
    }
    if(buttonAdded) {
        renderer.handleClick(x, y);
    }
}


void WaylandApplication::keyboardKeyHandler(void* data, struct wl_keyboard* keyboard,
                                            uint32_t serial, uint32_t time, uint32_t key,
                                            uint32_t state) {
    auto* app = static_cast<WaylandApplication*>(data);

    std::cout << "Keyboard event: key=" << key
              << ", state=" << (state == WL_KEYBOARD_KEY_STATE_PRESSED ? "PRESSED" : "RELEASED")
              << ", time=" << time << std::endl;

    if (state == WL_KEYBOARD_KEY_STATE_PRESSED) {
        struct xkb_context* ctx = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
        if (!ctx) {
            std::cerr << "Failed to create XKB context." << std::endl;
            return;
        }

        struct xkb_keymap* keymap = xkb_keymap_new_from_names(ctx, nullptr, XKB_KEYMAP_COMPILE_NO_FLAGS);
        if (!keymap) {
            std::cerr << "Failed to create XKB keymap." << std::endl;
            xkb_context_unref(ctx);
            return;
        }

        struct xkb_state* xkb_state = xkb_state_new(keymap);
        if (!xkb_state) {
            std::cerr << "Failed to create XKB state." << std::endl;
            xkb_keymap_unref(keymap);
            xkb_context_unref(ctx);
            return;
        }

        uint32_t keysym = xkb_state_key_get_one_sym(xkb_state, key + 8); // Додаємо 8 для коректного keycode
        char buffer[64];
        int size = xkb_keysym_to_utf8(keysym, buffer, sizeof(buffer));
        buffer[size] = '\0';

        std::cout << "Key pressed: keysym=" << keysym
                  << ", utf8='" << buffer << "'"
                  << ", keycode=" << key << std::endl;
//        app->textInput.isFocused = true;
        if (app->textInput.isFocused) {
            if (keysym == XKB_KEY_Escape) {
                std::cout << "Escape key detected. Unfocusing text input." << std::endl;
                app->textInput.isFocused = false;
            } else {
                app->textInput.handleKeyPress(keysym);
                app->renderer.drawTextInput(app->textInput);
            }
        }

        xkb_state_unref(xkb_state);
        xkb_keymap_unref(keymap);
        xkb_context_unref(ctx);
    }
}

WaylandApplication::~WaylandApplication() {
    wl_egl_window_destroy(egl_window);
    std::cout << "WaylandApplication resources cleaned up.\n";
}

//-----testing buttons----
void sayHelloWorld(){
    // just for testing
    std::cout << "hello world" << std::endl;
}

std::function<void()> createTextCallback(CairoRenderer &renderer, bool &isVisible) {
    return [&renderer, &isVisible]() {
        if (isVisible) {
            renderer.clearArea(50, 200, 400, 100);
            isVisible = false;
        } else {
            renderer.drawText("Hello, Wayland!", 100, 250, 1.0, 1.0, 1.0, 16);
            isVisible = true;
        }
    };
}

std::function<void()> showGeneralText(CairoRenderer &renderer, bool &isVisible) {
    return [&renderer, &isVisible]() {
        if (isVisible) {
            renderer.clearArea(20, 250, 130, 200);
            isVisible = false;
        } else {
            renderer.drawText("Here will be some text....", 20, 300, 1.0, 1.0, 1.0, 10);
            isVisible = true;
        }
    };
}

std::vector<std::vector<std::string>> parseCSVData(std::ifstream &file) {
    std::string line;
    std::vector<std::vector<std::string>> csvData;

    while (std::getline(file, line)) {
        std::stringstream lineStream(line);
        std::string cell;
        std::vector<std::string> row;

        while (std::getline(lineStream, cell, ',')) {
            cell.erase(cell.begin(), std::find_if(cell.begin(), cell.end(), [](unsigned char ch) {
                return !std::isspace(ch);
            }));
            cell.erase(std::find_if(cell.rbegin(), cell.rend(), [](unsigned char ch) {
                return !std::isspace(ch);
            }).base(), cell.end());

            row.push_back(cell);
        }

        csvData.push_back(row);
    }

    file.close();

    return csvData;
}


std::function<void()> parseFile(CairoRenderer &renderer, TextInput &textInput,
                                std::vector<std::vector<std::string>> &csvData) {

    return [&renderer, &textInput, &csvData]() {
        std::string filePath = textInput.getInputText();

        std::ifstream file(filePath);
        if (!file.is_open()) {
            renderer.drawText("Failed to open file", 20, 300, 1.0, 0.0, 0.0, 10);
            return;
        }

        csvData = parseCSVData(file);
        renderer.drawText("Finished processing", 230, 83, 0.0, 1.0, 0.0, 10);

    };
}

std::function<void()> showTable(CairoRenderer &renderer, int x, int y, std::vector<std::vector<std::string>> &csvData) {
    return [&renderer, x, y, &csvData] {
        int rows = csvData.size();
        int cols = rows > 0 ? csvData[0].size() : 0;

        if (rows == 0 || cols == 0) {
            renderer.drawText("No data in file", 20, 300, 1.0, 0.0, 0.0, 10);
            return;
        }

        int cellWidth = 100;
        int cellHeight = 20;
        double textR = 1.0, textG = 1.0, textB = 1.0;
        double lineR = 0.5, lineG = 0.5, lineB = 0.5;

        renderer.drawTable(csvData, x, y, cellWidth, cellHeight, rows, cols, textR, textG, textB, lineR, lineG, lineB);
    };
}


std::function<void()> showBarChart(CairoRenderer &renderer,
                                   const std::vector<int> &values,
                                   const std::vector<std::string> &labels,
                                   std::optional<std::string> &title) {
    return [&renderer, values, labels, title]() {
        renderer.clearArea(270, 300, 400, 400);
        renderer.drawBarChart(values, 320, 340, 300, 130, 0.2, 0.6, 0.8, labels, title);
    };
}

std::function<void()> showLineChart(CairoRenderer &renderer,
                                   const std::vector<int> &values_x,
                                    const std::vector<int> &values_y,
                                   const std::vector<std::string> &labels,
                                    std::optional<std::string> &title) {
    return [&renderer, values_x, values_y, labels, title]() {
        renderer.clearArea(270, 300, 400, 400);
        renderer.drawLineChart(values_x, values_y, 320, 340, 300, 130, 0.0, 1.0, 0.0, title);
    };
}


std::function<void()> showPieChart(CairoRenderer &renderer,
                                    const std::vector<int> &values,
                                    const std::vector<std::tuple<double, double, double>> colors,
                                    const std::vector<std::string> &labels,
                                    std::optional<std::string> &title) {
    return [&renderer, values, colors, labels, title]() {
        renderer.clearArea(270, 300, 400, 400);
        renderer.drawPieChart(values, 450, 400, 60, colors, labels, title);
    };
}



//------app
void WaylandApplication::run() {
    std::cout << "Application running...\n";
    bool isTextVisible = false;

    renderer.drawText("Program Sunny", 20, 40, 1.0, 1.0, 1.0, 20);
    renderer.drawImage("./../sun.png", 30, 50, 0.25, 0.25);
    renderer.drawLine(200, 1, 200, 700, 1.0, 1.0, 0.0, 1.2);
    renderer.drawLine(200, 100, 1000, 100, 1.0, 1.0, 0.0, 1.2);

    renderer.drawText("draw a table from the csv", 230, 130, 1.0, 1.0, 1.0, 15);

    Button button1(20, 200, 100, 30, "Show/Hide text", showGeneralText(renderer, isTextVisible));

    std::vector<std::vector<std::string>> csvData;
    Button button2(430, 115, 70, 20, "Parse CSV", parseFile(renderer, textInput, csvData));
    Button button3(520, 115, 100, 20, "Show table", showTable(renderer, 220, 150, csvData));

    renderer.drawLine(200, 250, 1000, 250, 1.0, 1.0, 0.0, 1.2);

    std::vector<int> values_bar_chart = {50, 100, 75, 150, 200};
    std::vector<std::string> labels_bar_chart = {"val1", "val2", "val3", "val4", "val5"};
    std::optional<std::string> title_bar = "Testing bar chart";

    std::vector<int> values_x = {10, 30, 20, 50, 40};
    std::vector<int> values_y = {10, 30, 20, 50, 40};
    std::optional<std::string> title_line = "Testing Line Chart";

    std::vector<int> values_pie = {10, 20, 30, 40};
    std::vector<std::tuple<double, double, double>> colors = {
            {1.0, 0.0, 0.0},
            {0.0, 1.0, 0.0},
            {0.0, 0.0, 1.0},
            {1.0, 1.0, 0.0}
    };

    std::vector<std::string> labels_pie = {"one", "two", "three", "four"};
    std::optional<std::string> title_pie = "Testing Pie Chart";


    Button button4(230, 270, 100, 20, "Show bar chart",
                   showBarChart(renderer, values_bar_chart, labels_bar_chart, title_bar));

    Button button5(350, 270, 100, 20, "Show line chart",
                   showLineChart(renderer, values_x, values_y, labels_bar_chart, title_line));

    Button button6(470, 270, 100, 20, "Show pie chart",
                   showPieChart(renderer, values_pie, colors, labels_pie, title_pie));



    renderer.addButton(button1);
    renderer.addButton(button2);
    renderer.addButton(button3);
    renderer.addButton(button4);
    renderer.addButton(button5);
    renderer.addButton(button6);


    renderer.drawButton();

    renderer.drawTextInput(textInput);


    while (wl_display_dispatch(display.getDisplay()) != -1) {
        // Main event loop
    }
}

