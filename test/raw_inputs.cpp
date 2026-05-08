// AI generated with GPT-5.2-Codex (2026-05-08)

#include "gameloop.hpp"
#include "input/input.hpp"
#include <iostream>

class InputTester: public GameLoop {
    Input input;

  void init() override {
      input = Input();
  }

  void update(double_t dt) override {
      input.update();
      std::cout << "W pressed? " << input.isKeyPressed(KeyCode::W) << std::endl;
      std::cout << "A pressed? " << input.isKeyPressed(KeyCode::A) << std::endl;
      std::cout << "S pressed? " << input.isKeyPressed(KeyCode::S) << std::endl;
      std::cout << "D pressed? " << input.isKeyPressed(KeyCode::D) << std::endl;
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
