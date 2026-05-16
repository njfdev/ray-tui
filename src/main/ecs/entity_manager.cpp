#include "entity_manager.hpp"
#include "component.hpp"
#include "system.hpp"
#include <iostream>
#include <map>
#include <vector>

EntityManager::EntityManager() {
  entities = std::map<int, std::vector<Component *>>();
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

  return id;
}

std::vector<int>
EntityManager::getEntityIdsWithComponents(std::vector<int> componentIds) {
  std::vector<int> applicableEntityIds{};

  for (auto [entityId, entity] : entities) {
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
}

// separate this into multiple functions
void EntityManager::update() {
  for (auto [systemId, system] : systems) {
    // find all entites with components of interest
    std::vector<int> applicableEntityIds =
        getEntityIdsWithComponents(system->requiredComponents());

    system->update(applicableEntityIds);
  }
}
