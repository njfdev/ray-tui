// AI generated with GPT-5.2-Codex (2026-05-08)

#pragma once

#include <cstdio>
#include <unistd.h>
#include <termios.h>
#include <vector>

#if defined(__APPLE__)
#include <CoreFoundation/CoreFoundation.h>
#elif defined(__linux__)
#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>
#endif

class RawInputs {
public:
    RawInputs();
    ~RawInputs();

    struct RawKeyEvent {
        bool down;
        unsigned int keycode;
    };

    std::vector<RawKeyEvent> updateKeyEvents();

    void pushEvent(bool down, unsigned int keycode);

#if defined(__APPLE__)
    void enableEventTap();
#endif

private:
    int fd;
    termios old;
    termios raw;

    std::vector<RawKeyEvent> pending_events;

#if defined(__APPLE__)
    CFMachPortRef event_tap;
    CFRunLoopSourceRef run_loop_source;
#elif defined(__linux__)
    Display *display;
    int xi_opcode;
#endif
};
