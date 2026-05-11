#include "scene/scene.hpp"
#include "math/intersection.hpp"
#include "math/vec3.hpp"
#include "render/framebuffer.hpp"
#include "scene/light.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iterator>

Intersection Scene::trace(Ray &ray, double min_dist) {
  return bvh.intersect(ray);
  // double best_dist = INFINITY;
  //
  // // default initialize as invalid
  // Intersection best_hit{};
  //
  // int i = 0;
  // for (auto shape : shapes) {
  //   auto hit = intersect(ray, shape);
  //   if (hit.dist < best_dist && hit.dist > min_dist) {
  //     best_hit = hit;
  //     best_dist = hit.dist;
  //     best_hit.obj = &object_data[i];
  //   }
  //   i++;
  // }
  //
  // return best_hit;
}

SceneObject* Scene::add_object(Shape shape, Material mat) {
  auto res = SceneObject { mat, nullptr };
  return bvh.insert(shape, bounds(shape), res);
  // if(!empty_slots.empty()) {
  //   int i = empty_slots.top();
  //   empty_slots.pop();
  //   shapes[i] = shape;
  //
  //   object_data[i] = SceneObject {
  //     mat, &shapes[i]
  //   };
  //
  //   return &object_data.at(i);
  // };
  //
  // shapes.push_back(shape);
  //
  // object_data.push_back(SceneObject {
  //   mat, &shapes.back()
  // });
  //
  // return &object_data.back();
}

void Scene::render(Framebuffer *fb, Ray fwd) {
  int w = fb->w;
  int h = fb->h;
  double fovv = fovh * ((double)h) / ((double)w);

  double tanH = std::tan(fovh * 0.5);
  double tanV = std::tan(fovv * 0.5);

  Vec3 right = (fwd.direction ^ Vec3{0.0, 0.0, 1.0}).normalize();
  Vec3 up = (right ^ fwd.direction).normalize();

  double ndcPixelSizeX = 2.0 / static_cast<double>(w);
  double ndcPixelSizeY = 2.0 / static_cast<double>(h);

  for (int iy = 0; iy < h; iy++) {
    for (int ix = 0; ix < w; ix++) {
      double randX = ndcPixelSizeX * static_cast<double>(rand() % 512) / 2048.0;
      double randY = ndcPixelSizeY * static_cast<double>(rand() % 512) / 2048.0;

      // Pixel center in NDC, range [-1, 1]
      double ndcX = (2.0 * (ix + 0.5) / w) - 1.0 + randX;
      double ndcY =
          1.0 - (2.0 * (iy + 0.5) / h) + randY; // negative, y=0 is top row

      Vec3 dir = (fwd.direction + right * (ndcX * tanH) + up * (ndcY * tanV))
                     .normalize();

      Ray ray{dir, fwd.origin};

      Color color = Color{};
      int lit = 0;
      auto hit = trace(ray, 1e-5);
      if (hit.valid()) {
        for (auto light : lights) {
          double phong = this->traceLight(hit.p, hit.normal, light);

          if (phong != 0) {
            color = color + phong * light.color * hit.obj->m.color;
            lit++;
          }
        }
      }

      if (lit == 0) {
        color = background;
      }

      color.validate();

      fb->framebuffer.data()[iy * w + ix] = Pixel{
          static_cast<uint8_t>(color.r * 255.0),
          static_cast<uint8_t>(color.g * 255.0),
          static_cast<uint8_t>(color.b * 255.0),
      };
    }
  }

  this->cur_frame++;
}

double Scene::traceLight(Vec3 p, Vec3 normal, PointLight light) {
  Vec3 light_offset = light.p - p;
  double light_len = light_offset.sqrLength();
  Vec3 light_dir = light_offset.normalize();
  Ray light_ray = Ray{light_dir, p + light_dir*0.01};
  Intersection light_hit = trace(light_ray, 0.01);

  // when ray dosent hit anything dist is +INFINITY, valid for lights.
  if (light_hit.dist * light_hit.dist >= light_len) {
    double weight = std::max(0.15f, light_dir * normal);

    return weight;
  }
  return 0.0;
}
