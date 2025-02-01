#include "Common/Rendering/RenderManager.h"

#include <format>
#include <SDL3/SDL_log.h>

#include "GameCommon.h"

#define CHECK_CREATE(var, thing) { if (!(var)) { SDL_Log("Failed to create %s: %s\n", thing, SDL_GetError()); } }

void RenderManager::Destroy()
{
    SDL_ReleaseWindowFromGPUDevice(gpu_device, GameCommon::window);
    SDL_DestroyGPUDevice(gpu_device);
}

void RenderManager::Draw()
{
    auto cmd_buf = AcquireCommandBuffer();

    if (!cmd_buf)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "AcquireGPUCommandBuffer failed: %s", SDL_GetError());
        return;
    }

    SDL_GPUTexture* swapchain_texture;
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmd_buf, GameCommon::window, &swapchain_texture, nullptr, nullptr))
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "WaitAndAcquireGPUSwapchainTexture failed: %s", SDL_GetError());
        return;
    }

    if (swapchain_texture)
        GameCommon::Draw(cmd_buf, swapchain_texture);
    
    SDL_SubmitGPUCommandBuffer(cmd_buf);
}

void RenderManager::Init()
{
    gpu_device = SDL_CreateGPUDevice(SHADER_SUPPORTED_FORMATS, false, nullptr);
    CHECK_CREATE(gpu_device, "GPU device")

    SDL_ClaimWindowForGPUDevice(gpu_device, GameCommon::window);
}

SDL_GPUShader* RenderManager::CreateShader(const std::string& shader_filename, uint32_t sampler_count,
                                           uint32_t uniform_buffer_count, uint32_t storage_buffer_count,
                                           uint32_t storage_texture_count)
{
    SDL_GPUShaderStage stage;

    if (shader_filename.contains(".vert")) stage = SDL_GPU_SHADERSTAGE_VERTEX;
    else if (shader_filename.contains(".frag")) stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
    else
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, std::format("Invalid shader stage for {}", shader_filename).c_str());
        return nullptr;
    }

    std::string shader_path;
    std::string entry_point;

    const SDL_GPUShaderFormat backendFormats = SDL_GetGPUShaderFormats(gpu_device);
    SDL_GPUShaderFormat format;

    if (backendFormats & SDL_GPU_SHADERFORMAT_SPIRV)
    {
        shader_path = "assets/Shaders/" + shader_filename + ".spv";
        format = SDL_GPU_SHADERFORMAT_SPIRV;
        entry_point = "main";
    }
    else if (backendFormats & SDL_GPU_SHADERFORMAT_MSL)
    {
        shader_path = "assets/Shaders/" + shader_filename + ".msl";
        format = SDL_GPU_SHADERFORMAT_MSL;
        entry_point = "main0";
    }
    else if (backendFormats & SDL_GPU_SHADERFORMAT_DXIL)
    {
        shader_path = "assets/Shaders/" + shader_filename + ".dxil";
        format = SDL_GPU_SHADERFORMAT_DXIL;
        entry_point = "main";
    }
    else
    {
        SDL_Log("%s", "Unrecognized backend shader format!");
        return NULL;
    }

    size_t code_size;
    void* code = GameCommon::LoadAsset(shader_path, &code_size);

    if (!code)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, std::format("Failed to load {}", shader_path).c_str());
        return nullptr;
    }

    SDL_GPUShaderCreateInfo shader_info = {
        .code_size = code_size,
        .code = static_cast<Uint8*>(code),
        .entrypoint = entry_point.c_str(),
        .format = format,
        .stage = stage,
        .num_samplers = sampler_count,
        .num_storage_textures = storage_texture_count,
        .num_storage_buffers = storage_buffer_count,
        .num_uniform_buffers = uniform_buffer_count,
    };

    SDL_GPUShader* shader = SDL_CreateGPUShader(gpu_device, &shader_info);
    if (!shader)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, std::format("Failed to create shader {}", shader_path).c_str());
        SDL_free(code);
        return nullptr;
    }

    SDL_free(code);
    return shader;
}

SDL_GPUTexture* RenderManager::CreateTexture(const SDL_GPUTextureCreateInfo& info)
{
    return SDL_CreateGPUTexture(gpu_device, &info);
}

SDL_GPUGraphicsPipeline* RenderManager::CreateGPUGraphicsPipeline(const SDL_GPUGraphicsPipelineCreateInfo& info)
{
    return SDL_CreateGPUGraphicsPipeline(gpu_device, &info);
}

SDL_GPUBuffer* RenderManager::CreateBuffer(const SDL_GPUBufferCreateInfo& info)
{
    return SDL_CreateGPUBuffer(gpu_device, &info);
}

SDL_GPUSampler* RenderManager::CreateSampler(const SDL_GPUSamplerCreateInfo& info)
{
    return SDL_CreateGPUSampler(gpu_device, &info);
}

SDL_GPUTransferBuffer* RenderManager::CreateTransferBuffer(const SDL_GPUTransferBufferCreateInfo& info)
{
    return SDL_CreateGPUTransferBuffer(gpu_device, &info);
}

SDL_GPUTextureFormat RenderManager::GetGPUSwapchainTextureFormat()
{
    return SDL_GetGPUSwapchainTextureFormat(gpu_device, GameCommon::window);
}

void* RenderManager::MapGPUTransferBuffer(SDL_GPUTransferBuffer* transfer_buffer, bool cycle)
{
    return SDL_MapGPUTransferBuffer(gpu_device, transfer_buffer, cycle);
}

void RenderManager::UnmapGPUTransferBuffer(SDL_GPUTransferBuffer* transfer_buffer)
{
    SDL_UnmapGPUTransferBuffer(gpu_device, transfer_buffer);
}

SDL_GPUCommandBuffer* RenderManager::AcquireCommandBuffer()
{
    return SDL_AcquireGPUCommandBuffer(gpu_device);
}

void RenderManager::ReleaseShader(SDL_GPUShader* shader)
{
    SDL_ReleaseGPUShader(gpu_device, shader);
}

void RenderManager::ReleaseTexture(SDL_GPUTexture* texture)
{
    SDL_ReleaseGPUTexture(gpu_device, texture);
}

void RenderManager::ReleaseSampler(SDL_GPUSampler* sampler)
{
    SDL_ReleaseGPUSampler(gpu_device, sampler);
}

void RenderManager::ReleaseGPUTransferBuffer(SDL_GPUTransferBuffer* transfer_buffer)
{
    SDL_ReleaseGPUTransferBuffer(gpu_device, transfer_buffer);
}

void RenderManager::ReleaseBuffer(SDL_GPUBuffer* buffer)
{
    SDL_ReleaseGPUBuffer(gpu_device, buffer);
}

void RenderManager::ReleaseGPUGraphicsPipeline(SDL_GPUGraphicsPipeline* pipeline)
{
    SDL_ReleaseGPUGraphicsPipeline(gpu_device, pipeline);
}
