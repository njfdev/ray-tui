#include "gameloop.hpp"
#include "render/tui.hpp"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <csignal>
#include <utility>

std::atomic<bool> isInterrupted = false;
void signalHandler(int sig) { isInterrupted = true; }

GameLoop::GameLoop() {}

void GameLoop::run() {
  std::signal(SIGINT, signalHandler);

  Tui tui;

  tui.getRenderDimensions(&screen_width, &screen_height);

  // resize framebuffer
  fb.resize(screen_width, screen_height);

  tui.init();
  init();

  auto start = std::chrono::steady_clock::now();

  int last_height = screen_height;
  int last_width = screen_width;
  while (!isInterrupted) {
    auto t = std::chrono::steady_clock::now();

    // update terminal dimensions
    tui.getRenderDimensions(&screen_width, &screen_height);
    if (last_height != screen_height || last_width != screen_width) {
      fb.resize(screen_width, screen_height);
    }
    auto end = std::chrono::steady_clock::now();

    input.update();
    update(std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
               .count() /
           1000.0);
    start = end;

    if (!disable_render)
      tui.render(fb);
  }

  cleanup();
  tui.cleanup();
}
