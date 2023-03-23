#pragma once
#include "CScene.h"

class CSceneTitle :
    public CScene
{
public:
    //CInterface* m_pTitle;
    //CInterface* m_pStartButton;
    //
    //CSound* m_pBgTitle;
    //CSound* m_pBtn;

public:
    virtual bool Init() override;
    virtual bool Frame() override;
    virtual bool Render() override;
    virtual bool Release() override;

    E_SCENE NextScene();

};

