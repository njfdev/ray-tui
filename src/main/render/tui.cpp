#include "render/tui.hpp"
#include <cstdio>
#include <functional>
#include <iostream>
#include <string>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <termios.h>
#include <unistd.h>

void Tui::init() {
  tmux = detectTmux();

  stringBuf = std::string();

  initTermios();

  // disables sync between C and C++ stdio apis, increases speed but just make
  // sure to only use one.
  // std::ios::sync_with_stdio(false);
}

void Tui::render(Framebuffer &fb) {
  int width, height;
  getTerminalDimensions(&width, &height);

  stringBuf.clear();

  // hide cursor and move to 0,0
  stringBuf += "\033[?25l\033[H";

  char fmtBuf[64];

  // fill screen, trying to buffer into string to limit system calls, not sure
  // if it helps
  for (int r = 0; r < height; r++) {
    int y = r * 2;
    for (int c = 0; c < width; c++) {
      Pixel top_px = fb.color(c, y, width, height * 2);
      Pixel bot_px = fb.color(c, y + 1, width, height * 2);

      // genuinely what was going through the head of whoever made terminal ANSI
      // escape sequences and also C formatting strings lol
      int s = snprintf(fmtBuf, sizeof(fmtBuf), "\033[38;2;%d;%d;%dm", top_px.r,
                       top_px.g, top_px.b);
      stringBuf.append(fmtBuf, s);

      s = snprintf(fmtBuf, sizeof(fmtBuf), "\033[48;2;%d;%d;%dm▀", bot_px.r,
                   bot_px.g, bot_px.b);
      stringBuf.append(fmtBuf, s);

      // std::cout << "\033[38;2;" << +top_px.r << ";" << +top_px.g << ";"
      //           << +top_px.b << "m";
      //
      // std::cout << "\033[48;2;" << +bot_px.r << ";" << +bot_px.g << ";"
      //           << +bot_px.b << "m▀";
    }

    if (r + 1 != height) {
      stringBuf += '\n';
      // std::cout << std::endl;
    }
  }

  // begin synchronized update (prevents flickering on supported terminals)
  beginSync(tmux);

  fwrite(stringBuf.data(), 1, stringBuf.size(), stdout);
  fflush(stdout);

  // end synchronized update
  endSync(tmux);
}

void Tui::cleanup() {
  // reshow cursor
  std::cout << "\033[?25h";
  // clear any custom highlighting
  std::cout << "\x1b[0m";

  // resetTermios();
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
    std::cout << "\033Ptmux;\033\033[?2026h\033\\";
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

// originally from
// https://stackoverflow.com/questions/6698161/getting-raw-input-from-console-using-c-or-c
// changed stuff to get it working, honestly don't really know how this works
// these apis are a little silly
void Tui::initTermios() {
  tcgetattr(STDIN_FILENO, &old);
  termios raw = old;
  raw.c_lflag &= ~(ICANON | ECHO);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

void Tui::resetTermios() { tcsetattr(STDIN_FILENO, TCSANOW, &old); }

void Tui::pollInput(std::function<int(int key)> handler, int timeout_ms) {
    pollfd pfd{ STDIN_FILENO, POLLIN, 0 };
    if (poll(&pfd, 1, timeout_ms) <= 0)
        return;

    // Read all available inputs
    unsigned char buf[64];
    ssize_t n = read(STDIN_FILENO, buf, sizeof(buf));
    for (ssize_t i = 0; i < n; i++)
        handler(buf[i]);
}
