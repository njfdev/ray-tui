#pragma once

#include "render/color.hpp"
#include <vector>

class Framebuffer {
public:
  std::vector<Pixel> framebuffer{};
  int w=0;
  int h=0;

  Framebuffer() {}

  void resize(int w, int h, Pixel background = {}) {
    this->w = w;
    this->h = h;
    framebuffer.assign(w * h, background);
  }

  Pixel color(int x, int y, int width, int height) {
      int index = y*width+x;
      if (index >= framebuffer.size()) {
          return Pixel{0, 0, 0};
      }
      return framebuffer.at(index);
  }
};
