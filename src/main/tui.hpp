#pragma once

#include <sys/ioctl.h>
#include <unistd.h>

class Tui {
public:
    void render();
    void cleanup();

private:
    void getTerminalDimensions(int* width, int* height);
};
