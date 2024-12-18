#pragma once

#include <SDL3/SDL.h>

#include "Game/REDGameConstant.h"

class GameCommon
{
public:
    GameCommon()
    {
        window = nullptr;
        renderer = nullptr;
    }
    virtual ~GameCommon()
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
    }

    void InitRenderState();
    
    static SDL_Window* window;
    static SDL_Renderer* renderer;
    static SDL_GPUDevice* gpu_device;
    SDL_
    Uint64 last_step{};
    SCENE_ID scene_id{};
    double frame_rate = 60.0;
};
