﻿#include <format>
#include "Common/Rendering/LazyTexture.h"
#include "GameCommon.h"
#include "../SDL_image/include/SDL3_image/SDL_image.h"
#include "SDL3/SDL_gpu.h"

LazyTexture::LazyTexture() : width(0), height(0)
{
}

LazyTexture::~LazyTexture()
{
    Destroy();
}

bool LazyTexture::Load(const std::string& filename, SDL_GPUTextureFormat format)
{
    Destroy();

    if (surface = IMG_Load(filename.c_str()); surface == nullptr)
    {
        SDL_Log(std::format("Unable to load image {}: {}", filename, SDL_GetError()).c_str());
    }
    else
    {
        SDL_GPUTextureCreateInfo create_info = {
            .type = SDL_GPU_TEXTURETYPE_2D,
            .format = format,
            .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER,
            .width = static_cast<uint32_t>(surface->w),
            .height = static_cast<uint32_t>(surface->h),
            .layer_count_or_depth = 1,
            .num_levels = 1,
        };
        if (texture = RenderManager::CreateTexture(create_info); texture == nullptr)
        {
            SDL_Log(std::format("Unable to create texture {} from loaded pixels: {}", filename,
                                SDL_GetError()).c_str());
        }
        else
        {
            width = surface->w;
            height = surface->h;
        }
    }

    return texture != nullptr;
}

bool LazyTexture::Load(SDL_IOStream* mem, SDL_GPUTextureFormat format)
{
    Destroy();

    if (surface = IMG_LoadTyped_IO(mem, false, nullptr); surface == nullptr)
    {
        SDL_Log(std::format("Unable to load image: {}", SDL_GetError()).c_str());
    }
    else
    {
        SDL_GPUTextureCreateInfo create_info = {
            .type = SDL_GPU_TEXTURETYPE_2D,
            .format = format,
            .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER,
            .width = static_cast<uint32_t>(surface->w),
            .height = static_cast<uint32_t>(surface->h),
            .layer_count_or_depth = 1,
            .num_levels = 1,
        };
        if (texture = RenderManager::CreateTexture(create_info); texture == nullptr)
        {
            SDL_Log(std::format("Unable to create texture from loaded pixels: {}", SDL_GetError()).c_str());
        }
        else
        {
            width = surface->w;
            height = surface->h;
        }
    }

    return texture != nullptr;
}

void LazyTexture::Destroy()
{
    RenderManager::ReleaseTexture(texture);
    SDL_DestroySurface(surface);
    surface = nullptr;
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

SDL_GPUTexture* LazyTexture::GetTexture() const
{
    return texture;
}

SDL_Surface* LazyTexture::GetSurface() const
{
    return surface;
}
