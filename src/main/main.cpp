#include "gameloop.hpp"
#include <csignal>
#include <cstdlib>

class Game : public GameLoop {

  void init() override {}

  void update(double_t dt) override {
    double w = width();
    double h = height();
    for (double y = 0; y < h; y++) {
      for (double x = 0; x < w; x++) {
        fb.framebuffer.data()[(int)y * width() + (int)x] =
            Pixel{static_cast<uint8_t>(x / w * 255.0),
                  static_cast<uint8_t>(y / h * 255.0), 0};
      }
    }
  }

  void cleanup() override {}
};

int main() {
  Game game{};

  game.run();
}
