#pragma once
#include "Scene.h"
#include "SoundMgr.h"
class SceneEnd :
    public Scene
{
public:

public:
    virtual bool Init() override;
    virtual bool Frame() override;
    virtual bool Render() override;
    virtual bool Release() override;

    virtual void DataLoad() override;

    E_SCENE NextScene();

};

