#pragma once

#include "component.hpp"
#include "system.hpp"
#include <cstdint>
#include <stdexcept>
#include <map>
#include <vector>

const uint32_t MAX_ENTITIES = 100;

/**
 * Class to handle the creation and storage of entities, as
 * well as calling systems for each entity.
 */
class EntityManager {
public:
    /**
     * Creates an EntityManager containing no entities.
     *
     * @return new EntityManager instance
     */
    EntityManager();
    /**
     * Cleans up the ECS environment.
     */
    ~EntityManager();

    /**
     * Create an entity object with no components and get
     * the ID of that entity.
     *
     * @return newly created entity ID
     */
    int createEntity();
    /**
     * Create an entity object, initialize it with a list of
     * components, and get the ID of that entity.
     *
     * @return newly created entity ID
     */
    int createEntity(std::vector<Component*> comps);

    /**
     * Add a single component to a pre-existing entity by
     * its ID.
     *
     * @param entityId ID of the entity to modify
     * @param comp pointer to the component to add
     */
    void addComponent(int entityId, Component* comp);

    /**
     * Helper function to get a vector of all the entity IDs where
     * the entities have all of the component IDs requested.
     *
     * @param componentIds which component IDs the entities must contain
     * @return a vector of all the applicable entity IDs
     */
    std::vector<int> getEntityIdsWithComponents(std::vector<int> componentIds);

    /**
     * Add a system that will be called on every applicable entity
     * each update loop.
     *
     * @param system which system to add
     * @return ID of the system in the ECS
     */
    int addSystem(System* system);

    /**
     * Runs all the systems in the ECS against all applicable entities.
     *
     * Should be called each update loop with the current delta time.
     *
     * @param dt delta time since the last update
     */
    void update(double dt);

    /**
     * Get a component of a specific type on a particular entity.
     *
     * An error will be thrown if the entity doesn't contain the
     * component type.
     *
     * @tparam C type of the component to retrieve
     * @param entityID ID of the entity to look at
     * @return pointer to the desired component
     */
    template<typename C>
    C* getComponent(int entityId) {
        std::vector<Component*> entity = entities[entityId];
        for (Component* comp : entity) {
            if (comp->id() == component_id<C>()) {
                return static_cast<C*>(comp);
            }
        }

        throw std::runtime_error(std::string("Entity with components does not contain desired component ") + typeid(C).name());
        // O(n)
    }

private:
    int nextEntityId = 0;
    int nextSystemId = 0;
    std::map<int, std::vector<Component*>> entities;
    std::map<int, System*> systems;

};
