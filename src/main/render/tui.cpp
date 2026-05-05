#include "render/tui.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/ioctl.h>

void Tui::render(Framebuffer &fb) {
  int width, height;
  getTerminalDimensions(&width, &height);

  // hide cursor
  std::cout << "\033[?25l";

  // move to 0,0
  std::cout << "\033[H";

  // begin synchronized update (prevents flickering on supported terminals)
  bool tmux = detectTmux();
  beginSync(tmux);

  // fill screen
  for (int r = 0; r < height; r++) {
    int y = r * 2;
    for (int c = 0; c < width; c++) {
      Pixel top_px = fb.color(c, y, width, height * 2);
      Pixel bot_px = fb.color(c, y + 1, width, height * 2);

      std::cout << "\033[38;2;" << +top_px.r << ";" << +top_px.g << ";"
                << +top_px.b << "m";

      std::cout << "\033[48;2;" << +bot_px.r << ";" << +bot_px.g << ";"
                << +bot_px.b << "m▀";
    }

    if (r + 1 != height) {
      std::cout << std::endl;
    }
  }

  // end synchronized update
  endSync(tmux);

  // make sure terminal buffer is immediately written
  std::cout << std::flush;
}

void Tui::cleanup() {
  // reshow cursor
  std::cout << "\033[?25h";
  // clear any custom highlighting
  std::cout << "\x1b[0m";
}

void Tui::getTerminalDimensions(int *width, int *height) {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  *width = w.ws_col;
  *height = w.ws_row;
}

void Tui::getRenderDimensions(int *width, int *height) {
  getTerminalDimensions(width, height);
  // because terminal characters are split vertically into half blocks
  *height *= 2;
}

// funky tmux detection+workaround for synchronized writes.
bool Tui::detectTmux() {
  const char *tmux = getenv("TMUX");
  if (tmux && *tmux)
    return true;
  return false;
}

void Tui::beginSync(bool tmux) {
  if (tmux) {
    fputs("\033Ptmux;\033\033[?2026h\033\\", stdout);
  } else {
    fputs("\033[?2026h", stdout);
  }
}

void Tui::endSync(bool tmux) {
  if (tmux) {
    fputs("\033Ptmux;\033\033[?2026l\033\\", stdout);
  } else {

    fputs("\033[?2026l", stdout);
  }
  fflush(stdout);
}
