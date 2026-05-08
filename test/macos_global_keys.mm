// AI generated with GPT-5.2-Codex (2026-05-08)

#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CoreFoundation.h>
#include <cstdio>
#include <cstdlib>
#include <csignal>

static CFMachPortRef g_event_tap = nullptr;
static CFRunLoopSourceRef g_run_loop_source = nullptr;
static bool g_running = true;

static const char *eventTypeName(CGEventType type) {
  switch (type) {
  case kCGEventKeyDown:
    return "KeyDown";
  case kCGEventKeyUp:
    return "KeyUp";
  case kCGEventFlagsChanged:
    return "FlagsChanged";
  default:
    return "Other";
  }
}

static void printKey(CGEventType type, CGEventRef event) {
  CGKeyCode keycode = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
  std::printf("%s keycode=%u\n", eventTypeName(type), (unsigned)keycode);
  std::fflush(stdout);
}

static CGEventRef eventCallback(CGEventTapProxy, CGEventType type, CGEventRef event, void *) {
  if (!g_running) {
    return event;
  }

  if (type == kCGEventTapDisabledByTimeout || type == kCGEventTapDisabledByUserInput) {
    CGEventTapEnable(g_event_tap, true);
    return event;
  }

  if (type == kCGEventKeyDown || type == kCGEventKeyUp || type == kCGEventFlagsChanged) {
    printKey(type, event);
  }

  return event;
}

static void handleSignal(int) {
  g_running = false;
  if (g_run_loop_source) {
    CFRunLoopStop(CFRunLoopGetCurrent());
  }
}

int main() {
  std::signal(SIGINT, handleSignal);
  std::signal(SIGTERM, handleSignal);

  CGEventMask mask = (1ULL << kCGEventKeyDown) | (1ULL << kCGEventKeyUp) |
                     (1ULL << kCGEventFlagsChanged);
  g_event_tap = CGEventTapCreate(kCGSessionEventTap,
                                 kCGHeadInsertEventTap,
                                 kCGEventTapOptionListenOnly,
                                 mask,
                                 eventCallback,
                                 nullptr);

  if (!g_event_tap) {
    std::fprintf(stderr, "Failed to create event tap.\n");
    std::fprintf(stderr, "Enable Input Monitoring for your terminal app.\n");
    return 1;
  }

  g_run_loop_source = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, g_event_tap, 0);
  CFRunLoopAddSource(CFRunLoopGetCurrent(), g_run_loop_source, kCFRunLoopCommonModes);
  CGEventTapEnable(g_event_tap, true);

  std::printf("Listening for global key events. Press Ctrl+C to exit.\n");
  std::fflush(stdout);
  CFRunLoopRun();

  if (g_run_loop_source) {
    CFRunLoopRemoveSource(CFRunLoopGetCurrent(), g_run_loop_source, kCFRunLoopCommonModes);
    CFRelease(g_run_loop_source);
    g_run_loop_source = nullptr;
  }
  if (g_event_tap) {
    CFRelease(g_event_tap);
    g_event_tap = nullptr;
  }

  return 0;
}
