#include <format>
#include "Common/Rendering/LazyTexture.h"
#include "GameCommon.h"
#include "../SDL_image/include/SDL3_image/SDL_image.h"

LazyTexture::LazyTexture() : texture(nullptr), width(0), height(0)
{
}

LazyTexture::~LazyTexture()
{
    Destroy();
}

bool LazyTexture::Load(std::string filename)
{
    Destroy();

    if (auto loaded_surface = IMG_Load(filename.c_str()); loaded_surface == nullptr)
    {
        SDL_Log(std::format("Unable to load image {}: {}", filename, SDL_GetError()).c_str());
    }
    else
    {
        if (texture = SDL_CreateTextureFromSurface(GameCommon::renderer, loaded_surface); texture == nullptr)
        {
            SDL_Log(std::format("Unable to create texture {} from loaded pixels: {}", filename,
                                SDL_GetError()).c_str());
        }
        else
        {
            width = loaded_surface->w;
            height = loaded_surface->h;
        }

        SDL_DestroySurface(loaded_surface);
    }

    return texture != nullptr;
}

bool LazyTexture::Load(SDL_IOStream* mem)
{
    Destroy();

    if (auto loaded_surface = IMG_LoadTyped_IO(mem, false, nullptr); loaded_surface == nullptr)
    {
        SDL_Log(std::format("Unable to load image: {}", SDL_GetError()).c_str());
    }
    else
    {
        if (texture = SDL_CreateTextureFromSurface(GameCommon::renderer, loaded_surface); texture == nullptr)
        {
            SDL_Log(std::format("Unable to create texture from loaded pixels: {}", SDL_GetError()).c_str());
        }
        else
        {
            width = loaded_surface->w;
            height = loaded_surface->h;
        }

        SDL_DestroySurface(loaded_surface);
    }

    return texture != nullptr;
}

void LazyTexture::Destroy()
{
    SDL_DestroyTexture(texture);
    texture = nullptr;
    width = 0;
    height = 0;
}

int LazyTexture::GetWidth() const
{
    return width;
}

int LazyTexture::GetHeight() const
{
    return height;
}
