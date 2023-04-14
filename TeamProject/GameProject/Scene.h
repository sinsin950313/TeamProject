#pragma once
#include "Interface.h"

enum E_SCENE
{
    S_TITLE = 0,
    S_INGAME,
    S_INGAME2,
    S_END,
    MAX_SCENE
};

class Scene
{
public:
    virtual bool    SetDevice(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext);
    virtual bool    Init();
    virtual bool    Frame();
    virtual bool    PreRender();
    virtual bool    Render();
    virtual bool    PostRender();
    virtual bool    Release();

    virtual void    DataLoad() = 0;

    virtual E_SCENE     NextScene() = 0;

public:
    ID3D11Device* m_pd3dDevice = nullptr;// 디바이스 객체
    ID3D11DeviceContext* m_pImmediateContext = nullptr;
    E_SCENE m_Scene = S_TITLE;
public:
    virtual ~Scene() { };
};

Scene* CreateScene(E_SCENE s_type);