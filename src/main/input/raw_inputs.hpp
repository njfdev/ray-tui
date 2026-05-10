// AI generated with GPT-5.2-Codex (2026-05-08)

#pragma once

#include <poll.h>
#include <unistd.h>
#include <termios.h>
#include <vector>
#include <unordered_set>

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

    bool isFocused() const { return focused; }
    bool isUsingStdinKeys() const { return use_stdin_keys; }

#if defined(__APPLE__)
    void enableEventTap();
#endif

private:
    int fd;
    termios old;
    termios raw;
    bool use_stdin_keys = false;
    bool focused = true;

    std::vector<RawKeyEvent> pending_events;

    // Persistent stdin parser state — escape sequences can be split across reads
    enum class StdinState { NORMAL, ESC, CSI } stdin_state = StdinState::NORMAL;
    std::vector<int> stdin_params;
    int stdin_cur = 0;
    bool stdin_has_cur = false;

    // Non-Kitty fallback: keys seen last frame, used to emit releases
    std::unordered_set<int> stdin_active_keys;

#if defined(__APPLE__)
    CFMachPortRef event_tap;
    CFRunLoopSourceRef run_loop_source;
#elif defined(__linux__)
    Display *display;
    int xi_opcode;
    std::vector<int> evdev_fds;
#endif

    static int asciiToKeycode(char c);
};
