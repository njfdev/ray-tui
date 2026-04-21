#include "tui.hpp"
#include <array>
#include <iostream>

std::array<int, 3> calcColor(int x, int y, int width, int height) {
    return {(int)((((double)x)/width)*255.0), (int)((((double)y)/height)*255), 0};
}

void Tui::render() {
    int width, height;
    getTerminalDimensions(&width, &height);

    // hide cursor
    std::cout << "\033[?25l";

    // move to 0,0
    std::cout << "\033[H";

    // fill screen
    for (int r = 0; r < height; r++) {
        int y = r*2;
        for (int c = 0; c < width; c++) {
            std::array<int, 3> topColor = calcColor(c, y, width, height*2);
            std::array<int, 3> bottomColor = calcColor(c, y+1, width, height*2);

            std::cout << "\033[38;2;" << topColor[0] << ";" << topColor[1] << ";" << topColor[2] << "m";
            std::cout << "\033[48;2;" << bottomColor[0] << ";" << bottomColor[1] << ";" << bottomColor[2] << "m▀";
        }

        if (r+1 != height) {
            std::cout << std::endl;
        } else {
            std::cout << "\x1b[0m";
        }
    }
}

void Tui::getTerminalDimensions(int* width, int* height) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    *width = w.ws_col;
    *height = w.ws_row;
}
