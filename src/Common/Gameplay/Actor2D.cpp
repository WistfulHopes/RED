#include "Common/Gameplay/Actor2D.h"
#include <SDL3/SDL.h>
#include <format>
#include <ranges>

#include "GameCommon.h"
#include "AALib/Base/sys_Math.h"
#include "Common/Rendering/LazyTexture.h"
#include "Common/Rendering/RenderManager.h"

typedef struct FragMultiplyUniform
{
    float r, g, b, a;
} FragMultiplyUniform;

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

    textures.insert({img_name, texture});

    return true;
}

void Actor2D::Draw()
{
    auto cmd_buf = RenderManager::AcquireCommandBuffer();

    if (!cmd_buf)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AcquireGPUCommandBuffer failed: %s", SDL_GetError());
        return;
    }

    SDL_GPUTexture* swapchain_texture;
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmd_buf, GameCommon::window, &swapchain_texture, nullptr, nullptr))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "WaitAndAcquireGPUSwapchainTexture failed: %s", SDL_GetError());
        return;
    }

    if (swapchain_texture)
    {
        SDL_GPUColorTargetInfo color_target_info = {0};
        color_target_info.texture = swapchain_texture;
        color_target_info.clear_color = (SDL_FColor){0.0f, 0.0f, 0.0f, 1.0f};
        color_target_info.load_op = SDL_GPU_LOADOP_CLEAR;
        color_target_info.store_op = SDL_GPU_STOREOP_STORE;

        SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(cmd_buf, &color_target_info, 1, NULL);

        SDL_BindGPUGraphicsPipeline(render_pass, pipeline);
        SDL_BindGPUVertexBuffers(render_pass, 0, &(SDL_GPUBufferBinding){.buffer = vertex_buffer, .offset = 0}, 1);
        SDL_BindGPUIndexBuffer(render_pass, &(SDL_GPUBufferBinding){.buffer = index_buffer, .offset = 0},
                               SDL_GPU_INDEXELEMENTSIZE_16BIT);
        SDL_BindGPUFragmentSamplers(render_pass, 0, &(SDL_GPUTextureSamplerBinding){
                                        .texture = textures[cur_sprite_name]->GetTexture(), .sampler = sampler
                                    }, 1);

        // Top-left
        AA_Matrix matrix_uniform = AA_Matrix(AA_Rotator(0, 0, 0)).Mult(AA_Matrix(-0.5f, -0.5f, 0));
        SDL_PushGPUVertexUniformData(cmd_buf, 0, &matrix_uniform, sizeof(matrix_uniform));
        SDL_PushGPUFragmentUniformData(cmd_buf, 0, &(FragMultiplyUniform){1.0f, 0.5f + SDL_sinf(0) * 0.5f, 1.0f, 1.0f},
                                       sizeof(FragMultiplyUniform));
        SDL_DrawGPUIndexedPrimitives(render_pass, 6, 1, 0, 0, 0);

        // Top-right
        matrix_uniform = AA_Matrix(AA_Rotator(0, 2.0f * SDL_PI_F, 0)).Mult(AA_Matrix(0.5f, -0.5f, 0));

        SDL_PushGPUVertexUniformData(cmd_buf, 0, &matrix_uniform, sizeof(matrix_uniform));
        SDL_PushGPUFragmentUniformData(cmd_buf, 0, &(FragMultiplyUniform){1.0f, 0.5f + SDL_cosf(0) * 0.5f, 1.0f, 1.0f},
                                       sizeof(FragMultiplyUniform));
        SDL_DrawGPUIndexedPrimitives(render_pass, 6, 1, 0, 0, 0);

        // Bottom-left
        matrix_uniform = AA_Matrix(AA_Rotator(0, 0, 0)).Mult(AA_Matrix(-0.5f, 0.5f, 0));
        SDL_PushGPUVertexUniformData(cmd_buf, 0, &matrix_uniform, sizeof(matrix_uniform));
        SDL_PushGPUFragmentUniformData(cmd_buf, 0, &(FragMultiplyUniform){1.0f, 0.5f + SDL_sinf(0) * 0.2f, 1.0f, 1.0f},
                                       sizeof(FragMultiplyUniform));
        SDL_DrawGPUIndexedPrimitives(render_pass, 6, 1, 0, 0, 0);

        // Bottom-right
        matrix_uniform = AA_Matrix(AA_Rotator(0, 0, 0)).Mult(AA_Matrix(0.5f, 0.5f, 0));
        SDL_PushGPUVertexUniformData(cmd_buf, 0, &matrix_uniform, sizeof(matrix_uniform));
        SDL_PushGPUFragmentUniformData(cmd_buf, 0, &(FragMultiplyUniform){1.0f, 0.5f + SDL_cosf(0) * 1.0f, 1.0f, 1.0f},
                                       sizeof(FragMultiplyUniform));
        SDL_DrawGPUIndexedPrimitives(render_pass, 6, 1, 0, 0, 0);

        SDL_EndGPURenderPass(render_pass);
    }

    SDL_SubmitGPUCommandBuffer(cmd_buf);
}

