#pragma once

#include <memory>
#include <string>
#include <SDL3/SDL.h>

#include "Common/Gameplay/GameState.h"
#include "Common/Rendering/RenderManager.h"
#include "Game/REDGameConstant.h"

class GameCommon
{
public:
    GameCommon()
    {
        if (window = SDL_CreateWindow("RED", 1920, 1080, 0), !window)
        {
            SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
            return;
        }
        base_path = std::string(SDL_GetBasePath());
        RenderManager::Init();

        initialized = true;
    }

    virtual ~GameCommon()
    {
        game_state = nullptr;
        RenderManager::Destroy();
        SDL_DestroyWindow(window);
    }

    static void* LoadAsset(std::string file_name, size_t* data_size);
    static void Draw(SDL_GPUCommandBuffer* cmd_buf, SDL_GPUTexture* swapchain_texture);

    void ChangeGameState(SCENE_ID in_scene_id);

    bool initialized = false;

    Uint64 last_step{};
    SCENE_ID scene_id = SCENE_ID_INVALID;
    double frame_rate = 60.0;

    static inline SDL_Window* window;

private:
    static inline std::string base_path;
    static inline std::unique_ptr<GameState> game_state;
};
