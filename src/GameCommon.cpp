#include "GameCommon.h"

#define SHADER_SUPPORTED_FORMATS (SDL_GPU_SHADERFORMAT_MSL | SDL_GPU_SHADERFORMAT_DXBC | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_SPIRV)

#define CHECK_CREATE(var, thing) { if (!(var)) { SDL_Log("Failed to create %s: %s\n", thing, SDL_GetError()); } }

void GameCommon::InitRenderState()
{
    gpu_device = SDL_CreateGPUDevice(SHADER_SUPPORTED_FORMATS, false, SDL_GetHint(SDL_HINT_GPU_DRIVER));
    CHECK_CREATE(gpu_device, "GPU device")

    SDL_ClaimWindowForGPUDevice(gpu_device, window);
}
