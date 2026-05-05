#pragma once

#include <vector>
#include "render/color.hpp"

class Framebuffer {
public:
  std::vector<Color> framebuffer{};

  Framebuffer() {}

  void resize(int w, int h, Color background = {0, 0, 0}) {
      framebuffer.assign(w * h, background);
  }

  void resize(int length, Color background = {0, 0, 0}) {
      framebuffer.assign(length, background);
  }

  Color color(int x, int y, int width, int height) {
    return framebuffer.at(y*width + x);
  }
};
