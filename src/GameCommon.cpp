#include "GameCommon.h"

#include "Common/Rendering/RenderManager.h"

void* GameCommon::LoadAsset(std::string file_name, size_t* data_size)
{
    return SDL_LoadFile((base_path + file_name).c_str(), data_size);
}
