#pragma once

#include <array>
#include <sys/ioctl.h>
#include <unistd.h>

class RenderSource {
public:
    virtual std::array<int, 3> color(int x, int y, int width, int height) {
        return {(int)((((double)x)/width)*255.0), (int)((((double)y)/height)*255), 0};
    }
};

class Tui {
public:
    void render(RenderSource &src);
    void cleanup();
    void getTerminalDimensions(int* width, int* height);
};
