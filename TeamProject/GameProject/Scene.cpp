#include "Scene.h"
#include "SceneTitle.h"
#include "SceneInGame.h"
#include "SceneEnd.h"

Scene* CreateScene(E_SCENE s_type)
{
    Scene* pScene;
    switch (s_type)
    {
    case E_SCENE::S_TITLE:
        pScene = new SceneTitle();
        break;
    case E_SCENE::S_INGAME:
        pScene = new SceneInGame();
        break;
    case E_SCENE::S_INGAME2:
        pScene = new SceneInGame();
        break;
    case E_SCENE::S_END:
        pScene = new SceneEnd();
        break;
    default:
        pScene = nullptr;
        break;
    }
    return pScene;
}

bool    Scene::SetDevice(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext)
{
    m_pd3dDevice = pd3dDevice;
    m_pImmediateContext = pContext;
    return true;
}

bool    Scene::Init()
{
    return true;
}

bool    Scene::Frame()
{
    return true;
}

bool Scene::PreRender()
{
    return true;
}

bool    Scene::Render()
{
    //m_Writer.Draw(0, 0, I_Timer.m_szTimer);
    return true;
}

bool Scene::PostRender()
{
    return true;
}

bool    Scene::Release()
{
    return true;
}