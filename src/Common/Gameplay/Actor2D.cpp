#include "Common/Gameplay/Actor2D.h"

#include <SDL3/SDL.h>
#include <format>
#include <ranges>
#include "Common/Rendering/LazyTexture.h"
#include "Common/Rendering/RenderManager.h"

Actor2D::Actor2D()
{
    InitGPU();
}

Actor2D::~Actor2D()
{
    for (const auto val : textures | std::views::values)
    {
        delete val;
    }

    RenderManager::ReleaseGPUGraphicsPipeline(pipeline);
    RenderManager::ReleaseBuffer(index_buffer);
    RenderManager::ReleaseBuffer(vertex_buffer);
    RenderManager::ReleaseGPUTransferBuffer(buffer_transfer_buffer);
    RenderManager::ReleaseSampler(sampler);
}

bool Actor2D::SetSprite(const char* img_name)
{
    if (!strncmp(img_name, cur_sprite_name, 0x20)) return true;

    strcpy(cur_sprite_name, img_name);
    
    if (textures.contains(img_name)) return true;

    auto idx = img_pac.SearchFileID(img_name);

    if (static_cast<int>(idx) == -1) return false;

    strcpy(cur_sprite_name, img_name);

    const auto offset = img_pac.GetPackOffsetAddr(idx);
    const auto size = img_pac.GetPackOffsetFileSize(idx);

    const auto stream = SDL_IOFromMem(offset, size);
    const auto texture = new LazyTexture();
    texture->Load(stream, SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM);

    textures.insert({img_name, texture});

    return true;
}

void Actor2D::Draw(SDL_GPUCommandBuffer* cmd_buf, SDL_GPUTexture* swapchain_texture, SDL_GPURenderPass* render_pass)
{
    auto texture = textures[cur_sprite_name];

    auto texture_transfer_buffer = RenderManager::CreateTransferBuffer({
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = static_cast<uint32_t>(texture->GetWidth() * texture->GetHeight() * 4)
    });

    auto texture_transfer_ptr = static_cast<Uint8*>(RenderManager::MapGPUTransferBuffer(
        texture_transfer_buffer, false));

    SDL_memcpy(texture_transfer_ptr, texture->GetSurface()->pixels,
               static_cast<uint32_t>(texture->GetWidth() * texture->GetHeight() * 4));
    RenderManager::UnmapGPUTransferBuffer(texture_transfer_buffer);

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

    SDL_GPUTransferBufferLocation transfer_buffer_location = {
        .transfer_buffer = buffer_transfer_buffer,
        .offset = 0
    };
    SDL_GPUBufferRegion buffer_region = {
        .buffer = vertex_buffer,
        .offset = 0,
        .size = sizeof(PositionTextureVertex) * 4
    };

    SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(cmd_buf);

    SDL_UploadToGPUBuffer(
        copy_pass,
        &transfer_buffer_location,
        &buffer_region,
        false
    );

    transfer_buffer_location = {
        .transfer_buffer = buffer_transfer_buffer,
        .offset = sizeof(PositionTextureVertex) * 4
    };
    buffer_region = {
        .buffer = index_buffer,
        .offset = 0,
        .size = sizeof(Uint16) * 6
    };
    
    SDL_UploadToGPUBuffer(
        copy_pass,
        &transfer_buffer_location,
        &buffer_region,
        false
    );

    SDL_GPUTextureTransferInfo texture_transfer_info = {
        .transfer_buffer = texture_transfer_buffer,
        .offset = 0,
    };
    SDL_GPUTextureRegion texture_region = {
        .texture = texture->GetTexture(),
        .w = (Uint32)texture->GetWidth(),
        .h = (Uint32)texture->GetHeight(),
        .d = 1,
    };
    
    SDL_UploadToGPUTexture(
        copy_pass,
        &texture_transfer_info,
        &texture_region,
        false
    );

    SDL_EndGPUCopyPass(copy_pass);
    
    RenderManager::ReleaseGPUTransferBuffer(texture_transfer_buffer);

    SDL_BindGPUGraphicsPipeline(render_pass, pipeline);
    SDL_GPUBufferBinding vertex_binding = {.buffer = vertex_buffer, .offset = 0};
    SDL_BindGPUVertexBuffers(render_pass, 0, &vertex_binding, 1);
    SDL_GPUBufferBinding index_binding = {.buffer = index_buffer, .offset = 0};
    SDL_BindGPUIndexBuffer(render_pass, &index_binding, SDL_GPU_INDEXELEMENTSIZE_16BIT);
    SDL_GPUTextureSamplerBinding texture_binding = {
        .texture = textures[cur_sprite_name]->GetTexture(), .sampler = sampler
    };
    SDL_BindGPUFragmentSamplers(render_pass, 0, &texture_binding, 1);
    SDL_DrawGPUIndexedPrimitives(render_pass, 6, 1, 0, 0, 0);
}

bool Actor2D::InitGPU()
{
    auto vertex_shader = RenderManager::CreateShader("Sprite.vert", 0, 1, 0, 0);
    if (!vertex_shader)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create vertex shader Sprite.vert");
        return false;
    }

    auto fragment_shader = RenderManager::CreateShader("Sprite.frag", 1, 0, 0, 0);
    if (!fragment_shader)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create fragment shader Sprite.frag");
        return false;
    }

    SDL_GPUColorTargetDescription gpu_color_target_description[] = {
        {
            .format = RenderManager::GetGPUSwapchainTextureFormat(),
            .blend_state = {
                .src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
                .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                .color_blend_op = SDL_GPU_BLENDOP_ADD,
                .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
                .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                .alpha_blend_op = SDL_GPU_BLENDOP_ADD,
                .enable_blend = true,
            }
        }
    };

    SDL_GPUVertexBufferDescription vertex_buffer_description[] = {
        {
            .slot = 0,
            .pitch = sizeof(PositionTextureVertex),
            .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
            .instance_step_rate = 0,
        }
    };
    SDL_GPUVertexAttribute vertex_attributes[] = {
        {
            .location = 0,
            .buffer_slot = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
            .offset = 0
        },
        {
            .location = 1,
            .buffer_slot = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
            .offset = sizeof(float) * 3
        }
    };
    SDL_GPUGraphicsPipelineCreateInfo pipeline_create_info = {
        .vertex_shader = vertex_shader,
        .fragment_shader = fragment_shader,
        .vertex_input_state = {
            .vertex_buffer_descriptions = vertex_buffer_description,
            .num_vertex_buffers = 1,
            .vertex_attributes = vertex_attributes,
            .num_vertex_attributes = 2,
        },
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .target_info = {
            .color_target_descriptions = gpu_color_target_description,
            .num_color_targets = 1,
        },
    };
    
    pipeline = RenderManager::CreateGPUGraphicsPipeline(pipeline_create_info);
    if (pipeline == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create graphics pipeline: %s", SDL_GetError());
        return false;
    }

    RenderManager::ReleaseShader(vertex_shader);
    RenderManager::ReleaseShader(fragment_shader);

    vertex_buffer = RenderManager::CreateBuffer(
        {
            .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
            .size = sizeof(PositionTextureVertex) * 4
        }
    );

    index_buffer = RenderManager::CreateBuffer(
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

    buffer_transfer_buffer = RenderManager::CreateTransferBuffer({
            .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
            .size = (sizeof(PositionTextureVertex) * 4) + (sizeof(Uint16) * 6)
        }
    );

    auto upload_cmd_buffer = RenderManager::AcquireCommandBuffer();

    SDL_SubmitGPUCommandBuffer(upload_cmd_buffer);

    return true;
}
