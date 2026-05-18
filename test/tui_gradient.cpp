#include "gameloop.hpp"

class Game : public GameLoop {

  void init() override {}

  void update(double dt) override {
    int h = height();
    int w = width();
    // manually assigned each pixel in the framebuffer
    for (int y = 0; y < h; y++) {
      for (int x = 0; x < w; x++) {
        // makes a gradient with red increasing left-to-right and green top-to-bottom
        fb.framebuffer.data()[y * w + x] =
            Pixel{static_cast<uint8_t>(x*255 / w),
                  static_cast<uint8_t>(y*255 / h), 0};
      }
    }

    // O(n^2)
  }

  void cleanup() override {}
};

int main() {
  Game game{};

  game.run();
}
