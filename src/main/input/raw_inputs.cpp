// AI generated with GPT-5.2-Codex (2026-05-08)

#include "input/raw_inputs.hpp"
#include <cstdint>
#include <cstdlib>
#include <fcntl.h>
#include <glob.h>
#include <iostream>
#include <poll.h>
#include <termios.h>
#include <unistd.h>
#include <cstring>
#if defined(__linux__)
#include <linux/input.h>
#endif

#if defined(__APPLE__)
#include <ApplicationServices/ApplicationServices.h>
#elif defined(__linux__)
#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>
#endif

#if defined(__APPLE__)
namespace {
const char *eventTypeName(CGEventType type) {
    switch (type) {
    case kCGEventKeyDown:
        return "KeyDown";
    case kCGEventKeyUp:
        return "KeyUp";
    default:
        return "Other";
    }
}

CGEventRef eventCallback(CGEventTapProxy, CGEventType type, CGEventRef event, void *refcon) {
    if (type == kCGEventTapDisabledByTimeout || type == kCGEventTapDisabledByUserInput) {
        auto *input = static_cast<RawInputs *>(refcon);
        if (input) {
            input->enableEventTap();
        }
        return event;
    }

    if (type == kCGEventKeyDown || type == kCGEventKeyUp) {
        CGKeyCode keycode = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
        bool down = (type == kCGEventKeyDown);
        auto *input = static_cast<RawInputs *>(refcon);
        if (input) {
            input->pushEvent(down, static_cast<unsigned>(keycode));
        }
    }

    return event;
}
} // namespace
#endif

#if defined(__APPLE__)
void RawInputs::enableEventTap() {
    if (event_tap) {
        CGEventTapEnable(event_tap, true);
    }
}
#endif

RawInputs::RawInputs() {
    tcgetattr(STDIN_FILENO, &old);

    raw = old;
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);

#if defined(__APPLE__)
    event_tap = nullptr;
    run_loop_source = nullptr;

    CGEventMask mask = (1ULL << kCGEventKeyDown) | (1ULL << kCGEventKeyUp);
    event_tap = CGEventTapCreate(kCGSessionEventTap,
                                 kCGHeadInsertEventTap,
                                 kCGEventTapOptionListenOnly,
                                 mask,
                                 eventCallback,
                                 this);
    if (event_tap) {
        run_loop_source = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, event_tap, 0);
        CFRunLoopAddSource(CFRunLoopGetCurrent(), run_loop_source, kCFRunLoopCommonModes);
        CGEventTapEnable(event_tap, true);
    }
#elif defined(__linux__)
    // Prefer Wayland stdin path over X11/XWayland when running under Wayland
    if (getenv("WAYLAND_DISPLAY")) {
        use_stdin_keys = true;
        display = nullptr;
        // Try to open evdev keyboard devices for proper key up/down events
        glob_t g;
        if (glob("/dev/input/event*", 0, nullptr, &g) == 0) {
            for (size_t i = 0; i < g.gl_pathc; i++) {
                int efd = open(g.gl_pathv[i], O_RDONLY | O_NONBLOCK);
                if (efd < 0) continue;
                uint8_t evbits[(EV_MAX + 7) / 8] = {};
                uint8_t keybits[(KEY_MAX + 7) / 8] = {};
                bool has_ev_key = ioctl(efd, EVIOCGBIT(0, sizeof(evbits)), evbits) >= 0
                                  && (evbits[EV_KEY / 8] & (1 << (EV_KEY % 8)));
                bool has_key_a  = has_ev_key
                                  && ioctl(efd, EVIOCGBIT(EV_KEY, sizeof(keybits)), keybits) >= 0
                                  && (keybits[KEY_A / 8] & (1 << (KEY_A % 8)));
                if (has_key_a) evdev_fds.push_back(efd);
                else close(efd);
            }
            globfree(&g);
        }
        // Kitty protocol as fallback when evdev is unavailable
        if (evdev_fds.empty())
            std::cout << "\033[>3u" << std::flush;
    } else {
    display = XOpenDisplay(nullptr);
    xi_opcode = 0;
    if (!display) {
        use_stdin_keys = true;
        std::cout << "\033[>3u" << std::flush;
    } else if (display) {
        int event, error;
        if (XQueryExtension(display, "XInputExtension", &xi_opcode, &event, &error)) {
            int major = 2;
            int minor = 0;
            if (XIQueryVersion(display, &major, &minor) == Success) {
                XIEventMask mask;
                unsigned char mask_data[(XI_LASTEVENT + 7) / 8];
                std::memset(mask_data, 0, sizeof(mask_data));
                XISetMask(mask_data, XI_RawKeyPress);
                XISetMask(mask_data, XI_RawKeyRelease);
                mask.deviceid = XIAllMasterDevices;
                mask.mask_len = sizeof(mask_data);
                mask.mask = mask_data;
                Window root = DefaultRootWindow(display);
                XISelectEvents(display, root, &mask, 1);
                XFlush(display);
            }
        }
    }
    } // else (not WAYLAND_DISPLAY)
