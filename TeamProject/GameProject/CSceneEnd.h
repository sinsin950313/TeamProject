#pragma once
#include "CScene.h"
#include "SoundMgr.h"
class CSceneEnd :
    public CScene
{
public:
    //CInterface* m_pOver;
    //CInterface* m_pBlack;
    //
    //CSound* m_pEndSound;

    int     m_iPlayed;

public:
    virtual bool Init() override;
    virtual bool Frame() override;
    virtual bool Render() override;
    virtual bool Release() override;

    E_SCENE NextScene();

};

