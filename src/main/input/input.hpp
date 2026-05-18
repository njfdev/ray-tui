#pragma once

#include "input/raw_inputs.hpp"

/**
 * macOS mapping of keyboard keys to their keycodes, where
 * the enum index is the keycode.
 */
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

/**
 * Wrapper around the RawInputs interface to OS/terminal specific
 * keyboard handling. Input allows a program to check the current
 * on/off state of any key.
 */
class Input {
public:
    /**
     * Create a new Input instance. This constructor also prepares
     * the terminal for focus reporting.
     *
     * @return newly created Input instance
     */
    Input();
    /**
     * Cleanup the terminal state from focus reporting mode.
     */
    ~Input();

    /**
     * Updates the state of the keyboard. Should be called once
     * at the beginning of each update loop.
     */
    void update();

    /**
     * Check the on/off state of a single key.
     *
     * @param key which key to check
     * @return the status of that key
     */
    bool isKeyPressed(Key key);


private:
    RawInputs raw_inputs{};
    int keyCodeStates[255] = {0};

    bool isTerminalFocused = true;

    /**
     * Check whether the terminal is focused or not to filter inputs
     */
    void updateFocusedStatus();
    bool isInputBuffered();

};
