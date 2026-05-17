#pragma once

#include "component.hpp"
#include "system.hpp"
#include <cstdint>
#include <stdexcept>
#include <map>
#include <vector>

const uint32_t MAX_ENTITIES = 100;

class EntityManager {
public:
    EntityManager();

    int createEntity();
    int createEntity(std::vector<Component*> comps);

    void addComponent(int entityId, Component* comp);

    std::vector<int> getEntityIdsWithComponents(std::vector<int> componentIds);

    int addSystem(System* system);

    void update();

    template<typename C>
    C* getComponent(int entityId) {
        std::vector<Component*> entity = entities[entityId];
        for (Component* comp : entity) {
            if (comp->id() == component_id<C>()) {
                return static_cast<C*>(comp);
            }
        }

        throw std::runtime_error(std::string("Entity with components does not contain desired component ") + typeid(C).name());
    }

private:
    int nextEntityId = 0;
    int nextSystemId = 0;
    std::map<int, std::vector<Component*>> entities;
    std::map<int, System*> systems;

};
