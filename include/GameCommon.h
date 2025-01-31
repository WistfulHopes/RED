#pragma once

#include <string>
#include <SDL3/SDL.h>

#include "Common/Rendering/RenderManager.h"
#include "Game/REDGameConstant.h"

class GameCommon
{
public:
    GameCommon()
    {
        if (!SDL_CreateWindowAndRenderer("RED", 1920, 1080, 0, &window, &renderer))
        {
            SDL_Log("SDL_CreateWindowAndRenderer failed: %s", SDL_GetError());
            return;
        }
        base_path = std::string(SDL_GetBasePath());
        RenderManager::Init();

        initialized = true;
    }
    virtual ~GameCommon()
    {
        RenderManager::Destroy();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
    }

    void Update()
    {
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    static void* LoadAsset(std::string file_name, size_t* data_size);

    bool initialized = false;
    
    Uint64 last_step{};
    SCENE_ID scene_id{};
    double frame_rate = 60.0;
    
    static inline SDL_Window* window;
    static inline SDL_Renderer* renderer;

private:
    static inline std::string base_path;
};
