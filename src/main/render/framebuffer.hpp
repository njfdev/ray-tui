#pragma once

#include "render/color.hpp"
#include <vector>

/**
 * A framebuffer of {Pixel}s
 */
class Framebuffer {
public:
  std::vector<Pixel> framebuffer{};
  int w = 0;
  int h = 0;

  Framebuffer() {}

  /**
   * Resizes the framebuffer to w*h
   *
   * @param w width of the framebuffer
   * @param h height of the framebuffer
   * @param background fill color of the framebuffer.
   *
   * O(w*h)
   */
  void resize(int w, int h, Pixel background = {}) {
    this->w = w;
    this->h = h;
    framebuffer.assign(w * h, background);
  }

  /**
   * Retrieves the pixel at x,y,
   *
   * @param x the x coordinate of the pixel
   * @param y the y coordinate of the pixel
   *
   * @returns the pixel at (x,y), if not present then returns a black pixel.
   *
   */
  Pixel color(int x, int y) {
    int index = y * w + x;
    if (index >= framebuffer.size()) {
      return Pixel{0, 0, 0};
    }
    return framebuffer.at(index);
  }
};
