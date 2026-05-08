#include "input/input.hpp"
#include "input/raw_inputs.hpp"
#include <iostream>
#include <sys/_types/_fd_def.h>

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

bool Input::isKeyPressed(KeyCode key) {
    return isTerminalFocused && keyCodeStates[(int)key];
}

void Input::updateFocusedStatus() {
    char c;

    int indexToAnsiSequence = 0;
    while (read(STDIN_FILENO, &c, 1) > 0) {
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
