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
    tui.pollInput(
        [&](int key) {
          char k = 0;
          switch (key) {
          case 119:
            k = 'w';
            break;
          case 115:
            k = 's';
            break;
          case 97:
            k = 'a';
            break;
          case 100:
            k = 'd';
            break;
          case 113:
            k = 'q';
            break;
          case 101:
            k = 'e';
            break;
          }
          if (k == 0)
            return 0;

          int oldest_i = 0;
          std::chrono::time_point<std::chrono::steady_clock> oldest_key = t;
          int empty = -1;

          for (int i = 0; i < keys_down.size(); i++) {
            if (keys_down[i].first == k) {
              keys_down[i].second = t;
              return 0;
            } else if (keys_down[i].first == 0) {
              empty = i;
            } else {
              auto dt = t - keys_down[i].second;
              if (dt > t - oldest_key) {
                oldest_key = keys_down[i].second;
                oldest_i = i;
              }
            }
          }

          auto nu = std::pair<char, timestamp>{k, t};

          if (empty != -1) {
            keys_down[empty] = nu;
          } else {
            keys_down[oldest_i] = nu;
          }

          return 0;
        },
        5);

    auto max =
        std::max_element(keys_down.begin(), keys_down.end(),
                         [](auto a, auto b) { return a.second < b.second; });

    if (t - max->second > std::chrono::milliseconds(250)) {
      cur_key = 0;
    } else {
      cur_key = max->first;
    }

    // update terminal dimensions
    tui.getRenderDimensions(&screen_width, &screen_height);
    if (last_height != screen_height || last_width != screen_width) {
      fb.resize(screen_width, screen_height);
    }
    auto end = std::chrono::steady_clock::now();
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
