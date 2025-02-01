#pragma once

#include "Common/Gameplay/GameState.h"

class BattleGameState : public GameState
{
    void Init(SCENE_CBase* in_scene) override;

    void AddActor2D(class OBJ_CBase* obj);
};