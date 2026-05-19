#include "scene/scene.hpp"
#include "ecs/entity_manager.hpp"
#include "math/intersection.hpp"
#include "math/vec3.hpp"
#include "render/bvh.hpp"
#include "render/framebuffer.hpp"
#include "render/render_data.hpp"
#include "scene/components/position.hpp"
#include "scene/components/renderable.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <variant>

static Vec3 reflectDir(Vec3 d, Vec3 n) { return d - 2.0f * (d * n) * n; }

static Vec3 refractDir(Vec3 d, Vec3 n, float eta) {
  float cos_i = -(d * n);
  float sin2_t = eta * eta * (1.0f - cos_i * cos_i);
  if (sin2_t >= 1.0f)
    return Vec3{};
  float cos_t = std::sqrt(1.0f - sin2_t);
  return eta * d + (eta * cos_i - cos_t) * n;
}

Scene::Scene(EntityManager *entityManager) {
  this->entityManager = entityManager;
}

Intersection Scene::trace(Ray &ray, double min_dist) {
  return bvh.intersect(ray);
}

Color Scene::castRay(Ray ray, int depth) {
  Intersection hit = trace(ray, 1e-5);
  if (!hit.valid())
    return background;

  const Material &mat = hit.obj->appearance.mat;

  // direct phong lighting
  Color direct{};
  for (int entityId : entityManager->getEntityIdsWithComponents(
           {component_id<Position>(), component_id<Renderable>()})) {
    Renderable *appearance = entityManager->getComponent<Renderable>(entityId);
    if (!std::holds_alternative<EmptyGeometry>(appearance->geometry))
      continue;
    Position *origin = entityManager->getComponent<Position>(entityId);
    double phong = traceLight(hit.p, hit.normal, origin->pos);
    if (phong != 0)
      direct = direct + phong * mat.diffuse * appearance->mat.color * mat.color;
  }

  // specular (reflection)
  Color reflect_color{};
  if (mat.specular > 0.0 && depth > 0) {
    Vec3 r = reflectDir(ray.direction, hit.normal);
    Ray reflect_ray{r, hit.p + hit.normal * 1e-4f};
    reflect_color = castRay(reflect_ray, depth - 1) * mat.specular;
  }

  Color surface = direct + reflect_color;

  if (mat.opacity >= 1.0 || depth <= 0)
    return surface;

  // flip normal + eta depending on if inside or outside
  bool inside = (ray.direction * hit.normal) > 0.0f;
  Vec3 n = inside ? hit.normal * -1.0f : hit.normal;
  float eta = inside ? (float)mat.refractIdx : (1.0f / (float)mat.refractIdx);

  Color transmit{};
  if (mat.refract > 0.0) {
    Vec3 t = refractDir(ray.direction, n, eta);
    if (t.sqrLength() > 0.0f) {
      Ray refract_ray{t, hit.p + n * -1e-4f};
      transmit = castRay(refract_ray, depth - 1) * mat.refract;
    } else {
      Vec3 r = reflectDir(ray.direction, n);
      Ray tir_ray{r, hit.p + n * 1e-4f};
      transmit = castRay(tir_ray, depth - 1);
    }
  } else {
    // transparent, ray goes straight through.
    Ray straight{ray.direction, hit.p + ray.direction * 1e-4f};
    transmit = castRay(straight, depth - 1);
  }

  return surface * mat.opacity + transmit * (1.0 - mat.opacity);
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

      double ndcX = (2.0 * (ix + 0.5) / w) - 1.0 + randX;
      double ndcY = 1.0 - (2.0 * (iy + 0.5) / h) + randY;

      Vec3 dir = (fwd.direction + right * (ndcX * tanH) + up * (ndcY * tanV))
                     .normalize();

      Ray ray{dir, fwd.origin};

      Color color = castRay(ray, bounceConfig.reflect);
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

double Scene::traceLight(Vec3 p, Vec3 normal, Vec3 lightOrigin) {
  Vec3 light_offset = lightOrigin - p;
  double light_len = light_offset.sqrLength();
  Vec3 light_dir = light_offset.normalize();
  Ray light_ray = Ray{light_dir, p + light_dir * 0.01};
  Intersection light_hit = trace(light_ray, 0.01);

  // when ray dosent hit anything dist is +INFINITY, valid for lights.
  if (light_hit.dist * light_hit.dist >= light_len) {
    double weight = std::max(0.15f, light_dir * normal);
    return weight;
  }
  return 0.05;
}

void Scene::construct() {
  for (int entityId : entityManager->getEntityIdsWithComponents(
           {component_id<Position>(), component_id<Renderable>()})) {
    Position *origin = entityManager->getComponent<Position>(entityId);
    Renderable *appearance = entityManager->getComponent<Renderable>(entityId);

    if (std::holds_alternative<EmptyGeometry>(appearance->geometry)) {
      continue;
    }

    bvh.insert(*origin, *appearance);
  }

  bvh.construct();
}
