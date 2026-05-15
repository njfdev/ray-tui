#include "entity_manager.hpp"
#include "component.hpp"
#include "system.hpp"
#include <array>
#include <list>
#include <map>
#include <stdexcept>
#include <tuple>

EntityManager::EntityManager() {
    entities = std::map<int, std::list<Component*>>();
}

int EntityManager::createEntity() {
    int id = nextEntityId++;

    entities.insert({id, std::list<Component*>()});

    return id;
}

int EntityManager::createEntity(std::list<Component*> comps) {
    int id = createEntity();

    entities[id] = comps;

    return id;
}

void EntityManager::addComponent(int entityId, Component* comp) {
    entities[entityId].push_back(comp);
}

int EntityManager::addSystem(System* system) {
    int id = nextSystemId++;

    systems.insert({id, system});

    return id;
}

std::list<int> EntityManager::getEntityIdsWithComponents(std::list<int> componentIds) {
  std::list<int> applicableEntityIds{};

  for (auto [entityId, entity] : entities) {
      std::list<int> requiredCompIds = componentIds;
      for (Component* comp : entity) {
          for (auto it = requiredCompIds.begin(); it != requiredCompIds.end(); it++) {
              if (component_id<typeof *comp>() == *it) {
                  requiredCompIds.erase(it);
                  break;
              }
          }

          if (requiredCompIds.empty()) {
              applicableEntityIds.push_back(entityId);
              break;
          }
      }
  }

  return applicableEntityIds;
}

// separate this into multiple functions
void EntityManager::update() {
    for (auto [systemId, system] : systems) {
        // find all entites with components of interest
        std::list<int> applicableEntityIds = getEntityIdsWithComponents(system->requiredComponents());

        system->update(applicableEntityIds);
    }
}
