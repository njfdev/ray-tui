#pragma once

#include <vector>

class EntityManager;

/**
 * When added to the ECS, a system will be called on each entity
 * with the components the system specifies. Essentially, it is
 * code for entities.
 */
class System {
public:
    /**
     * Returns all the component type IDs that an entity must have
     * for this system to apply to it.
     *
     * @return a vector of required component type IDs
     */
    virtual std::vector<int> requiredComponents() { return {}; };

    /**
     * Called once when the system is created.
     *
     * @param entityManager pointer to the EntityManager calling this system
     */
    virtual void init(EntityManager* entityManager) {};
    /**
     * Called every update loop. All the applicable entities are passed
     * in by argument by their IDs.
     *
     * A system can use the entity ID to get a pointer to the entity
     * with the entity manager.
     *
     * @param dt time since the last update
     * @param entityManager pointer to the EntityManager calling this system
     * @param entityIds all entity IDs that this system applies to
     */
    virtual void update(double dt, EntityManager* entityManager, std::vector<int> entityIds) {};
    /**
     * Called once before the system is destroyed.
     *
     * @param entityManager pointer to the EntityManager calling this system
     */
    virtual void cleanup(EntityManager* entityManager) {};

};
