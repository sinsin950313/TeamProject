#pragma once
#include "Interface.h"
#include "Camera.h"
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

    virtual void SetCurrentRTT(ID3D11RenderTargetView** pRTVS, UINT iRtvCount, ID3D11DepthStencilView* pDSV, D3D11_VIEWPORT* pViewport);
    ID3D11RenderTargetView** m_pCurrentRenderTargetViews = nullptr;
    UINT m_iCurrentRTVCount = 0;
    ID3D11DepthStencilView* m_pCurrentDepthStencilView = nullptr;
    D3D11_VIEWPORT* m_pCurrentViewport = nullptr;

public:
    ID3D11Device* m_pd3dDevice = nullptr;// µð¹ÙÀÌ½º °´Ã¼
    ID3D11DeviceContext* m_pImmediateContext = nullptr;
   
    E_SCENE m_Scene = S_TITLE;
public:
    virtual ~Scene();

};

Scene* CreateScene(E_SCENE s_type);