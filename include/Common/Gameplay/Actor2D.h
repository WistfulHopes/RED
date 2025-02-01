#pragma once
#include <string>
#include <unordered_map>
#include "Actor.h"
#include "AALib/Common/tl_Filepack.h"
#include "Common/Rendering/RenderManager.h"

class LazyTexture;
struct SDL_GPUShader;

class Actor2D : public Actor
{
protected:
    AA_Filepack_FPAC img_pac{};
    char cur_sprite_name[0x20]{};
    std::unordered_map<std::string, LazyTexture*> textures{};

public:
    Actor2D();
    ~Actor2D() override;

    virtual void LoadSprites() = 0;
    virtual PositionTextureVertex* GetCoords() = 0;
    
    bool SetSprite(const char* img_name);

    void Draw(SDL_GPUCommandBuffer* cmd_buf, SDL_GPUTexture* swapchain_texture, SDL_GPURenderPass* render_pass) override;

private:
    bool InitGPU();
    
    SDL_GPUTransferBuffer* buffer_transfer_buffer;
    SDL_GPUSampler* sampler;
};
