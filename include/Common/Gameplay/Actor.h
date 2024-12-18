#pragma once

#include <vector>
#include "Component.h"

class Actor
{
private:
    std::vector<Component*> components;
    
public:
    Actor() = default;
    virtual ~Actor() = default;
    
    virtual void Draw() = 0;
};
