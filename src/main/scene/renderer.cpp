#include "renderer.hpp"
#include "scene/components/position.hpp"
#include "scene/components/renderable.hpp"

std::list<int> Renderer::requiredComponents() {
  return { component_id<Position>(), component_id<Renderable>() };
}

void Renderer::init() {

}

void Renderer::update(std::list<int> entityIds) {

}

void Renderer::cleanup() {

}
