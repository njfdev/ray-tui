#pragma once

#include <chrono>
#include <cmath>
#include "input/input.hpp"
#include "render/framebuffer.hpp"

using timestamp = std::chrono::time_point<std::chrono::steady_clock>;

class GameLoop {
public:
    GameLoop();

    void run();

    bool disable_render = false;

protected:
    Framebuffer fb{};
    Input input{};

    double FRAMERATE = 60.0;

    int width() { return screen_width; }
    int height() { return screen_height; }

    virtual void init() = 0;
    virtual void update(double_t dt) = 0;
    virtual void cleanup() = 0;

private:
    int screen_width, screen_height;
};
