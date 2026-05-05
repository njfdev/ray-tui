#pragma once

#include "render/color.hpp"
#include <vector>

class Framebuffer {
public:
  std::vector<Pixel> framebuffer{};

  Framebuffer() {}

  void resize(int w, int h, Pixel background = {}) {
    framebuffer.assign(w * h, background);
  }

  void resize(int length, Pixel background = {}) {
    framebuffer.assign(length, background);
  }

  Pixel color(int x, int y, int width, int height) {
    return framebuffer.at(y * width + x);
  }
};
