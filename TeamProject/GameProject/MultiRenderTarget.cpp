#include "MultiRenderTarget.h"

namespace SSB
{
	bool MultiRenderTarget::Initialize_SetData(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext, FLOAT fWidth, FLOAT fHeight)
	{
		m_pd3dDevice = pd3dDevice;
		m_pImmediateContext = pContext;
		m_fWidth = fWidth;
		m_fHeight = fHeight;

		return true;
	}
	MultiRenderTargetResult MultiRenderTarget::GetResult()
	{
		return { m_pPositionShaderResourceView, m_pNormalShaderResourceView, m_pColorShaderResourceView, m_pDepthStencilShaderResourceView };
	}
	bool MultiRenderTarget::Init()
	{
		CreatePositionTexture();
		CreateNormalTexture();
		CreateColorTexture();
		//CreateDepthTexture();
		CreateDepthStencil();
		CreateViewPort();

		return true;
	}
	bool MultiRenderTarget::Frame()
	{
		ID3D11RenderTargetView* pRTV[] = { m_pPositionRenderTargetView, m_pNormalRenderTargetView, m_pColorRenderTargetView };
		m_pImmediateContext->OMSetRenderTargets(3, pRTV, m_pDepthStencilView);
		m_pImmediateContext->RSSetViewports(1, &m_Viewport);
		ClearRenderTargets();

		return true;
	}
	bool MultiRenderTarget::Render()
	{
		ID3D11RenderTargetView* pRTV[] = { m_pPositionRenderTargetView, m_pNormalRenderTargetView, m_pColorRenderTargetView };
		m_pImmediateContext->OMSetRenderTargets(3, pRTV, m_pDepthStencilView);
		m_pImmediateContext->RSSetViewports(1, &m_Viewport);

		return true;
	}
	bool MultiRenderTarget::Release()
	{
		if (m_pPositionTexture != nullptr)
		{
			m_pPositionTexture->Release();
			m_pPositionTexture = nullptr;
		}

		if (m_pNormalTexture != nullptr)
		{
			m_pNormalTexture->Release();
			m_pNormalTexture = nullptr;
		}

		if (m_pColorTexture != nullptr)
		{
			m_pColorTexture->Release();
			m_pColorTexture = nullptr;
		}

		//if (m_pDepthTexture != nullptr)
		//{
		//	m_pDepthTexture->Release();
		//	m_pDepthTexture = nullptr;
		//}

		if (m_pPositionRenderTargetView != nullptr)
		{
			m_pPositionRenderTargetView->Release();
			m_pPositionRenderTargetView = nullptr;
		}

		if (m_pNormalRenderTargetView != nullptr)
		{
			m_pNormalRenderTargetView->Release();
			m_pNormalRenderTargetView = nullptr;
		}

		if (m_pColorRenderTargetView != nullptr)
		{
			m_pColorRenderTargetView->Release();
			m_pColorRenderTargetView = nullptr;
		}

		//if (m_pDepthRenderTargetView != nullptr)
		//{
		//	m_pDepthRenderTargetView->Release();
		//	m_pDepthRenderTargetView = nullptr;
		//}

		if (m_pPositionShaderResourceView != nullptr)
		{
			m_pPositionShaderResourceView->Release();
			m_pPositionShaderResourceView = nullptr;
		}

		if (m_pNormalShaderResourceView != nullptr)
		{
			m_pNormalShaderResourceView->Release();
			m_pNormalShaderResourceView = nullptr;
		}

		if (m_pColorShaderResourceView != nullptr)
		{
			m_pColorShaderResourceView->Release();
			m_pColorShaderResourceView = nullptr;
		}

		if (m_pDepthStencilView != nullptr)
		{
			m_pDepthStencilView->Release();
			m_pDepthStencilView = nullptr;
		}

		if (m_pDepthStencilShaderResourceView != nullptr)
		{
			m_pDepthStencilShaderResourceView->Release();
			m_pDepthStencilShaderResourceView = nullptr;
		}

		if (m_pPixelShader != nullptr)
		{
			m_pPixelShader->Release();
			m_pPixelShader = nullptr;
		}

		return true;
	}
	HRESULT MultiRenderTarget::CreatePositionTexture()
	{
		HRESULT hr;

		D3D11_TEXTURE2D_DESC desc;
		desc.Width = (UINT)m_fWidth;
		desc.Height = (UINT)m_fHeight;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.ArraySize = 1;

		if (FAILED(hr = m_pd3dDevice->CreateTexture2D(&desc, NULL, &m_pPositionTexture)))
		{
			return hr;
		}

		if (FAILED(hr = m_pd3dDevice->CreateShaderResourceView(m_pPositionTexture, NULL, &m_pPositionShaderResourceView)))
		{
			return hr;
		}

		if (FAILED(hr = m_pd3dDevice->CreateRenderTargetView(m_pPositionTexture, NULL, &m_pPositionRenderTargetView)))
		{
			return hr;
		}
	}
	HRESULT MultiRenderTarget::CreateNormalTexture()
	{
		HRESULT hr;

		D3D11_TEXTURE2D_DESC desc;
		desc.Width = (UINT)m_fWidth;
		desc.Height = (UINT)m_fHeight;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.ArraySize = 1;

		if (FAILED(hr = m_pd3dDevice->CreateTexture2D(&desc, NULL, &m_pNormalTexture)))
		{
			return hr;
		}

		if (FAILED(hr = m_pd3dDevice->CreateShaderResourceView(m_pNormalTexture, NULL, &m_pNormalShaderResourceView)))
		{
			return hr;
		}

		if (FAILED(hr = m_pd3dDevice->CreateRenderTargetView(m_pNormalTexture, NULL, &m_pNormalRenderTargetView)))
		{
			return hr;
		}
	}
	HRESULT MultiRenderTarget::CreateColorTexture()
	{
		HRESULT hr;

		D3D11_TEXTURE2D_DESC desc;
		desc.Width = (UINT)m_fWidth;
		desc.Height = (UINT)m_fHeight;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.ArraySize = 1;

		if (FAILED(hr = m_pd3dDevice->CreateTexture2D(&desc, NULL, &m_pColorTexture)))
		{
			return hr;
		}

		if (FAILED(hr = m_pd3dDevice->CreateShaderResourceView(m_pColorTexture, NULL, &m_pColorShaderResourceView)))
		{
			return hr;
		}

		if (FAILED(hr = m_pd3dDevice->CreateRenderTargetView(m_pColorTexture, NULL, &m_pColorRenderTargetView)))
		{
			return hr;
		}
	}
	void MultiRenderTarget::CreateViewPort()
	{
		m_Viewport.Width = m_fWidth;
		m_Viewport.Height = m_fHeight;
		m_Viewport.TopLeftX = 0;
		m_Viewport.TopLeftY = 0;
		m_Viewport.MinDepth = 0.0f;
		m_Viewport.MaxDepth = 1.0f;
	}
	//HRESULT MultiRenderTarget::CreateDepthTexture()
	//{
	//	HRESULT hr;

