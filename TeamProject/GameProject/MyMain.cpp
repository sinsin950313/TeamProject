#include "MyMain.h"
#include "Input.h"
#include "SceneMgr.h"

int		MyMain::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    //if (m_pMainCamera)
    //{
    //    m_pMainCamera->WndProc(hWnd, msg, wParam, lParam);
    //}
    return -1;
}

bool    MyMain::Init()
{
    I_Scene.SetDevice(m_pd3dDevice, m_pImmediateContext);
    I_Scene.Init();

    return true;
}

bool    MyMain::Frame()
{
    if (I_Input.GetKey(VK_ESCAPE) == KEY_PUSH)
        m_bGameRun = false;
    
    I_Scene.Frame();
    return true;
}

bool    MyMain::Render()
{
    I_Scene.Render();
    ClearD3D11DeviceContext();
	return true;
}

bool    MyMain::Release()
{
    I_Scene.Release();
    return true;
}

GAME_RUN(MyMain, PROJECTNAME, 1600, 900)