#endif
}

RawInputs::~RawInputs() {
#if defined(__linux__)
    if (use_stdin_keys && evdev_fds.empty())
        std::cout << "\033[<u" << std::flush; // pop Kitty protocol stack
    for (int efd : evdev_fds) close(efd);
    tcsetattr(STDIN_FILENO, TCSANOW, &old);
#endif

#if defined(__APPLE__)
    if (run_loop_source) {
        CFRunLoopRemoveSource(CFRunLoopGetCurrent(), run_loop_source, kCFRunLoopCommonModes);
        CFRelease(run_loop_source);
        run_loop_source = nullptr;
    }
    if (event_tap) {
        CFRelease(event_tap);
        event_tap = nullptr;
    }
#elif defined(__linux__)
    if (display) {
        XCloseDisplay(display);
        display = nullptr;
    }
#endif
}

void RawInputs::pushEvent(bool down, unsigned int keycode) {
    pending_events.push_back(RawKeyEvent{down, keycode});
}

// Maps X11 keycode (evdev+8) to Key enum value (macOS VK layout)
static int x11ToKeyEnum(unsigned int kc) {
    switch (kc) {
    case 38: return 0;  case 39: return 1;  case 40: return 2;  case 41: return 3;
    case 43: return 4;  case 42: return 5;  case 52: return 6;  case 53: return 7;
    case 54: return 8;  case 55: return 9;  case 56: return 11; case 24: return 12;
    case 25: return 13; case 26: return 14; case 27: return 15; case 29: return 16;
    case 28: return 17; case 10: return 18; case 11: return 19; case 12: return 20;
    case 13: return 21; case 15: return 22; case 14: return 23; case 21: return 24;
    case 18: return 25; case 16: return 26; case 20: return 27; case 17: return 28;
    case 19: return 29; case 35: return 30; case 32: return 31; case 30: return 32;
    case 34: return 33; case 31: return 34; case 33: return 35; case 36: return 36;
    case 46: return 37; case 44: return 38; case 48: return 39; case 45: return 40;
    case 47: return 41; case 51: return 42; case 59: return 43; case 61: return 44;
    case 57: return 45; case 58: return 46; case 60: return 47; case 23: return 48;
    case 65: return 49; case 49: return 50; case 22: return 51; case  9: return 53;
    default: return -1;
    }
}

int RawInputs::asciiToKeycode(char c) {
    switch (c) {
    case 'a': case 'A': return 0;
    case 's': case 'S': return 1;
    case 'd': case 'D': return 2;
    case 'f': case 'F': return 3;
    case 'h': case 'H': return 4;
    case 'g': case 'G': return 5;
    case 'z': case 'Z': return 6;
    case 'x': case 'X': return 7;
    case 'c': case 'C': return 8;
    case 'v': case 'V': return 9;
    case 'b': case 'B': return 11;
    case 'q': case 'Q': return 12;
    case 'w': case 'W': return 13;
    case 'e': case 'E': return 14;
    case 'r': case 'R': return 15;
    case 'y': case 'Y': return 16;
    case 't': case 'T': return 17;
    case '1': return 18;
    case '2': return 19;
    case '3': return 20;
    case '4': return 21;
    case '6': return 22;
    case '5': return 23;
    case '=': return 24;
    case '9': return 25;
    case '7': return 26;
    case '-': return 27;
    case '8': return 28;
    case '0': return 29;
    case ']': return 30;
    case 'o': case 'O': return 31;
    case 'u': case 'U': return 32;
    case '[': return 33;
    case 'i': case 'I': return 34;
    case 'p': case 'P': return 35;
    case '\r': return 36;
    case 'l': case 'L': return 37;
    case 'j': case 'J': return 38;
    case '\'': return 39;
    case 'k': case 'K': return 40;
    case ';': return 41;
    case '\\': return 42;
    case ',': return 43;
    case '/': return 44;
    case 'n': case 'N': return 45;
    case 'm': case 'M': return 46;
    case '.': return 47;
    case '\t': return 48;
    case ' ': return 49;
    case '`': return 50;
    case '\x7f': return 51;
    case '\n': return 52;
    default: return -1;
    }
}

std::vector<RawInputs::RawKeyEvent> RawInputs::updateKeyEvents() {
    pending_events.clear();
#if defined(__APPLE__)
    if (!event_tap) {
        return {};
    }
    CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, true);
