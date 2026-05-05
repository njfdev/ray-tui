#include <cstdlib>
#include <csignal>
#include "gameloop.hpp"

class Game : public GameLoop {

    void init() override {
    }

    void update() override {
        for (int y = 0; y < height(); y++) {
            for (int x = 0; x < width(); x++) {
                fb.framebuffer.data()[y*width() + x] = {(int)((((double)x)/width())*255.0), (int)((((double)y)/height())*255), 0};
            }
        }
    }

    void cleanup() override {

    }

};

int main() {
    Game game{};

    game.run();
}
