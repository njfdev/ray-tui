#include "tui.hpp"

void Tui::render() {
    int width, height;
    getTerminalDimensions(&width, &height);

    // fill screen
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            std::cout << (x+y)%10;
        }
        std::cout << std::endl;
    }
}

void Tui::getTerminalDimensions(int* width, int* height) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    *width = w.ws_col;
    *height = w.ws_row;
}
