#pragma once

#include "component.hpp"
#include "system.hpp"
#include <list>
#include <map>

const uint32_t MAX_ENTITIES = 100;

class EntityManager {
public:
    EntityManager();

    int createEntity();

    void addComponent(int entityId, Component comp);
    template<typename C>
    C* getComponent(int entityId);

    int addSystem(System system);

    void update();

private:
    int nextEntityId = 0;
    int nextSystemId = 0;
    std::map<int, std::list<Component>> entities;
    std::map<int, System> systems;

};
