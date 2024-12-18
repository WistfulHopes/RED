#include "Common/Gameplay/Actor2D.h"
#include <SDL3/SDL.h>
#include <format>
#include <ranges>

#include "LazyTexture.h"

Actor2D::~Actor2D()
{
    for (const auto val : textures | std::views::values)
    {
        delete val;
    }
}

bool Actor2D::SetSprite(const char* img_name)
{
    if (!strncmp(img_name, cur_sprite_name, 0x20)) return true;
    if (textures.contains(img_name)) return true;

    auto idx = img_pac.SearchFileID(img_name);

    if (static_cast<int>(idx) == -1) return false;

    strcpy(cur_sprite_name, img_name);

    const auto offset = img_pac.GetPackOffsetAddr(idx);
    const auto size = img_pac.GetPackOffsetFileSize(idx);

    const auto stream = SDL_IOFromMem(offset, size);
    const auto texture = new LazyTexture();
    texture->Load(stream);

    textures.insert({ img_name, texture });

    return true;
}
