#pragma once
#include "Common/Gameplay/Actor2D.h"

class Actor2DBattle : Actor2D
{
public:
    class OBJ_CBase* obj;

    void LoadSprites() override;
};
