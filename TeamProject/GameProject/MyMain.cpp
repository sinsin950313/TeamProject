#include "MyMain.h"
#include "Input.h"
#include "SoundMgr.h"
#include "FileIOObject.h"


bool    MyMain::Init()
{
    m_pMainCamera = new CCameraDebug;
    m_pMainCamera->CreateViewMatrix(TVector3(0, 0, -30), TVector3(0, 0, 0.1f), TVector3(0, 1, 0));
    m_pMainCamera->CreateProjMatrix(0.1f, 1500.0f, XM_PI * 0.25f
        , (float)g_rcClient.right / (float)g_rcClient.bottom);


    SSB::ObjectScriptIO io;
    std::string str = io.Read("ModelWriteTest_Box");

    m_pModelTest = new ModelTest();
    m_pModelTest->SetDevice(m_pd3dDevice, m_pImmediateContext);
    m_pModelTest->Init();
    
    m_pModelTest->m_pModel = new SSB::Model();
    m_pModelTest->m_pModel->SetDevice(m_pd3dDevice, m_pImmediateContext);
    m_pModelTest->m_pModel->Deserialize(str);
    m_pModelTest->m_pModel->Init();

    return true;
}

bool    MyMain::Frame()
{
    if (I_Input.GetKey(VK_ESCAPE) == KEY_PUSH)
        m_bGameRun = false;

    if (m_pMainCamera)
    {
        m_pMainCamera->Frame();
    }

    TMatrix matWorld = TMatrix::Identity;
    m_pModelTest->SetMatrix(&matWorld, &m_pMainCamera->m_matView, &m_pMainCamera->m_matProj);
    m_pModelTest->Frame();
    return true;
}

bool    MyMain::Render()
{
    //setcameramatrix(view proj);
    m_pModelTest->Render();

    return true;
}

bool    MyMain::Release()
{
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

    return true;
}

GAME_RUN(MyMain, PROJECTNAME, 1600, 900)