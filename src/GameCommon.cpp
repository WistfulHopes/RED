#include "GameCommon.h"

#include "Battle/BattleGameState.h"
#include "Common/Rendering/RenderManager.h"
#include "Game/REDGameCommon.h"

void* GameCommon::LoadAsset(std::string file_name, size_t* data_size)
{
    return SDL_LoadFile((base_path + file_name).c_str(), data_size);
}

void GameCommon::ChangeGameState(SCENE_ID in_scene_id)
{
    scene_id = in_scene_id;
    
    switch (in_scene_id)
    {
    case SCENE_ID_BATTLE:
        game_state = std::make_unique<BattleGameState>();
        break;
    default:
        game_state = std::make_unique<GameState>();
        break;
    }
    
    game_state->Init(REDGameCommon::GetInstance()->GetScene());
}

void GameCommon::Draw(SDL_GPUCommandBuffer* cmd_buf, SDL_GPUTexture* swapchain_texture)
{
    game_state->Draw(cmd_buf, swapchain_texture);
}
