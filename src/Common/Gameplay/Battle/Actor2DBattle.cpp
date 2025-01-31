#include "Common/Gameplay/Battle/Actor2DBattle.h"

#include "GameCommon.h"
#include "Game/Battle/Object/obj_base.h"
#include "Game/Battle/Object/Char/char_Base.h"

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
