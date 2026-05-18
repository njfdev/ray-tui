#include "gameloop.hpp"
#include "render/tui.hpp"
#include <atomic>
#include <chrono>
#include <csignal>
#include <thread>

using namespace std::chrono;

// handler to check interrupts (e.g., CTRL-C)
std::atomic<bool> isInterrupted = false;
void signalHandler(int sig) { isInterrupted = true; }

GameLoop::GameLoop() {}

void GameLoop::run() {
  std::signal(SIGINT, signalHandler);

  // create and init TUI renderer and gameloop
  Tui tui;
  tui.getRenderDimensions(&screen_width, &screen_height);
  fb.resize(screen_width, screen_height);

  tui.init();
  init();

  auto start = steady_clock::now();

  // keep track of previous TUI/gameloop state
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

    // wait if there is time till the next frame
    double frame_time = 1/FRAMERATE;
    double elapsed = duration_cast<milliseconds>(steady_clock::now() - start).count() / 1000.0;
    double time_to_wait = frame_time-elapsed;
    if (time_to_wait > 0.0) {
        std::this_thread::sleep_for(milliseconds((int) (time_to_wait*1000.0)));
    }

    // calculate the delta time
    auto end = steady_clock::now();
    last_dt = duration_cast<milliseconds>(end - start)
               .count() /
           1000.0;
    start = end;
  }

  cleanup();
  tui.cleanup();
}
