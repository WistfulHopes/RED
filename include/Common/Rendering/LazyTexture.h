#pragma once
#include <string>

struct SDL_GPUTexture;
struct SDL_Surface;

class LazyTexture
{
public:
    LazyTexture();

    ~LazyTexture();

    bool Load(const std::string& filename);
    bool Load(struct SDL_IOStream* mem);
    void Destroy();

    int GetWidth() const;
    int GetHeight() const;
    SDL_Surface* GetSurface() const;
    SDL_GPUTexture* GetTexture() const;
    
private:
    SDL_Surface* surface;
    SDL_GPUTexture* texture;

    int width;
    int height;
};
