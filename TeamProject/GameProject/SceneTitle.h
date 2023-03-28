#pragma once
#include "Scene.h"

class SceneTitle :
    public Scene
{
public:

public:
    virtual bool Init() override;
    virtual bool Frame() override;
    virtual bool Render() override;
    virtual bool Release() override;

    E_SCENE NextScene();

};

