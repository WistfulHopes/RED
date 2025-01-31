#pragma once
#include <string>
#include <unordered_map>
#include "Actor.h"
#include "AALib/Common/tl_Filepack.h"
#include "Common/Rendering/RenderManager.h"

class LazyTexture;
struct SDL_GPUShader;

class Actor2D : Actor
{
protected:
    AA_Filepack_FPAC img_pac{};
    char cur_sprite_name[0x20]{};
    std::unordered_map<std::string, LazyTexture*> textures{};
    struct SDL_GPUSampler* sampler;

public:
    Actor2D() = default;
    virtual ~Actor2D() override;

    virtual void LoadSprites() = 0;
    virtual PositionTextureVertex* GetCoords() = 0;

    bool SetSprite(const char* img_name);

    void Draw() override;

private:
    bool InitGPU();
};
