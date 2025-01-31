#pragma once

#include <vector>
#include "Component.h"

struct SDL_GPUBuffer;

class Actor
{
protected:
    std::vector<Component*> components;

    struct SDL_GPUGraphicsPipeline* pipeline;
    SDL_GPUBuffer* vertex_buffer;
    SDL_GPUBuffer* index_buffer;

public:
    Actor() = default;
    virtual ~Actor() = default;

    virtual void Draw() = 0;
};
