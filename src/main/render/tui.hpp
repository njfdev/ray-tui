#pragma once

#include "render/framebuffer.hpp"

class Tui {
public:
    void render(Framebuffer &src);
    void cleanup();
    void getRenderDimensions(int* width, int* height);

private:
    void getTerminalDimensions(int* width, int* height);
    bool detectTmux();
    void beginSync(bool tmux);
    void endSync(bool tmux);
};
