#include "render/tui.hpp"
#include <array>
#include <iostream>

void Tui::render(Framebuffer &fb) {
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
            std::array<int, 3> topColor = fb.color(c, y, width, height*2);
            std::array<int, 3> bottomColor = fb.color(c, y+1, width, height*2);

            std::cout << "\033[38;2;" << topColor[0] << ";" << topColor[1] << ";" << topColor[2] << "m";
            std::cout << "\033[48;2;" << bottomColor[0] << ";" << bottomColor[1] << ";" << bottomColor[2] << "m▀";
        }

        if (r+1 != height) {
            std::cout << std::endl;
        } else {
            // make sure terminal buffer is immediately written
            std::cout << std::flush;
        }
    }
}

void Tui::cleanup() {
    // reshow cursor
    std::cout << "\033[?25h";
    // clear any custom highlighting
    std::cout << "\x1b[0m";
}

void Tui::getTerminalDimensions(int* width, int* height) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    *width = w.ws_col;
    *height = w.ws_row;
}

void Tui::getRenderDimensions(int* width, int* height) {
    getTerminalDimensions(width, height);
    // because terminal characters are split vertically into half blocks
    *height *= 2;
}

int Tui::getFrameBufferSize() {
    int w, h;
    getRenderDimensions(&w, &h);
    return w*h;
}
