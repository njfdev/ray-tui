// AI generated with GPT-5.2-Codex (2026-05-08)

#include "input/input.hpp"
#include "tui.hpp"
#include <chrono>
#include <termios.h>
#include <thread>
#include <unistd.h>
#include <iostream>
#include <cstring>

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
        auto *input = static_cast<Input *>(refcon);
        if (input) {
            input->enableEventTap();
        }
        return event;
    }

    if (type == kCGEventKeyDown || type == kCGEventKeyUp) {
        CGKeyCode keycode = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
        std::cout << eventTypeName(type) << " keycode=" << static_cast<unsigned>(keycode) << std::endl;
    }

    return event;
}
} // namespace
#endif

#if defined(__APPLE__)
void Input::enableEventTap() {
    if (event_tap) {
        CGEventTapEnable(event_tap, true);
    }
}
#endif

Input::Input() {
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
    display = XOpenDisplay(nullptr);
    xi_opcode = 0;
    if (display) {
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
#endif
}

Input::~Input() {
    tcsetattr(STDIN_FILENO, TCSANOW, &old);

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

void Input::updateKeyEvents() {
#if defined(__APPLE__)
    if (!event_tap) {
        std::cerr << "Event tap not available. Enable Input Monitoring." << std::endl;
        return;
    }
    CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, true);
#elif defined(__linux__)
    if (!display) {
        std::cerr << "X11 display not available." << std::endl;
        return;
    }
    while (XPending(display) > 0) {
        XEvent event;
        XNextEvent(display, &event);
        if (event.xcookie.type == GenericEvent && event.xcookie.extension == xi_opcode &&
            XGetEventData(display, &event.xcookie)) {
            if (event.xcookie.evtype == XI_RawKeyPress || event.xcookie.evtype == XI_RawKeyRelease) {
                auto *raw = static_cast<XIRawEvent *>(event.xcookie.data);
                const char *name = (event.xcookie.evtype == XI_RawKeyPress) ? "KeyDown" : "KeyUp";
                std::cout << name << " keycode=" << raw->detail << std::endl;
            }
            XFreeEventData(display, &event.xcookie);
        }
    }
#else
    char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
        std::cout << "KeyDown keycode=" << static_cast<unsigned>(c) << std::endl;
        std::cout << "KeyUp keycode=" << static_cast<unsigned>(c) << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    if (c == 'q') {
        tcsetattr(STDIN_FILENO, TCSANOW, &old);
    }
#endif
}
