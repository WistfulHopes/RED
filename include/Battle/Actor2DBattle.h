#pragma once
#include "Common/Gameplay/Actor2D.h"

class OBJ_CBase;

class Actor2DBattle : public Actor2D
{
public:
    Actor2DBattle(OBJ_CBase* in_obj);
    
    OBJ_CBase* obj;

    void LoadSprites() override;
    void Draw(SDL_GPUCommandBuffer* cmd_buf, SDL_GPUTexture* swapchain_texture, SDL_GPURenderPass* render_pass) override;
    PositionTextureVertex* GetCoords() override;
};
