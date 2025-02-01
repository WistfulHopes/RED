#include "Battle/Actor2DBattle.h"

#include <array>

#include "GameCommon.h"
#include "Common/Rendering/LazyTexture.h"
#include "Game/Battle/Object/obj_base.h"
#include "Game/Battle/Object/Char/char_Base.h"

Actor2DBattle::Actor2DBattle(OBJ_CBase* in_obj) : obj(in_obj)
{
}

void Actor2DBattle::LoadSprites()
{
    size_t img_pac_size = 0;

    std::string path = "\\assets\\IMG_";
    if (obj->m_IsPlayerObj)
    {
        path += ((OBJ_CCharBase*)obj)->m_CharName.GetStr();
    }
    else
    {
        path += std::string(((OBJ_CCharBase*)obj->m_pParentPly.GetPtr())->m_CharName.GetStr()) + "EF";
    }
    path += ".pac";

    img_pac.SetPackFile(GameCommon::LoadAsset(path, &img_pac_size));
}

void Actor2DBattle::Draw(struct SDL_GPUCommandBuffer* cmd_buf, struct SDL_GPUTexture* swapchain_texture,
                         struct SDL_GPURenderPass* render_pass)
{
    const auto imgName = obj->m_ClsnAnalyzer.GetImageFileName(0);
    if (!imgName) return;

    if (!SetSprite(imgName)) return;

    Actor2D::Draw(cmd_buf, swapchain_texture, render_pass);
}

std::array<PositionTextureVertex, 4> Actor2DBattle::GetCoords()
{
    auto scene = dynamic_cast<SCENE_CBattle*>(REDGameCommon::GetInstance()->GetScene());
    const auto linkMagn = scene->GetBattleScreenManager()->GetLinkMagn() * 0.001f;

    std::array<PositionTextureVertex, 4> vertices{};

    float dest_x = obj->m_ClsnAnalyzer.GetViewWorldRect(0).m_X * 1000.f;
    float dest_y = obj->m_ClsnAnalyzer.GetViewWorldRect(0).m_Y * 1000.f;
    float size_x = obj->m_ClsnAnalyzer.GetViewWorldRect(0).m_W * 1000.f;
    float size_y = obj->m_ClsnAnalyzer.GetViewWorldRect(0).m_H * 1000.f;

    float uv_x_min = obj->m_ClsnAnalyzer.GetViewTextureRect(0).m_X / textures[cur_sprite_name]->GetWidth();
    float uv_x_max = (obj->m_ClsnAnalyzer.GetViewTextureRect(0).m_X + obj->m_ClsnAnalyzer.GetViewTextureRect(0).m_W) / textures[cur_sprite_name]->GetWidth();
    float uv_y_min = obj->m_ClsnAnalyzer.GetViewTextureRect(0).m_Y / textures[cur_sprite_name]->GetHeight();
    float uv_y_max = (obj->m_ClsnAnalyzer.GetViewTextureRect(0).m_Y + obj->m_ClsnAnalyzer.GetViewTextureRect(0).m_H) / textures[cur_sprite_name]->GetHeight();
    
    if (obj->GetObjDir() == OBJDIR_RIGHT)
    {
        vertices[0] = PositionTextureVertex(
            ((float)obj->m_PosX - dest_x) * linkMagn,
            ((float)obj->m_PosY - dest_y + size_y / 2) * linkMagn, 0, uv_x_min, uv_y_min);
        vertices[1] = PositionTextureVertex(
            ((float)obj->m_PosX - dest_x - size_x) * linkMagn,
            ((float)obj->m_PosY - dest_y + size_y / 2) * linkMagn, 0, uv_x_max, uv_y_min);
        vertices[2] = PositionTextureVertex(
            ((float)obj->m_PosX - dest_x - size_x) * linkMagn,
            ((float)obj->m_PosY - dest_y - size_y / 2) * linkMagn, 0, uv_x_max, uv_y_max);
        vertices[3] = PositionTextureVertex(
            ((float)obj->m_PosX - dest_x) * linkMagn,
            ((float)obj->m_PosY - dest_y - size_y / 2) * linkMagn, 0, uv_x_min, uv_y_max);
    }
    else
    {
        vertices[0] = PositionTextureVertex(
            ((float)obj->m_PosX + dest_x) * linkMagn,
            ((float)obj->m_PosY - dest_y + size_y / 2) * linkMagn, 0, uv_x_min, uv_y_min);
        vertices[1] = PositionTextureVertex(
            ((float)obj->m_PosX + dest_x + size_x) * linkMagn,
            ((float)obj->m_PosY - dest_y + size_y / 2) * linkMagn, 0, uv_x_max, uv_y_min);
        vertices[2] = PositionTextureVertex(
            ((float)obj->m_PosX + dest_x + size_x) * linkMagn,
            ((float)obj->m_PosY - dest_y - size_y / 2) * linkMagn, 0, uv_x_max, uv_y_max);
        vertices[3] = PositionTextureVertex(
            ((float)obj->m_PosX + dest_x) * linkMagn,
            ((float)obj->m_PosY - dest_y - size_y / 2) * linkMagn, 0, uv_x_min, uv_y_max);
    }

    return vertices;
}
