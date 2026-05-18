#include "entity_manager.hpp"
#include "component.hpp"
#include "system.hpp"
#include <map>
#include <vector>

EntityManager::EntityManager() {
  entities = std::map<int, std::vector<Component *>>();
}

EntityManager::~EntityManager() {
  for (const auto& [id, sys] : systems) {
    sys->cleanup(this);
  }
}

int EntityManager::createEntity() {
  int id = nextEntityId++;

  entities.insert({id, std::vector<Component *>()});

  return id;
}

int EntityManager::createEntity(std::vector<Component *> comps) {
  int id = createEntity();

  entities[id] = comps;

  return id;
}

void EntityManager::addComponent(int entityId, Component *comp) {
  entities[entityId].push_back(comp);
}

int EntityManager::addSystem(System *system) {
  int id = nextSystemId++;

  systems.insert({id, system});

  system->init(this);

  return id;
}

std::vector<int> EntityManager::getEntityIdsWithComponents(std::vector<int> componentIds) {
  std::vector<int> applicableEntityIds{};

  for (auto [entityId, entity] : entities) {
    // check if this entity contains all the required components
    bool found_all = true;
    for (int req_comp_id : componentIds) {
      bool found = false;
      for (Component *comp : entity) {
        if (comp->id() == req_comp_id) {
          found = true;
          break;
        }
      }

      if (!found) {
        found_all = false;
        break;
      }
    }

    if (found_all) {
      applicableEntityIds.push_back(entityId);
    }
  }

  return applicableEntityIds;
  // O(n^3)
}

void EntityManager::update(double dt) {
  for (int i = 0; i < systems.size(); i++) {
    // find all entites with components of interest
    std::vector<int> applicableEntityIds =
        getEntityIdsWithComponents(systems[i]->requiredComponents());

    // apply the system to those entities
    systems[i]->update(dt, this, applicableEntityIds);
  }

  // O(n^4)
}
