#pragma once

#include "render/framebuffer.hpp"
#include <string>
#include <termios.h>

/**
 * Utility class for terminal IO needeed to create the terminal UI (TUI)
 */
class Tui {
public:
  /**
   * Initializes the TUI
   */
  void init();

  /**
   * Renders a frame to the terminal screen
   *
   * @param src the framebuffer to render
   *
   * O(n) where n is the number of pixels in the framebuffer
   */
  void render(Framebuffer &src);

  /**
   * Cleans up the TUI, resetting the terminal to its previous state
   */
  void cleanup();

  /**
   * Gets the frame size that can be rendered to the terminal.
   *
   * @param width a pointer to the integer that will be overwritten with the
   * width of the terminal "screen"
   *
   * @param width a pointer to the integer that will be overwritten with the
   * height of the terminal "screen"
   */
  void getRenderDimensions(int *width, int *height);

private:
  bool tmux = false;
  std::string stringBuf;

  void getTerminalDimensions(int *width, int *height);
  bool detectTmux();
  void beginSync(bool tmux);
  void endSync(bool tmux);
};
