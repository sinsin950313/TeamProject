#include "MyMain.h"
#include "Input.h"
#include "SceneMgr.h"
//#define _CRTDBG_MAP_ALLOC
//#include <cstdlib>
//#include <crtdbg.h>
//#ifdef _DEBUG
//#define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
//#endif

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
    /*_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetBreakAlloc(917542);*/
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