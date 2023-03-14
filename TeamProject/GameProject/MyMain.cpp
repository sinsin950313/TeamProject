#include "MyMain.h"
#include "Input.h"
#include "SoundMgr.h"
#include "FileIOObject.h"
#include "Player.h"

int		MyMain::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (m_pMainCamera)
    {
        m_pMainCamera->WndProc(hWnd, msg, wParam, lParam);
    }
    return -1;
}

bool    MyMain::Init()
{
    m_pMainCamera = new CameraTPS;
    //m_pMainCamera = new CameraDebug;
    //m_pMainCamera = new CCamera;
    m_pMainCamera->CreateViewMatrix(TVector3(0, 0, -30), TVector3(0, 0, 0.1f), TVector3(0, 1, 0));
    m_pMainCamera->CreateProjMatrix(0.1f, 1500.0f, XM_PI * 0.25f
        , (float)g_rcClient.right / (float)g_rcClient.bottom);


    SSB::ObjectScriptIO io;
    std::string str = io.Read("ModelWriteTest_Man");

    m_pModelTest = new Player();
    m_pModelTest->SetDevice(m_pd3dDevice, m_pImmediateContext);
    ((Player*)m_pModelTest)->m_pMainCamera = m_pMainCamera;
    m_pModelTest->Init();
    ((CameraTPS*)m_pMainCamera)->m_vFollowPos = &m_pModelTest->m_vPos;
    
    m_pModelTest->m_pModel = new SSB::Model();
    m_pModelTest->m_pModel->SetDevice(m_pd3dDevice, m_pImmediateContext);
    m_pModelTest->m_pModel->Deserialize(str);
    m_pModelTest->m_pModel->Init();
    m_pModelTest->m_pModel->SetCurrentAnimation("Take 001");

    m_pDebugBox = new DebugBox;
    m_pDebugBox->Create(m_pd3dDevice, m_pImmediateContext);

    m_pQuadTree = MAPLOAD::OpenMap(L"../../data/map/temp_8_8.map", m_pd3dDevice, m_pImmediateContext);

    return true;
}

bool    MyMain::Frame()
{
    if (I_Input.GetKey(VK_ESCAPE) == KEY_PUSH)
        m_bGameRun = false;

    if (m_pMainCamera)
    {
        m_pMainCamera->Frame();

        m_pModelTest->SetMatrix(nullptr, &m_pMainCamera->m_matView, &m_pMainCamera->m_matProj);
    }

    m_pQuadTree->Update();
    m_pModelTest->Frame();
    return true;
}

bool    MyMain::Render()
{
    m_pQuadTree->SetMatrix(nullptr, &m_pMainCamera->m_matView, &m_pMainCamera->m_matProj);
    m_pQuadTree->Render();

    //TMatrix matWorld = TMatrix::Identity;
    //m_pModelTest->SetMatrix(&matWorld, &m_pMainCamera->m_matView, &m_pMainCamera->m_matProj);
    //m_pModelTest->Frame();
    m_pModelTest->Render();

    if (m_pDebugBox)
    {
		m_pDebugBox->SetBox(TVector3(0, 0, 0), TVector3::Zero, TVector3::One);
		m_pDebugBox->SetMatrix(&m_pMainCamera->m_matView, &m_pMainCamera->m_matProj);
        m_pDebugBox->SetColor(TColor(0, 0, 1, 1));
		m_pDebugBox->UpdateBuffer();
		m_pDebugBox->Render();
    }

    return true;
}

bool    MyMain::Release()
{
    if (m_pQuadTree)
    {
        m_pQuadTree->Release();
        delete m_pQuadTree;
        m_pQuadTree = nullptr;
    }

    if (m_pMainCamera)
    {
        m_pMainCamera->Release();
        delete m_pMainCamera;
    }
    
    if (m_pModelTest)
    {
        m_pModelTest->Release();
        delete m_pModelTest;
    }

    if (m_pDebugBox)
    {
        m_pDebugBox->Release();
        delete m_pDebugBox;
    }

    return true;
}

GAME_RUN(MyMain, PROJECTNAME, 1600, 900)