#include "math/ray.hpp"
#include "math/shapes.hpp"
#include "math/vec3.hpp"
#include "scene/color.hpp"
#include "scene/scene.hpp"
#include "tui.hpp"

#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <ostream>
#include <thread>
#include <vector>

static std::atomic<bool> isInterrupted(false);
static void onSig(int) { isInterrupted = true; }

struct Framebuffer : RenderSource {
  std::vector<Color> *framebuffer;

  Framebuffer(std::vector<Color> &framebuffer) {
    this->framebuffer = &framebuffer;
  }

  Color color(int x, int y, int width, int height) override {
    return framebuffer->at(y*width + x);
  }
};

int main() {
  std::signal(SIGINT, onSig);

  Plane plane;
  plane.origin = Vec3{0, 0, 0};
  plane.normal = Vec3{0, 0, -1};

  Material mat{Color{200, 200, 200}, 0.0, 1.0, 1.0, 0.0, 1.0};

  Scene scene;
  scene.objects.push_back(SceneObject{mat, &plane});
  scene.background = Color{20, 20, 40};
  scene.fovh = 1.2;

  Ray camera(Vec3{1.0, 0.0, 0.0}.normalize(), Vec3{0.0, 0.0, 1.0});

  Tui tui;
  std::vector<Color> framebuffer;

  Framebuffer src = Framebuffer(framebuffer);

  while (!isInterrupted) {
    int tw, th;
    tui.getTerminalDimensions(&tw, &th);
    int w = tw;
    int h = th * 2; // two framebuffer rows per terminal row (half-blocks)

    framebuffer.assign((size_t)w * h, scene.background);
    scene.render(framebuffer.data(), {w, h}, camera);
    tui.render(src);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  tui.cleanup();
  return 0;
}
