#pragma once

#include "render/framebuffer.hpp"
#include <functional>
#include <string>
#include <termios.h>

class Tui {
public:
  void init();
  void render(Framebuffer &src);
  void cleanup();
  void getRenderDimensions(int *width, int *height);

  void pollInput(std::function<int(int key)> handler, int timeout_ms);

private:
  bool tmux = false;
  std::string stringBuf;
  termios old;

  void getTerminalDimensions(int *width, int *height);
  bool detectTmux();
  void beginSync(bool tmux);
  void endSync(bool tmux);

  void initTermios();
  void resetTermios();
};
