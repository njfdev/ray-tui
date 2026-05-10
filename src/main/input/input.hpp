#pragma once

#include "input/raw_inputs.hpp"

enum Key {
    A = 0,
    S,
    D,
    F,
    H,
    G,
    Z,
    X,
    C,
    V,
    B = 11,
    Q,
    W,
    E,
    R,
    Y,
    T,
    ONE,
    TWO,
    THREE,
    FOUR,
    SIX,
    FIVE,
    EQUALS,
    NINE,
    SEVEN,
    HYPEN,
    EIGHT,
    ZERO,
    RIGHT_BRACKET,
    O,
    U,
    LEFT_BRACKET,
    I,
    P,
    RETURN,
    L = 37,
    J,
    APOSTROPHE,
    K,
    COLON,
    BACK_SLASH,
    COMMA,
    FORWARD_SLASH,
    N,
    M,
    PERIOD,
    TAB,
    SPACE,
    BACKTICK,
    DELETE,
    ENTER,
    ESCAPE,
};

class Input {
public:
    Input();
    ~Input();

    void update();

    bool isKeyPressed(Key key);


private:
    RawInputs raw_inputs{};
    int keyCodeStates[255] = {0};

    bool isTerminalFocused = true;

    void updateFocusedStatus();
    bool isInputBuffered();

};
