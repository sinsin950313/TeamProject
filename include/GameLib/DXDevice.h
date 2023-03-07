#pragma once
#include "MyWindows.h"

#include <d3d11.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

class DXDevice
	: public MyWindows
{
public:
	virtual bool    Init();
	virtual bool    Frame();
	virtual bool    Render();
	virtual bool    Release();

	virtual HRESULT ResizeDevice(UINT width, UINT height);
	virtual HRESULT CreateDxResource();
	virtual HRESULT DeleteDxResource();

public:
	// 1)디바이스 생성
	HRESULT CreateDevice();
	// 2) 팩토리 생성
	HRESULT	CreateDXGIDevice();
	// 3) 스왑체인 생성
	HRESULT CreateSwapChain();
	// 4) 랜더타겟뷰 생성
	HRESULT CreateRenderTargetView();
	// 5) 깊이 스텐실 뷰 생성
	HRESULT CreateDepthStencilView();
	// 6) 뷰포트 설정
	void CreateViewport();

	void ClearD3D11DeviceContext();

protected:
	ID3D11Device*				m_pd3dDevice = nullptr;
	ID3D11DeviceContext*		m_pImmediateContext = nullptr;
	IDXGIFactory*					m_pGIFactory = nullptr;


	IDXGISwapChain*				m_pSwapChain = nullptr;
	ID3D11RenderTargetView* m_pRTV = nullptr;
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;
	D3D11_VIEWPORT			m_vp;
};

