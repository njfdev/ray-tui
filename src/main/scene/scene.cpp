#include "scene/scene.hpp"
#include "math/object.hpp"
#include "math/vec3.hpp"
#include "scene/color.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <optional>
#include <utility>

std::optional<std::pair<Intersection, SceneObject>> Scene::trace(Ray &ray) {
  double bestDist = INFINITY;
  std::optional<Intersection> i = {};
  std::optional<SceneObject> o = {};
  for (auto obj : objects) {
    auto hit = obj.obj->intersect(ray);

    if (hit.has_value()) {
      double dist = (hit->p - ray.origin).sqrLength();
      if (dist < bestDist) {
        i = hit;
        o = obj;
        bestDist = dist;
      }
    }
  }

  if (!o.has_value()) {
    return {};
  }

  return std::optional(std::pair{i.value(), o.value()});
}

void Scene::render(Color framebuffer[], std::array<int, 2> res, Ray fwd) {
  double fovv = fovh * ((double)res[1]) / ((double)res[0]);

  double tanH = std::tan(fovh * 0.5);
  double tanV = std::tan(fovv * 0.5);

  Vec3 right = (fwd.direction ^ Vec3{0.0, 0.0, 1.0}).normalize();
  Vec3 up = (right ^ fwd.direction).normalize();

  for (int iy = 0; iy < res[1]; iy++) {
    for (int ix = 0; ix < res[0]; ix++) {
      // Pixel center in NDC, range [-1, 1]
      double ndcX = (2.0 * (ix + 0.5) / res[0]) - 1.0;
      double ndcY =
          1.0 - (2.0 * (iy + 0.5) / res[1]); // negative, y=0 is top row

      Vec3 dir = (fwd.direction + right * (ndcX * tanH) + up * (ndcY * tanV))
                     .normalize();

      Ray ray{dir, fwd.origin};

      auto hit = trace(ray);
      if (hit.has_value()) {
        double depth = (hit->first.p - fwd.origin).length();

        framebuffer[iy * res[0] + ix] = hit->second.m.color;
      } else {
        framebuffer[iy * res[0] + ix] = background;
      }
    }
  }
}
