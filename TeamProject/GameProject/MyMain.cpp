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

    m_pModel = new SSB::Model();
    m_pModel->SetDevice(m_pd3dDevice, m_pImmediateContext);
    m_pModel->Deserialize(str);
    m_pModel->Init();

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
    return true;
}

bool    MyMain::Render()
{
    //setcameramatrix(view proj);
    m_pModel->Render();

    return true;
}

bool    MyMain::Release()
{
    if (m_pMainCamera)
    {
        m_pMainCamera->Release();
        delete m_pMainCamera;
    }
    if (m_pModel)
    {
        m_pModel->Release();
        delete m_pModel;
    }

    return true;
}

GAME_RUN(MyMain, PROJECTNAME, 1600, 900)