#pragma once

#include <vector>
#include "Component.h"
#include "Game/REDGameCommon.h"
#include "Game/Scene/scene_Battle.h"

struct SDL_GPUBuffer;

inline void GamePos2SDLPos(const int32_t& x, const int32_t& y, const int32_t& z, AA_Vector3& v)
{
    if (const auto scene = dynamic_cast<SCENE_CBattle*>(REDGameCommon::GetInstance()->GetScene()))
    {
        const auto linkMagn = scene->GetBattleScreenManager()->GetLinkMagn();
        v.X = (float)x * 0.001f * linkMagn;
        v.Y = (float)y * 0.001f * linkMagn;
        v.Z = (float)z * 0.001f * linkMagn;
    }
}

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

    virtual void Draw(struct SDL_GPUCommandBuffer* cmd_buf, struct SDL_GPUTexture* swapchain_texture, struct SDL_GPURenderPass* render_pass) = 0;
};
