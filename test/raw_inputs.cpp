// AI generated with GPT-5.2-Codex (2026-05-08)

#include "gameloop.hpp"
#include "input/input.hpp"
#include <iostream>

class InputTester: public GameLoop {
  void init() override {}

  void update(double_t dt) override {
      input.update();
      std::cout << "W pressed? " << input.isKeyPressed(Key::W) << std::endl;
      std::cout << "A pressed? " << input.isKeyPressed(Key::A) << std::endl;
      std::cout << "S pressed? " << input.isKeyPressed(Key::S) << std::endl;
      std::cout << "D pressed? " << input.isKeyPressed(Key::D) << std::endl;
      std::cout << "\033[4F\r";
      (void)dt;
  }

  void cleanup() override {}
};

int main() {
  InputTester tester{};
  tester.disable_render = true;

  tester.run();
}
