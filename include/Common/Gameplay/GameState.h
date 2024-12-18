#pragma once

#include <vector>

class AA_CCamera;
class SCENE_CBase;
class Actor;

class GameState
{
private:
    SCENE_CBase* scene;
    AA_CCamera* camera;
    std::vector<Actor*> actors;

public:
    GameState() = default;
    virtual ~GameState() = default;
    
    virtual void Init(SCENE_CBase* in_scene);
    void SetCamera(AA_CCamera* in_camera) { camera = in_camera; }
    virtual void Draw();
};
