#include "Application.h"
#include "Window.h"
#include "Button.h"

int main() {
    Application app;

    Window window(800, 600);
    Button button(50, 50, 200, 50, "Click Me");

    button.setOnClick([]() {
        std::cout << "Button clicked!" << std::endl;
    });

    window.addControl(&button);
    window.show();

    return app.run();
}
