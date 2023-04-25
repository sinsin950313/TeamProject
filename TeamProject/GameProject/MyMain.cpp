#include "MyMain.h"
#include "Input.h"
#include "SceneMgr.h"
#include "LightManager.h"

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
    m_MRT.Initialize_SetData(m_pd3dDevice, m_pImmediateContext);
    m_MRT.Init();

    m_screen.Initialize_SetData(m_pd3dDevice, m_pImmediateContext);
    m_screen.Init();
    m_screen.m_pConstantBuffer_Fog = DX::CreateConstantBuffer(m_pd3dDevice, &m_screen.m_ConstantData_Fog, sizeof(m_screen.m_ConstantData_Fog));

    I_Scene.SetDevice(m_pd3dDevice, m_pImmediateContext);
    I_Scene.Init();
    return true;
}

bool    MyMain::Frame()
{
    if (I_Input.GetKey(VK_ESCAPE) == KEY_PUSH)
        m_bGameRun = false;

    m_MRT.Frame(); 

    m_screen.Frame();
    
    I_Scene.Frame();
    return true;
}

bool    MyMain::Render()
{
    // Calculate Shadow Depth Map
    {
        auto lights = SSB::I_Light.GetLightList();
        for (auto light : lights)
        {
            light->PreRender();
            I_Scene.PreRender();
        }
    }

    // Calculate MRT data
    m_MRT.Render();
    I_Scene.Render();

    // Change m_RT.m_pTexture desc if test
	//m_pImmediateContext->CopyResource(m_RT.m_pTexture.Get(), m_MRT.m_pPositionTexture);

    // Draw
    {
		m_screen.SetMultiRenderTargetResult(m_MRT.GetResult());
        auto lights = SSB::I_Light.GetLightList();
        for (auto light : lights)
        {
            light->Render();
            m_screen.m_ConstantData_Fog.currentCameraPos = g_CurrentCameraPos;
            m_screen.m_ConstantData_Fog.linearFogStart = 30.0f;
            m_screen.m_ConstantData_Fog.linearFogEnd = 150.0f;
            m_screen.m_ConstantData_Fog.expFogDensity = 0.001f;
            m_pImmediateContext->UpdateSubresource(m_screen.m_pConstantBuffer_Fog, NULL, NULL, &m_screen.m_ConstantData_Fog, NULL, NULL);
            m_pImmediateContext->PSSetConstantBuffers(11, 1, &m_screen.m_pConstantBuffer_Fog);
            m_screen.Render();
        }
    }

	m_pImmediateContext->CopyResource(m_RT.m_pTexture.Get(), m_screen.GetRenderTargetTexture());

    ID3D11ShaderResourceView* pSRVs[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
    m_pImmediateContext->PSSetShaderResources(0, 16, pSRVs);
    m_pImmediateContext->OMSetRenderTargets(
        1, m_RT.m_pRenderTargetView.GetAddressOf(), m_MRT.GetDepthStencilView());
    I_Scene.SetCurrentRTT(m_RT.m_pRenderTargetView.GetAddressOf(), 1, m_MRT.GetDepthStencilView(), &m_RT.m_Viewport);

    ComPtr<ID3D11ShaderResourceView>	pSRV;
    if (SUCCEEDED(m_pd3dDevice->CreateShaderResourceView(m_screen.GetRenderTargetTexture(), NULL, pSRV.GetAddressOf())))
    {
        m_pImmediateContext->PSSetShaderResources(9, 1, pSRV.GetAddressOf());
    }

    I_Scene.PostRender();

    ClearD3D11DeviceContext();
	return true;
}

bool    MyMain::Release()
{
    I_Scene.Release();
    m_MRT.Release();
    m_screen.Release();
    return true;
}

GAME_RUN(MyMain, PROJECTNAME, 1600, 900)