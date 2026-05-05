#include "scene/scene.hpp"
#include "math/object.hpp"
#include "math/vec3.hpp"
#include "render/framebuffer.hpp"
#include "scene/light.hpp"
#include <array>
#include <cmath>
#include <cstdlib>
#include <optional>
#include <utility>

std::optional<std::pair<Intersection, SceneObject>> Scene::trace(Ray &ray) {
  double bestDist = INFINITY;
  double min_dist = 1e-6;
  std::optional<Intersection> i = {};
  std::optional<SceneObject> o = {};
  for (auto obj : objects) {
    auto hit = obj.obj->intersect(ray);

    if (hit.has_value()) {
      double dist = hit->rayDist;
      if (dist < bestDist && dist > min_dist) {
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

void Scene::render(Framebuffer *fb, std::array<int, 2> res, Ray fwd) {
  double fovv = fovh * ((double)res[1]) / ((double)res[0]);

  double tanH = std::tan(fovh * 0.5);
  double tanV = std::tan(fovv * 0.5);

  Vec3 right = (fwd.direction ^ Vec3{0.0, 0.0, 1.0}).normalize();
  Vec3 up = (right ^ fwd.direction).normalize();

  double ndcPixelSizeX = 2.0 / static_cast<double>(res[0]);
  double ndcPixelSizeY = 2.0 / static_cast<double>(res[1]);

  for (int iy = 0; iy < res[1]; iy++) {
    for (int ix = 0; ix < res[0]; ix++) {
      double randX = ndcPixelSizeX * static_cast<double>(rand() % 512) / 2048.0;
      double randY = ndcPixelSizeY * static_cast<double>(rand() % 512) / 2048.0;

      // Pixel center in NDC, range [-1, 1]
      double ndcX = (2.0 * (ix + 0.5) / res[0]) - 1.0 + randX;
      double ndcY =
          1.0 - (2.0 * (iy + 0.5) / res[1]) + randY; // negative, y=0 is top row

      Vec3 dir = (fwd.direction + right * (ndcX * tanH) + up * (ndcY * tanV))
                     .normalize();

      Ray ray{dir, fwd.origin};

      Color color = Color{};
      int lit = 0;
      auto hit = trace(ray);
      if (hit.has_value()) {
        auto p = hit->first.p;
        auto o = hit->second;
        for (auto light : lights) {
          double phong = this->traceLight(p, hit->first.normal, light);

          if (phong != 0) {
            color = color + phong * light.color * o.m.color;
            lit++;
          }
        }
      }

      if (lit == 0) {
        color = background;
      }

      color.validate();

      fb->framebuffer.data()[iy * res[0] + ix] = Pixel{
          static_cast<uint8_t>(color.r * 255.0),
          static_cast<uint8_t>(color.g * 255.0),
          static_cast<uint8_t>(color.b * 255.0),
      };
    }
  }

  this->framecount++;
}

double Scene::traceLight(Vec3 p, Vec3 normal, PointLight light) {
  Vec3 light_offset = light.p - p;
  double light_len = light_offset.sqrLength();
  Vec3 light_dir = light_offset.normalize();
  Ray light_ray = Ray{light_dir, p};
  auto light_hit = trace(light_ray);
  double light_hit_dist = light_hit->first.rayDist;

  if (!light_hit.has_value() || light_hit_dist * light_hit_dist >= light_len) {
    double weight = std::max(0.15, light_dir * normal);

    return weight;
  }
  return 0.3;
}
