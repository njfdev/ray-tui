#pragma once
#include "material.hpp"
#include "math/object.hpp"
#include "math/ray.hpp"
#include "scene/color.hpp"
#include "scene/light.hpp"
#include <array>
#include <vector>

struct SceneObject {
  Material m;
  RayObject *obj;
};

struct BounceConfig {
  int reflect;
  int diffuse;
  int nDiffuse;
};

class Scene {
public:
  std::vector<SceneObject> objects;
  std::vector<PointLight> lights;
  Color background;
  double fovh;

  void render(Color framebuffer[], std::array<int, 2> res, Ray camera);

private:
  std::optional<std::pair<Intersection, SceneObject>> trace(Ray &ray);
};
