#pragma once

#include "component.hpp"
#include "system.hpp"
#include <iostream>
#include <list>
#include <map>

const uint32_t MAX_ENTITIES = 100;

class EntityManager {
public:
    EntityManager();

    int createEntity();
    int createEntity(std::list<Component*> comps);

    void addComponent(int entityId, Component* comp);

    std::list<int> getEntityIdsWithComponents(std::list<int> componentIds);

    int addSystem(System* system);

    void update();

    template<typename C>
    C* getComponent(int entityId) {
        std::list<Component*> entity = entities[entityId];
        for (Component* comp : entity) {
            if (component_id<typeof *comp>() == component_id<C>()) {
                return reinterpret_cast<C*>(&comp);
            }
        }

        throw std::runtime_error(std::string("Entity with components does not contain desired component ") + typeid(C).name());
    }

private:
    int nextEntityId = 0;
    int nextSystemId = 0;
    std::map<int, std::list<Component*>> entities;
    std::map<int, System*> systems;

};
