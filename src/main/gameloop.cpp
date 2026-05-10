#include "gameloop.hpp"
#include "render/tui.hpp"
#include <atomic>
#include <chrono>
#include <csignal>
#include <thread>

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
  double last_dt = 1.0/FRAMERATE;
  while (!isInterrupted) {
    // update terminal dimensions
    tui.getRenderDimensions(&screen_width, &screen_height);
    if (last_height != screen_height || last_width != screen_width) {
      fb.resize(screen_width, screen_height);
    }

    input.update();
    update(last_dt);

    if (!disable_render)
      tui.render(fb, screen_width, screen_height);

    // wait if there is time till next frame
    double frame_time = 1/FRAMERATE;
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start)
               .count() / 1000.0;
    double time_to_wait = frame_time-elapsed;
    if (time_to_wait > 0.0) {
        std::this_thread::sleep_for(std::chrono::milliseconds((int) (time_to_wait*1000.0)));
    }

    auto end = std::chrono::steady_clock::now();
    last_dt = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
               .count() /
           1000.0;
    start = end;
  }

  cleanup();
  tui.cleanup();
}
