#include <cstdlib>
#include <csignal>
#include <iostream>
#include <thread>
#include <atomic>

#include "tui.hpp"

std::atomic<bool> isInterrupted(false);

void signalHandler(int num) {
    isInterrupted = true;
}

int main(int argc, char *argv[])
{
    std::signal(SIGINT, signalHandler);

    Tui tui;
    while (!isInterrupted) {
        RenderSource src = RenderSource();
        tui.render(src);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    tui.cleanup();
}
