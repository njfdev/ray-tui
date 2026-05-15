#include "entity_manager.hpp"
#include "component.hpp"
#include "system.hpp"
#include <array>
#include <list>
#include <map>
#include <stdexcept>
#include <tuple>

EntityManager::EntityManager() {
    entities = std::map<int, std::list<Component>>();
}

int EntityManager::createEntity() {
    int id = nextEntityId++;

    entities.insert({id, std::list<Component>()});

    return id;
}

void EntityManager::addComponent(int entityId, Component comp) {
    entities[entityId].push_back(comp);
}

template<typename C>
C* EntityManager::getComponent(int entityId) {
    auto entity = entities[entityId];
    for (Component comp : entity) {
        if (component_id<typeof comp>() == component_id<C>()) {
            return comp;
        }
    }

    throw std::runtime_error("Entity does not contain desired component");
}

int EntityManager::addSystem(System system) {
    int id = nextSystemId++;

    systems.insert({id, system});

    return id;
}

// separate this into multiple functions
void EntityManager::update() {
    for (auto [systemId, system] : systems) {
        // find all entites with components of interest
        std::list<int> applicableEntityIds{};

        for (auto [entityId, entity] : entities) {
            std::list<int> requiredCompIds = system.requiredComponents();
            for (auto comp : entity) {
                for (auto it = requiredCompIds.begin(); it != requiredCompIds.end(); it++) {
                    if (component_id<typeof comp>() == *it) {
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

        system.update(applicableEntityIds);
    }
}
