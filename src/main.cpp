#define SDL_MAIN_USE_CALLBACKS 1
#include <format>
#include <string>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "GameCommon.h"
#include "Game/REDGameCommon.h"

namespace
{
    void LoadCharaData(std::string chara_id, int idx)
    {
        size_t bbs_size = 0;
        auto bbs_data = (uint8_t*)GameCommon::LoadAsset(std::format("assets/BBS_{}.bbsbin", chara_id), &bbs_size);
    
        size_t ef_bbs_size = 0;
        auto ef_bbs_data = (uint8_t*)GameCommon::LoadAsset(std::format("assets/BBS_{}EF.bbsbin", chara_id), &ef_bbs_size);
    
        auto col_data = (uint8_t*)GameCommon::LoadAsset(std::format("assets/COL_{}.pac", chara_id), nullptr);

        REDGameCommon::GetInstance()->LoadCharaData(bbs_data, bbs_size, ef_bbs_data, ef_bbs_size, col_data, idx);
    }
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    SDL_SetAppMetadata("RED", "0.1.0", "com.wistfulhopes.red");

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    auto game_common = new GameCommon();
    if (!game_common || !game_common->initialized) return SDL_APP_FAILURE;
    *appstate = game_common;
    
    LoadCharaData("SOL", 0);
    LoadCharaData("DMY", 1);
    LoadCharaData("DMY", 2);
    LoadCharaData("SOL", 3);
    LoadCharaData("DMY", 4);
    LoadCharaData("DMY", 5);
    LoadCharaData("CMN", 6);
    
    game_common->last_step = SDL_GetTicks();
    
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    if (event->type == SDL_EVENT_QUIT) return SDL_APP_SUCCESS;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
    const auto game_common = (GameCommon*)appstate;

    const Uint64 now = SDL_GetTicks();
    const auto frame_rate = Uint64(1 / game_common->frame_rate * 1000.);
    
    while (now - game_common->last_step >= frame_rate)
    {
        REDGameCommon::GetInstance()->Tick((float)now / 1000.f);
        game_common->last_step += frame_rate;
    }

    if (game_common->scene_id != REDGameCommon::GetInstance()->GetSceneID())
    {
        game_common->scene_id = REDGameCommon::GetInstance()->GetSceneID();
    }

    game_common->Update();
    
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    if (appstate)
    {
        auto game_common = (GameCommon*)appstate;
        delete game_common;
    }
}