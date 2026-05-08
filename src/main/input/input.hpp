// AI generated with GPT-5.2-Codex (2026-05-08)

#pragma once

#include <cstdio>
#include <unistd.h>
#include <termios.h>

#if defined(__APPLE__)
#include <CoreFoundation/CoreFoundation.h>
#elif defined(__linux__)
#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>
#endif

class Input {
public:
    Input();
    ~Input();

    void updateKeyEvents();

#if defined(__APPLE__)
    void enableEventTap();
#endif

private:
    int fd;
    termios old;
    termios raw;

#if defined(__APPLE__)
    CFMachPortRef event_tap;
    CFRunLoopSourceRef run_loop_source;
#elif defined(__linux__)
    Display *display;
    int xi_opcode;
#endif
};
