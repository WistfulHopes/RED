#pragma once
#include <string>
#include <unordered_map>
#include "Actor.h"
#include "AALib/Common/tl_Filepack.h"

class LazyTexture;
class OBJ_CBase;

class Actor2D : Actor
{
private:
    AA_Filepack_FPAC img_pac{};
    char cur_sprite_name[0x20]{};
    std::unordered_map<std::string, LazyTexture*> textures{};

public:
    Actor2D() = default;
    virtual ~Actor2D();

    virtual void LoadSprites() = 0;
    bool SetSprite(const char* img_name);
};
