#pragma once

#include <array>
#include <chrono>
#include <cmath>
#include <utility>
#include "render/framebuffer.hpp"

using timestamp = std::chrono::time_point<std::chrono::steady_clock>;

class GameLoop {
public:
    GameLoop();

    void run();

    char cur_key = 0;
    std::array<std::pair<char, timestamp>, 5> keys_down;
    bool disable_render = false;

protected:
    Framebuffer fb{};

    int width() { return screen_width; }
    int height() { return screen_height; }

    virtual void init() = 0;
    virtual void update(double_t dt) = 0;
    virtual void cleanup() = 0;

private:
    int screen_width, screen_height;
};
