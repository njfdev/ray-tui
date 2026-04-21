#include <cstdlib>
#include <iostream>
#include <ratio>
#include <thread>

#include "tui.hpp"

int main(int argc, char *argv[])
{
    Tui tui;
    while (true) {
        tui.render();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
