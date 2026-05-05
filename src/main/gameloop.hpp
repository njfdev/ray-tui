#pragma once

#include <csignal>
#include "render/framebuffer.hpp"

class GameLoop {
public:
    GameLoop();

    void run();

protected:
    Framebuffer fb{};

    int width() { return screen_width; }
    int height() { return screen_height; }

    virtual void init() = 0;
    virtual void update() = 0;
    virtual void cleanup() = 0;

private:
    int screen_width, screen_height;
};
