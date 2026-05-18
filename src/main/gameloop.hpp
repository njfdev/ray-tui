#pragma once

#include <chrono>
#include "input/input.hpp"
#include "render/framebuffer.hpp"

using timestamp = std::chrono::time_point<std::chrono::steady_clock>;

/**
 * Handles the application lifecycle, with things like interrupt
 * handling, framebuffer rendering, and lifecycle function management.
 */
class GameLoop {
public:
    /**
     * Create a GameLoop object that handles the application lifecycle,
     * with things like interrupt handling, framebuffer rendering, and
     * lifecycle function management.
     *
     * NOTE: Only one GameLoop object constructor should ever be called.
     *
     * @return new GameLoop object
     */
    GameLoop();

    /**
     * Call this function once to handle the entire game loop lifecycle
     * and rendering. This function exits when the user ends the program.
     */
    void run();

    bool disable_render = false;

protected:
    // write to this framebuffer in update() to render to the terminal
    Framebuffer fb{};
    // use this to watch keyboard inputs
    Input input{};

    double FRAMERATE = 60.0;

    /**
     * Get the current effective render width of the terminal for this cycle
     *
     * @return effective render width
     */
    int width() { return screen_width; }
    /**
      * Get the current effective render height of the terminal for this cycle
      *
      * @return effective render height
      */
    int height() { return screen_height; }

    /**
     * Code implemented in init() will be run once at the start of
     * the game loop, after the TUI has been initialized.
     */
    virtual void init() = 0;
    /**
     * Code implemented in update() will attempt to run at
     * `FRAMERATE` times a second.
     *
     * To render to the terminal, update() should fill the
     * framebuffer (`fb`) with the desired pixels to render
     * that frame.
     *
     * @param dt time in seconds between the last and current frame
     */
    virtual void update(double dt) = 0;
    /**
      * Code implemented in cleanup() will be run once at the end of
      * the game loop, before the TUI is cleaned up.
     */
    virtual void cleanup() = 0;

private:
    int screen_width, screen_height;
};
