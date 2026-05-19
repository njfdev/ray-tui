#pragma once

#include "ecs/entity_manager.hpp"
#include "math/intersection.hpp"
#include "math/ray.hpp"
#include "render/bvh.hpp"
#include "render/color.hpp"
#include "render/framebuffer.hpp"

/**
 * Config for number of bounces to do by type.
 */
struct BounceConfig {
  int reflect;
  int diffuse;
  int nDiffuse;
};

/**
 * Holds information about a visual scene, allows rendering of a scene to a
 * framebuffer.
 */
class Scene {
public:
  /**
   * the entity manager for the scene. Objects added to the scene should be
   * inserted to the entity manager.
   */
  EntityManager *entityManager;
  Color background;

  // horizontal FOV of the camera
  double fovh;
  int cur_frame;

  Scene(EntityManager *entityManager);

  /**
   * Constructs the scene, constructing the BVH in the process
   * Should be called after all entities have been added before alling
   * render(...)
   */
  void construct();

  /**
   * Renders the scene to the framebuffer at a specific camera position
   *
   * @param camera a ray giving the position and forward vector of the camera.
   * Up vector is assumed to lie between forward vector and z axis.
   */
  void render(Framebuffer *fb, Ray camera);

  /**
   * The BVH for the scene. entities added to the entittymanager will be
   * inserted here during {construct}
   */
  BVH bvh = BVH();

  /**
   * The bounce config for this scene.
   */
  BounceConfig bounceConfig = {2, 1, 0};

private:
  Intersection trace(Ray &ray, double min_dist);
  double traceLight(Vec3 p, Vec3 normal, Vec3 lightOrigin);
  Color castRay(Ray ray, int depth);
};
