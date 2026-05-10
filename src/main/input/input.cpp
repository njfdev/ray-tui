#include "input/input.hpp"
#include "input/raw_inputs.hpp"
#include <iostream>
#include <poll.h>

Input::Input() {
    // enable focus reporting
    std::cout << "\033[?1004h" << std::flush;
}

Input::~Input() {
    // disable focus reporting
    std::cout << "\033[?1004l" << std::flush;
}

void Input::update() {
    updateFocusedStatus();

    std::vector<RawInputs::RawKeyEvent> events = raw_inputs.updateKeyEvents();

    for (RawInputs::RawKeyEvent event : events) {
        keyCodeStates[event.keycode] = event.down;
        // std::cout << event.keycode << ": " << event.down << std::endl;
    }
}

bool Input::isKeyPressed(Key key) {
    return isTerminalFocused && keyCodeStates[(int)key];
}

void Input::updateFocusedStatus() {
    if (raw_inputs.isUsingStdinKeys()) {
        isTerminalFocused = raw_inputs.isFocused();
        return;
    }

    char c;
    int indexToAnsiSequence = 0;
    struct pollfd pfd = { STDIN_FILENO, POLLIN, 0 };
    while (poll(&pfd, 1, 0) > 0 && (pfd.revents & POLLIN) && read(STDIN_FILENO, &c, 1) > 0) {
        pfd.revents = 0;
        if (indexToAnsiSequence == 0 && c == '\033') {
            indexToAnsiSequence += 1;
        } else if (indexToAnsiSequence == 1 && c == '[') {
            indexToAnsiSequence += 1;
        } else if (indexToAnsiSequence == 2) {
            if (c == 'I') {
                isTerminalFocused = true;
            } else if (c == 'O') {
                isTerminalFocused = false;
            }
            indexToAnsiSequence = 0;
        }
    }
}
