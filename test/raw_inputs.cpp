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
      input.updateKeyEvents();
      (void)dt;
  }

  void cleanup() override {}
};

int main() {
  InputTester tester{};
  tester.disable_render = true;

  tester.run();
}
