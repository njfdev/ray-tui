#pragma once

#include <vector>
class System {
public:
    System();
    ~System();

    virtual std::vector<int> requiredComponents() { return {}; };

    virtual void init() {};
    virtual void update(std::vector<int> entityIds) {};
    virtual void cleanup() {};

};
