#pragma once

#include <sys/ioctl.h>
#include <unistd.h>

#include "render/framebuffer.hpp"


class Tui {
public:
    void render(Framebuffer &src);
    void cleanup();
    void getRenderDimensions(int* width, int* height);
    int getFrameBufferSize();

private:
    void getTerminalDimensions(int* width, int* height);
};