	//	D3D11_TEXTURE2D_DESC desc;
	//	desc.Width = (UINT)m_fWidth;
	//	desc.Height = (UINT)m_fHeight;
	//	desc.MipLevels = 1;
	//	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//	desc.SampleDesc.Count = 1;
	//	desc.SampleDesc.Quality = 0;
	//	desc.Usage = D3D11_USAGE_DEFAULT;
	//	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	//	desc.CPUAccessFlags = 0;
	//	desc.MiscFlags = 0;
	//	desc.ArraySize = 1;

	//	if (FAILED(hr = m_pd3dDevice->CreateTexture2D(&desc, NULL, &m_pDepthTexture)))
	//	{
	//		return hr;
	//	}

	//	if (FAILED(hr = m_pd3dDevice->CreateShaderResourceView(m_pDepthTexture, NULL, &m_pDepthShaderResourceView)))
	//	{
	//		return hr;
	//	}

	//	if (FAILED(hr = m_pd3dDevice->CreateRenderTargetView(m_pDepthTexture, NULL, &m_pDepthRenderTargetView)))
	//	{
	//		return hr;
	//	}
	//}
	HRESULT MultiRenderTarget::CreateDepthStencil()
	{
		HRESULT hr;

		// Depth Buffer
		ComPtr<ID3D11Texture2D> pDSTexture = nullptr;
		D3D11_TEXTURE2D_DESC DescDepth;
		DescDepth.Width = m_fWidth;
		DescDepth.Height = m_fHeight;
		DescDepth.MipLevels = 1;
		DescDepth.ArraySize = 1;
		DescDepth.Format = DXGI_FORMAT_R24G8_TYPELESS;
		DescDepth.SampleDesc.Count = 1;
		DescDepth.SampleDesc.Quality = 0;
		DescDepth.Usage = D3D11_USAGE_DEFAULT;
		DescDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		DescDepth.CPUAccessFlags = 0;
		DescDepth.MiscFlags = 0;
		if (FAILED(hr = m_pd3dDevice->CreateTexture2D(&DescDepth, NULL, &pDSTexture)))
		{
			return hr;
		}
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		if (FAILED(hr = m_pd3dDevice->CreateDepthStencilView(pDSTexture.Get(), &dsvDesc, &m_pDepthStencilView)))
		{
			return hr;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		if (FAILED(hr = m_pd3dDevice->CreateShaderResourceView(pDSTexture.Get(), &srvDesc, &m_pDepthStencilShaderResourceView)))
		{
			return hr;
		}
	}
	void MultiRenderTarget::ClearRenderTargets()
	{
		const FLOAT color[] = { 0, 0, 0, 0 };
		m_pImmediateContext->ClearRenderTargetView(m_pPositionRenderTargetView, color);
		m_pImmediateContext->ClearRenderTargetView(m_pNormalRenderTargetView, color);
		m_pImmediateContext->ClearRenderTargetView(m_pColorRenderTargetView, color);

		m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
}