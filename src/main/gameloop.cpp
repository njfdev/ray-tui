#include "gameloop.hpp"
#include "render/tui.hpp"
#include <thread>

std::atomic<bool> isInterrupted = false;
void signalHandler(int sig) {
    isInterrupted = true;
}

GameLoop::GameLoop() {}

void GameLoop::run() {
    std::signal(SIGINT, signalHandler);

    Tui tui;

    tui.getRenderDimensions(&screen_width, &screen_height);

    // resize framebuffer
    fb.resize(screen_width*screen_height);

    init();

    while (!isInterrupted) {
        // update terminal dimensions
        tui.getRenderDimensions(&screen_width, &screen_height);
        fb.resize(screen_width*screen_height);
        update();
        tui.render(fb);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    cleanup();
    tui.cleanup();
}
