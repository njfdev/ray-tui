#include "math/ray.hpp"
#include "math/shapes.hpp"
#include "math/vec3.hpp"
#include "gameloop.hpp"
#include "render/color.hpp"
#include "scene/scene.hpp"

#include <csignal>
#include <vector>

class PlaneCastTest : public GameLoop {
    Scene scene;
    Plane plane;
    Ray camera{Vec3{1.0, 0.0, 0.0}.normalize(), Vec3{0.0, 0.0, 1.0}};

    void init() override {
        plane.origin = Vec3{0, 0, 0};
        plane.normal = Vec3{0, 0, -1};

        Material mat{Color{200, 200, 200}, 0.0, 1.0, 1.0, 0.0, 1.0};

        scene.objects.push_back(SceneObject{mat, &plane});
        scene.background = Color{20, 20, 40};
        scene.fovh = 1.2;
    }

    void update() override {
        scene.render(&fb, {width(), height()}, camera);
    }

    void cleanup() override {

    }

};

int main() {
    PlaneCastTest game{};

    game.run();
}
