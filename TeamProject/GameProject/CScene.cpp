#include "CScene.h"

bool    CScene::Create(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext)
{
    m_pd3dDevice = pd3dDevice;
    m_pImmediateContext = pContext;
    return true;
}

bool    CScene::Init()
{
    return true;
}

bool    CScene::Frame()
{
    return true;
}

bool    CScene::Render()
{
    //m_Writer.Draw(0, 0, I_Timer.m_szTimer);
    return true;
}

bool    CScene::Release()
{
    return true;
}