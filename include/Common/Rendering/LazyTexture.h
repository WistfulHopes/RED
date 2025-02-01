#pragma once
#include <string>
#include <SDL3/SDL_gpu.h>

class LazyTexture
{
public:
    LazyTexture();

    ~LazyTexture();

    bool Load(const std::string& filename, SDL_GPUTextureFormat format);
    bool Load(SDL_IOStream* mem, SDL_GPUTextureFormat format);
    void Destroy();

    [[nodiscard]] int GetWidth() const;
    [[nodiscard]] int GetHeight() const;
    [[nodiscard]] SDL_Surface* GetSurface() const;
    [[nodiscard]] SDL_GPUTexture* GetTexture() const;
    
private:
    SDL_Surface* surface{};
    SDL_GPUTexture* texture{};

    int width;
    int height;
};
