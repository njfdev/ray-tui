#include "gameloop.hpp"

class Game : public GameLoop {

  void init() override {}

  void update(double_t dt) override {
    int h = height();
    int w = width();
    for (int y = 0; y < h; y++) {
      for (int x = 0; x < w; x++) {
        fb.framebuffer.data()[y * w + x] =
            Pixel{static_cast<uint8_t>(x*255 / w),
                  static_cast<uint8_t>(y*255 / h), 0};
      }
    }
  }

  void cleanup() override {}
};

int main() {
  Game game{};

  game.run();
}
