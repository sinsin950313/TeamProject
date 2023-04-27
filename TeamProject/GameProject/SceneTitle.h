#pragma once
#include "Scene.h"
#include "InteractiveObject.h"

class SceneTitle :
    public Scene
{
public:

public:
    virtual bool Init() override;
    virtual bool Frame() override;
    virtual bool Render() override;
    virtual bool PostRender() override;
    virtual bool Release() override;
    virtual void DataLoad() override;

    E_SCENE NextScene();

public:
    TestObj* m_pObj;
    InteractiveObject* m_pInteract;

    Interface* m_pInter;

};

