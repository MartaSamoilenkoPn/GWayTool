#include "Application.h"
#include "Button.h"
#include "Window.h"

int main() {
    Application app;

    Window window(800, 600, app.getDisplay());
    Button button(50, 50, 200, 50, "Click Me");

    window.addControl(&button);
    window.show();

    return app.run();
}
