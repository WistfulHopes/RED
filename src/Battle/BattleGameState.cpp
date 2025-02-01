#include "Battle/BattleGameState.h"

#include "Battle/Actor2DBattle.h"
#include "Game/Scene/scene_Battle.h"

void BattleGameState::Init(SCENE_CBase* in_scene)
{
    GameState::Init(in_scene);

    if (const auto battle_scene = dynamic_cast<SCENE_CBattle*>(in_scene))
    {
        AddActor2D(battle_scene->GetBattleObjectManager()->GetPlayer(SIDE_1P, MemberID_01));
        AddActor2D(battle_scene->GetBattleObjectManager()->GetPlayer(SIDE_2P, MemberID_01));
    }
}

void BattleGameState::AddActor2D(OBJ_CBase* obj)
{
    auto actor = new Actor2DBattle(obj);
    actor->LoadSprites();
    actors.push_back(actor);
}
