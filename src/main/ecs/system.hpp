#pragma once

#include <list>
class System {
public:
    System();
    ~System();

    virtual std::list<int> requiredComponents() { return {}; };

    virtual void init() {};
    virtual void update(std::list<int> entityIds) {};
    virtual void cleanup() {};

};
