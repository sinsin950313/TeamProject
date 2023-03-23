#pragma once
#include "CScene.h"

class CSceneInGame : public CScene
{
public:
    virtual bool    Init();
    virtual bool    Frame();
    virtual bool    Render();
    virtual bool    Release();

private:
    bool		ObjectInit();
    bool		ObjectFrame();
    bool		ObjectRender();
    bool		ObjectRelease();

    E_SCENE NextScene();

public:
    //CSpacePartition* m_pWorldSP;

private:
    //CPlayer* m_pPlayer;
    //std::list<CMapObject*> m_pMapList;
    //std::list<CSprite*>     m_pBorder;
    //
    //CInterface* m_pScore;
    //CInterface* m_pLife;
    //CInterface* m_pAlert;
    //
    //bool    m_isAlert;
    //
    //float   m_fOnGameTime;
    //
    //int     m_iScore;
    //
    //CSound* m_pBgIngame;
    //CSound* m_pItemSound;
    //CSound* m_pAlertSound;

    //std::map<int, CBaseObject*>	m_StaticList;
    //std::map<int, CBaseObject*>	m_AllObjectList;
    //std::vector<CBaseObject*>		m_DrawList;
};

