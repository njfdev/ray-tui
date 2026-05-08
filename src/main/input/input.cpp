#include "input/input.hpp"
#include "input/raw_inputs.hpp"
#include <iostream>
#include <ostream>

Input::Input() {}

void Input::update() {
    std::vector<RawInputs::RawKeyEvent> events = raw_inputs.updateKeyEvents();

    for (RawInputs::RawKeyEvent event : events) {
        keyCodeStates[event.keycode] = event.down;
        // std::cout << event.keycode << ": " << event.down << std::endl;
    }
}

bool Input::isKeyPressed(KeyCode key) {
    return keyCodeStates[(int)key];
}
