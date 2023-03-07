#include "GameCore.h"
#include "Input.h"
#include "Timer.h"
#include "TextureMgr.h"
#include "ShaderMgr.h"
#include "SoundMgr.h"
#include "DXState.h"

HRESULT GameCore::CreateDxResource()
{
	m_Writer.Init();

	IDXGISurface1* pBackBuffer;
	m_pSwapChain->GetBuffer(0, __uuidof(IDXGISurface1), (void**)&pBackBuffer);
	m_Writer.Set(pBackBuffer);
	pBackBuffer->Release();
	return S_OK;
}

HRESULT GameCore::DeleteDxResource()
{
	m_Writer.DeleteDxResource();
	return S_OK;
}

bool	GameCore::Run()
{
	CoreInit();

	m_bGameRun = true;
	while (m_bGameRun)
	{
		if (MyWindows::Run())
		{
			if (!CoreFrame() || !CoreRender())
			{
				m_bGameRun = false;
			}
		}
		else
		{
			m_bGameRun = false;
		}
	}

	CoreRelease();

	return true;
}

bool	GameCore::CoreInit()
{
	m_isRelease = false;
	DXDevice::Init();
	I_Tex.SetDevice(m_pd3dDevice, m_pImmediateContext);
	I_Shader.SetDevice(m_pd3dDevice, m_pImmediateContext);
	DXState::SetState(m_pd3dDevice);

	I_Sound.Init();
	I_Input.Init();
	I_Timer.Init();

	CreateDxResource();

	{
		m_BG.m_VertexList.resize(4);
		m_BG.m_VertexList[0].p = { -1.0f, 1.0f, 0.0f };
		m_BG.m_VertexList[1].p = { +1.0f, 1.0f,  0.0f };
		m_BG.m_VertexList[2].p = { -1.0f, -1.0f, 0.0f };
		m_BG.m_VertexList[3].p = { 1.0f, -1.0f, 0.0f };

		m_BG.m_VertexList[0].c = { 1.0f, 1.0f, 1.0f, 1.0f };
		m_BG.m_VertexList[1].c = { 1.0f, 1.0f, 1.0f, 1.0f };
		m_BG.m_VertexList[2].c = { 1.0f, 1.0f, 1.0f, 1.0f };
		m_BG.m_VertexList[3].c = { 1.0f, 1.0f, 1.0f, 1.0f };

		m_BG.m_VertexList[0].t = { 0.0f, 0.0f };
		m_BG.m_VertexList[1].t = { 1.0f, 0.0f };
		m_BG.m_VertexList[2].t = { 0.0f, 1.0f };
		m_BG.m_VertexList[3].t = { 1.0f, 1.0f };

		m_BG.m_IndexList.resize(6);
		m_BG.m_IndexList[0] = 0;
		m_BG.m_IndexList[1] = 1;
		m_BG.m_IndexList[2] = 2;
		m_BG.m_IndexList[3] = 2;
		m_BG.m_IndexList[4] = 1;
		m_BG.m_IndexList[5] = 3;
	}

	m_BG.Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/DefaultRT.txt", L"../../data/_RAINBOW.bmp");
	m_RT.Create(m_pd3dDevice, m_pImmediateContext, 2048, 2048);

	return Init();
}

bool	GameCore::CoreFrame()
{
	I_Input.Frame();
	I_Timer.Frame();
	I_Sound.Frame();
	m_Writer.Frame();

	ClearD3D11DeviceContext();
	return Frame();
}

bool	GameCore::CorePreRender()
{
	m_pImmediateContext->OMSetRenderTargets(1, &m_pRTV, m_pDepthStencilView);
	float color[4] = { 0, 0, 0, 1.0f };
	m_pImmediateContext->ClearRenderTargetView(m_pRTV, color);
	m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView
		, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_pImmediateContext->RSSetViewports(1, &m_vp);
	m_pImmediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pImmediateContext->PSSetSamplers(0, 1, &DXState::g_pDefaultSSMirror);
	m_pImmediateContext->RSSetState(DXState::g_pDefaultRSSolid);
	m_pImmediateContext->OMSetBlendState(DXState::g_pAlphaBlend, 0, -1);
	m_pImmediateContext->OMSetDepthStencilState(DXState::g_pDefaultDepthStencil, 0xff);
	return true;
}

bool	GameCore::CoreRender()
{
	CorePreRender();
	//랜더 타겟 지정
	m_RT.m_pOldRTV = m_pRTV;
	m_RT.m_pOldDSV = m_pDepthStencilView;
	m_RT.m_vpOld[0] = m_vp;

	//ID3D11DeviceContext::OMSetRenderTargets: Resource being set to OM RenderTarget slot 0 is still bound on input
	if (m_RT.Begin(m_pImmediateContext))
	{
		Render();
		m_RT.End(m_pImmediateContext);
	}

	if (m_RT.m_pSRV)
	{
		m_BG.m_pTextureSRV = m_RT.m_pSRV.Get();
	}

	CorePostRender();
	return true;
}

bool	GameCore::CorePostRender()
{
	m_BG.SetMatrix(nullptr, nullptr, nullptr);
	m_BG.Render();

	I_Input.Render();
	I_Timer.Render();
	m_Writer.Render();

	m_pSwapChain->Present(0, 0);
	return true;
}

bool	GameCore::CoreRelease()
{
	m_RT.Release();
	m_BG.Release();
	Release();
	
	I_Sound.Release();
	I_Tex.Release();
	I_Shader.Release();
	I_Input.Release();
	I_Timer.Release();

	m_Writer.Release();

	DXState::Release();
	DXDevice::Release();
	m_isRelease = true;
	return true;
}