#elif defined(__linux__)
    if (use_stdin_keys) {
        // Evdev path: proper key up/down, no Kitty needed
        if (!evdev_fds.empty()) {
            for (int efd : evdev_fds) {
                struct input_event ev;
                while (read(efd, &ev, sizeof(ev)) == (ssize_t)sizeof(ev)) {
                    if (ev.type == EV_KEY && ev.value != 2) { // ignore repeat
                        bool down = (ev.value == 1);
                        int kc = x11ToKeyEnum(ev.code + 8);
                        if (kc >= 0) pushEvent(down, (unsigned)kc);
                    }
                }
            }
            // Read stdin only for focus events
            char fbuf[256];
            ssize_t fn;
            struct pollfd fpfd = { STDIN_FILENO, POLLIN, 0 };
            while (poll(&fpfd, 1, 0) > 0 && (fpfd.revents & POLLIN)) {
                fn = read(STDIN_FILENO, fbuf, sizeof(fbuf));
                if (fn <= 0) break;
                fpfd.revents = 0;
                for (ssize_t i = 0; i + 2 < fn; i++) {
                    if (fbuf[i] == '\033' && fbuf[i+1] == '[') {
                        if      (fbuf[i+2] == 'I') { focused = true;  i += 2; }
                        else if (fbuf[i+2] == 'O') { focused = false; i += 2; }
                    }
                }
            }
            return pending_events;
        }

        char buf[4096];
        ssize_t n;
        struct pollfd pfd = { STDIN_FILENO, POLLIN, 0 };

        // Keys seen this frame from the non-Kitty bare-char fallback path
        std::unordered_set<int> seen_bare_keys;

        // Flush a completed CSI sequence using persistent state
        auto flush_csi = [&](char terminator) {
            if (stdin_has_cur) stdin_params.push_back(stdin_cur);
            stdin_cur = 0; stdin_has_cur = false;

            if (terminator == 'I') {
                focused = true;
            } else if (terminator == 'O') {
                focused = false;
            } else if (terminator == 'u') {
                // Kitty key event: explicit press/release
                int codepoint  = stdin_params.size() >= 1 ? stdin_params[0] : 0;
                int event_type = stdin_params.size() >= 3 ? stdin_params[2] : 1;
                if (event_type == 2) event_type = 1; // repeat → press
                bool down = (event_type != 3);
                int kc = (codepoint == 27) ? 53
                       : (codepoint < 128) ? asciiToKeycode((char)codepoint) : -1;
                if (kc >= 0) pushEvent(down, (unsigned)kc);
            }
            stdin_params.clear();
        };

        while (poll(&pfd, 1, 0) > 0 && (pfd.revents & POLLIN)) {
            n = read(STDIN_FILENO, buf, sizeof(buf));
            if (n <= 0) break;
            pfd.revents = 0;

            for (int i = 0; i < n; i++) {
                char c = buf[i];
                switch (stdin_state) {
                case StdinState::NORMAL:
                    if (c == '\033') {
                        stdin_state = StdinState::ESC;
                    } else {
                        // Non-Kitty terminal: bare ASCII char means key is held this frame
                        int kc = asciiToKeycode(c);
                        if (kc >= 0) seen_bare_keys.insert(kc);
                    }
                    break;
                case StdinState::ESC:
                    if (c == '[') {
                        stdin_state = StdinState::CSI;
                    } else {
                        // Bare ESC key
                        seen_bare_keys.insert(53);
                        stdin_state = StdinState::NORMAL;
                        int kc = asciiToKeycode(c);
                        if (kc >= 0) seen_bare_keys.insert(kc);
                    }
                    break;
                case StdinState::CSI:
                    if (c >= '0' && c <= '9') {
                        stdin_cur = stdin_cur * 10 + (c - '0');
                        stdin_has_cur = true;
                    } else if (c == ';') {
                        stdin_params.push_back(stdin_has_cur ? stdin_cur : 0);
                        stdin_cur = 0; stdin_has_cur = false;
                    } else {
                        flush_csi(c);
                        stdin_state = StdinState::NORMAL;
                    }
                    break;
                }
            }
        }

        // Non-Kitty fallback: emit press for newly seen keys, release for gone keys
        for (int kc : seen_bare_keys)
            if (!stdin_active_keys.count(kc)) pushEvent(true, (unsigned)kc);
        for (int kc : stdin_active_keys)
            if (!seen_bare_keys.count(kc)) pushEvent(false, (unsigned)kc);
        stdin_active_keys = seen_bare_keys;

        return pending_events;
    }
    if (!display) {
        return {};
    }
    while (XPending(display) > 0) {
        XEvent event;
        XNextEvent(display, &event);
        if (event.xcookie.type == GenericEvent && event.xcookie.extension == xi_opcode &&
            XGetEventData(display, &event.xcookie)) {
            if (event.xcookie.evtype == XI_RawKeyPress || event.xcookie.evtype == XI_RawKeyRelease) {
                auto *raw = static_cast<XIRawEvent *>(event.xcookie.data);
                bool down = (event.xcookie.evtype == XI_RawKeyPress);
                int kc = x11ToKeyEnum(raw->detail);
                if (kc >= 0) pushEvent(down, static_cast<unsigned>(kc));
            }
            XFreeEventData(display, &event.xcookie);
        }
    }
#else
    char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
        unsigned int keycode = static_cast<unsigned>(static_cast<unsigned char>(c));
        pushEvent(true, keycode);
        pushEvent(false, keycode);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    if (c == 'q') {
        tcsetattr(STDIN_FILENO, TCSANOW, &old);
    }
#endif
    return pending_events;
}