bool Actor2D::InitGPU()
{
    if (!SetSprite(cur_sprite_name)) return false;

    auto texture = textures[cur_sprite_name];

    auto vertex_shader = RenderManager::CreateShader("Sprite.vert", 0, 1, 0, 0);
    if (!vertex_shader)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create vertex shader Sprite.vert");
        return false;
    }

    auto fragment_shader = RenderManager::CreateShader("Sprite.frag", 0, 1, 0, 0);
    if (!fragment_shader)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create fragment shader Sprite.frag");
        return false;
    }

    SDL_GPUGraphicsPipelineCreateInfo pipeline_create_info = {
        .target_info = {
            .num_color_targets = 1,
            .color_target_descriptions = (SDL_GPUColorTargetDescription[]){
                {
                    .format = RenderManager::GetGPUSwapchainTextureFormat(),
                    .blend_state = {
                        .enable_blend = true,
                        .alpha_blend_op = SDL_GPU_BLENDOP_ADD,
                        .color_blend_op = SDL_GPU_BLENDOP_ADD,
                        .src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
                        .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
                        .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                        .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA
                    }
                }
            }
        },
        .vertex_input_state = (SDL_GPUVertexInputState){
            .num_vertex_buffers = 1,
            .vertex_buffer_descriptions = (SDL_GPUVertexBufferDescription[]){
                {
                    .slot = 0,
                    .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
                    .instance_step_rate = 0,
                    .pitch = sizeof(PositionTextureVertex)
                }
            },
            .num_vertex_attributes = 2,
            .vertex_attributes = (SDL_GPUVertexAttribute[]){
                {
                    .buffer_slot = 0,
                    .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
                    .location = 0,
                    .offset = 0
                },
                {
                    .buffer_slot = 0,
                    .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
                    .location = 1,
                    .offset = sizeof(float) * 3
                }
            }
        },
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .vertex_shader = vertex_shader,
        .fragment_shader = fragment_shader,
    };

    pipeline = RenderManager::CreateGPUGraphicsPipeline(pipeline_create_info);
    if (pipeline == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create graphics pipeline");
        return false;
    }

    vertex_buffer = RenderManager::CreateBuffer(
        {
            .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
            .size = sizeof(PositionTextureVertex) * 4
        }
    );

    vertex_buffer = RenderManager::CreateBuffer(
        {
            .usage = SDL_GPU_BUFFERUSAGE_INDEX,
            .size = sizeof(Uint16) * 6
        }
    );

    sampler = RenderManager::CreateSampler({
        .min_filter = SDL_GPU_FILTER_NEAREST,
        .mag_filter = SDL_GPU_FILTER_NEAREST,
        .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
        .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
        .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
        .address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
    });

    auto buffer_transfer_buffer = RenderManager::CreateTransferBuffer({
            .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
            .size = (sizeof(PositionTextureVertex) * 4) + (sizeof(Uint16) * 6)
        }
    );

    PositionTextureVertex* transfer_data = static_cast<PositionTextureVertex*>(RenderManager::MapGPUTransferBuffer(
        buffer_transfer_buffer, false));

    auto coords = GetCoords();

    transfer_data[0] = coords[0];
    transfer_data[1] = coords[1];
    transfer_data[2] = coords[2];
    transfer_data[3] = coords[3];


    Uint16* index_data = (Uint16*)&transfer_data[4];
    index_data[0] = 0;
    index_data[1] = 1;
    index_data[2] = 2;
    index_data[3] = 0;
    index_data[4] = 2;
    index_data[5] = 3;

    RenderManager::UnmapGPUTransferBuffer(buffer_transfer_buffer);

    auto texture_transfer_buffer = RenderManager::CreateTransferBuffer({
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = static_cast<uint32_t>(texture->GetWidth() * texture->GetHeight() * 4)
    });

    auto texture_transfer_ptr = static_cast<Uint8*>(RenderManager::MapGPUTransferBuffer(
        texture_transfer_buffer, false));

    SDL_memcpy(texture_transfer_ptr, texture->GetSurface()->pixels,
               static_cast<uint32_t>(texture->GetWidth() * texture->GetHeight() * 4));
    RenderManager::UnmapGPUTransferBuffer(texture_transfer_buffer);

    auto upload_cmd_buffer = RenderManager::AcquireCommandBuffer();

    SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(upload_cmd_buffer);

    SDL_UploadToGPUBuffer(
        copy_pass,
        &(SDL_GPUTransferBufferLocation){
            .transfer_buffer = buffer_transfer_buffer,
            .offset = 0
        },
        &(SDL_GPUBufferRegion){
            .buffer = vertex_buffer,
            .offset = 0,
            .size = sizeof(PositionTextureVertex) * 4
        },
        false
    );

    SDL_UploadToGPUBuffer(
        copy_pass,
        &(SDL_GPUTransferBufferLocation){
            .transfer_buffer = buffer_transfer_buffer,
            .offset = sizeof(PositionTextureVertex) * 4
        },
        &(SDL_GPUBufferRegion){
            .buffer = index_buffer,
            .offset = 0,
            .size = sizeof(Uint16) * 6
        },
        false
    );

    SDL_UploadToGPUTexture(
        copy_pass,
        &(SDL_GPUTextureTransferInfo){
            .transfer_buffer = texture_transfer_buffer,
            .offset = 0, /* Zeroes out the rest */
        },
        &(SDL_GPUTextureRegion){
            .texture = texture->GetTexture(),
            .w = static_cast<Uint8>(texture->GetWidth()),
            .h = static_cast<Uint8>(texture->GetHeight()),
            .d = 1
        },
        false
    );

    SDL_EndGPUCopyPass(copy_pass);
    SDL_SubmitGPUCommandBuffer(upload_cmd_buffer);
    RenderManager::ReleaseGPUTransferBuffer(buffer_transfer_buffer);
    RenderManager::ReleaseGPUTransferBuffer(texture_transfer_buffer);

    return true;
}
