#include "gameloop.hpp"
#include <iostream>

class InputTester: public GameLoop {
  double t = 0.0;

  void init() override {

  }

  void update(double_t dt) override {

      std::cout << cur_key;
    // for (auto it = keys_down.begin(); it != keys_down.end(); ++it) {
    //   std::cout << it->first << ", ";
    // }

    std::cout << std::endl;
  }

  void cleanup() override {}
};

int main() {
  InputTester tester{};
  tester.disable_render = true;

  tester.run();
}

