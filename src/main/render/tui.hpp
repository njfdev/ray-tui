#pragma once

#include "render/framebuffer.hpp"
#include <string>
#include <termios.h>

class Tui {
public:
  void init();
  void render(Framebuffer &src);
  void cleanup();
  void getRenderDimensions(int *width, int *height);

private:
  bool tmux = false;
  std::string stringBuf;

  void getTerminalDimensions(int *width, int *height);
  bool detectTmux();
  void beginSync(bool tmux);
  void endSync(bool tmux);
};
