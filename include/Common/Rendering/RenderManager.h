#pragma once
#include <string>
#include <SDL3/SDL_gpu.h>

#ifdef _WIN32
#define SHADER_SUPPORTED_FORMATS (SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXBC | SDL_GPU_SHADERFORMAT_DXIL)
#elif __linux__ 
#define SHADER_SUPPORTED_FORMATS (SDL_GPU_SHADERFORMAT_SPIRV)
#endif

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

    static void Draw();
    
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
    static void ReleaseSampler(SDL_GPUSampler* texture);
    static void ReleaseGPUTransferBuffer(SDL_GPUTransferBuffer* transfer_buffer);
    static void ReleaseBuffer(SDL_GPUBuffer* buffer);
    static void ReleaseGPUGraphicsPipeline(SDL_GPUGraphicsPipeline* pipeline);

private:
    static inline SDL_GPUDevice* gpu_device{};
};
