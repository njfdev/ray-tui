// AI generated with GPT-5.2-Codex (2026-05-08)

#include "gameloop.hpp"
#include "input/input.hpp"
#include <iostream>

class InputTester: public GameLoop {
  void init() override {}

  void update(double dt) override {
      input.update();
      // manually check WASD key presses and log
      std::cout << "W pressed? " << input.isKeyPressed(Key::W) << std::endl;
      std::cout << "A pressed? " << input.isKeyPressed(Key::A) << std::endl;
      std::cout << "S pressed? " << input.isKeyPressed(Key::S) << std::endl;
      std::cout << "D pressed? " << input.isKeyPressed(Key::D) << std::endl;
      // move cursor back up 4 lines to overwrite WASD logs the next update
      std::cout << "\033[4F\r";
  }

  void cleanup() override {}
};

int main() {
  InputTester tester{};
  // make it so logging is visible
  tester.disable_render = true;

  tester.run();
}
