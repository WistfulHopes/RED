#pragma once
#include <string>
#include <SDL3/SDL_render.h>

class LazyTexture
{
public:
    LazyTexture();

    ~LazyTexture();

    bool Load(std::string filename);
    bool Load(SDL_IOStream* mem);
    void Destroy();

    int GetWidth() const;
    int GetHeight() const;

private:
    SDL_Texture* texture;

    int width;
    int height;
};
