#pragma once
#include <string>
#include <SDL3/SDL_gpu.h>

#define SHADER_SUPPORTED_FORMATS (SDL_GPU_SHADERFORMAT_MSL | SDL_GPU_SHADERFORMAT_DXBC | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_SPIRV)

struct PositionTextureVertex
{
    float x, y, z;
    float u, v;
};

class RenderManager
{
public:
    static void Init();
    static void Destroy();

    static SDL_GPUShader* CreateShader(const std::string& shader_filename, uint32_t sampler_count,
                                       uint32_t uniform_buffer_count,
                                       uint32_t storage_buffer_count, uint32_t storage_texture_count);
    static SDL_GPUTexture* CreateTexture(const SDL_GPUTextureCreateInfo& info);
    static SDL_GPUGraphicsPipeline* CreateGPUGraphicsPipeline(const SDL_GPUGraphicsPipelineCreateInfo& info);
    static SDL_GPUBuffer* CreateBuffer(const SDL_GPUBufferCreateInfo& info);
    static SDL_GPUSampler* CreateSampler(const SDL_GPUSamplerCreateInfo& info);
    static SDL_GPUTransferBuffer* CreateTransferBuffer(const SDL_GPUTransferBufferCreateInfo& info);

    static SDL_GPUTextureFormat GetGPUSwapchainTextureFormat();

    static void* MapGPUTransferBuffer(SDL_GPUTransferBuffer* transfer_buffer, bool cycle);
    static void UnmapGPUTransferBuffer(SDL_GPUTransferBuffer* transfer_buffer);

    static SDL_GPUCommandBuffer* AcquireCommandBuffer();
    
    static void ReleaseShader(SDL_GPUShader* shader);
    static void ReleaseTexture(SDL_GPUTexture* texture);
    static void ReleaseGPUTransferBuffer(SDL_GPUTransferBuffer* transfer_buffer);

private:
    static inline SDL_GPUDevice* gpu_device{};
};
