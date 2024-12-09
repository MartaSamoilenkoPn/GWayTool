#include "application.h"
#include <iostream>

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



int main() {
    try {
        WaylandApplication app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
