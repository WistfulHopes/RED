#include "Common/Gameplay/GameState.h"

#include <SDL3/SDL_gpu.h>

#include "AALib/AASystemRED.h"
#include "AALib/Base/sys_Math.h"
#include "Common/Gameplay/Actor.h"

GameState::~GameState()
{
    for (auto actor : actors)
    {
        delete actor;
    }
}

void GameState::Init(SCENE_CBase* in_scene)
{    
    scene = in_scene;
}

void GameState::Draw(SDL_GPUCommandBuffer* cmd_buf, SDL_GPUTexture* swapchain_texture) const
{
    auto camera = AASystemRED::GetInstance()->GetCameraManager().Get(0);
    AA_Matrix view_proj_matrix = camera->GetViewProjMatrix();
    
    SDL_GPUColorTargetInfo color_target_info = {nullptr};
    color_target_info.texture = swapchain_texture;
    color_target_info.clear_color = {1.0f, 1.0f, 1.0f, 1.0f};
    color_target_info.load_op = SDL_GPU_LOADOP_CLEAR;
    color_target_info.store_op = SDL_GPU_STOREOP_STORE;

    SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(cmd_buf, &color_target_info, 1, NULL);

    SDL_PushGPUVertexUniformData(cmd_buf, 0, &view_proj_matrix, sizeof(AA_Matrix));

    for (auto actor : actors)
    {
        actor->Draw(cmd_buf, swapchain_texture, render_pass);
    }
    
    SDL_EndGPURenderPass(render_pass);
}
