#include "math/ray.hpp"
#include "math/shapes.hpp"
#include "math/vec3.hpp"
#include "scene/color.hpp"
#include "scene/scene.hpp"
#include "tui.hpp"

#include <atomic>
#include <chrono>
#include <cmath>
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
  plane.normal = Vec3{0, 0, 1};

  Sphere sphere = {};
  sphere.origin = Vec3{0.0,0.0,0.0};
  sphere.radius = 0.4;

  Material mat1{Color{200, 0, 0}, 0.0, 1.0, 1.0, 0.0, 1.0};
  Material mat2{Color{0, 0, 200}, 0.0, 1.0, 1.0, 0.0, 1.0};

  Scene scene;
  scene.objects.push_back(SceneObject{mat1, &plane});
  scene.objects.push_back(SceneObject{mat2, &sphere});
  scene.background = Color{20, 20, 40};
  scene.fovh = 1.2;

  Ray camera(Vec3{1.0, 0.0, 0.0}.normalize(), Vec3{-5.0, 0.0, 1.0});

  Tui tui;
  std::vector<Color> framebuffer;

  Framebuffer src = Framebuffer(framebuffer);

  double t = 0.0;

  while (!isInterrupted) {
    int tw, th;
    tui.getTerminalDimensions(&tw, &th);
    int w = tw;
    int h = th * 2; // two framebuffer rows per terminal row (half-blocks)

    framebuffer.assign((size_t)w * h, scene.background);
    camera.direction = Vec3 { cos(t), sin(t), 0.0 };
    scene.render(framebuffer.data(), {w, h}, camera);
    tui.render(src);
    t += 0.05;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  tui.cleanup();
  return 0;
}
