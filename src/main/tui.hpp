#pragma once

#include <iostream>
#include <sys/ioctl.h>
#include <sys/ttycom.h>
#include <unistd.h>

class Tui {
public:
    void render();

private:
    void getTerminalDimensions(int* width, int* height);
};
