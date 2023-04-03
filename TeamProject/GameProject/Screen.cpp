#include "Screen.h"
#include "DXState.h"
#include "CommonPath.h"

namespace SSB
{
	void Screen::Initialize_SetData(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext, FLOAT fWidth, FLOAT fHeight)
	{
		m_pd3dDevice = pd3dDevice;
		m_pImmediateContext = pContext;
		m_fWidth = fWidth;
		m_fHeight = fHeight;
	}

	void	Screen::CreateVertexData()
	{
		if (m_VertexList.size() > 0)
			return;

		m_VertexList.resize(4);
		m_VertexList[0].p = { -1.0f, 1.0f, 0.0f };
		m_VertexList[0].uv = {0, 0};

		m_VertexList[1].p = { +1.0f, 1.0f,  0.0f };
		m_VertexList[1].uv = {1, 0};

		m_VertexList[2].p = { -1.0f, -1.0f, 0.0f };
		m_VertexList[2].uv = {0, 1};

		m_VertexList[3].p = { 1.0f, -1.0f, 0.0f };
		m_VertexList[3].uv = {1, 1};
	}
	void	Screen::CreateIndexData()
	{
		if (m_IndexList.size() > 0)
			return;
		// 정점버퍼에 인덱스
		m_IndexList.resize(6);
		m_IndexList[0] = 0;
		m_IndexList[1] = 1;
		m_IndexList[2] = 2;
		m_IndexList[3] = 2;
		m_IndexList[4] = 1;
		m_IndexList[5] = 3;
	}

	HRESULT Screen::CreateVertexBuffer()
	{
		HRESULT hr;

		CreateVertexData();

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = sizeof(ScreenVertex) * m_VertexList.size();
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = &m_VertexList.at(0);
		hr = m_pd3dDevice->CreateBuffer(
			&bd,
			&sd,
			&m_pVertexBuffer);

		return hr;
	}
	HRESULT Screen::CreateIndexBuffer()
	{
		HRESULT hr;

		CreateIndexData();

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = sizeof(DWORD) * m_IndexList.size();
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = &m_IndexList.at(0);
		hr = m_pd3dDevice->CreateBuffer(
			&bd,
			&sd,
			&m_pIndexBuffer);

		return hr;
	}

	HRESULT Screen::CreateVertexLayout()
	{
		HRESULT hr;
		D3D11_INPUT_ELEMENT_DESC ied[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TextureUV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
		UINT NumElements = sizeof(ied) / sizeof(ied[0]);
		hr = m_pd3dDevice->CreateInputLayout(
			ied,
			NumElements,
			m_pShader->m_pVSCode->GetBufferPointer(),
			m_pShader->m_pVSCode->GetBufferSize(),
			&m_pVertexLayout);

		return hr;
	}

	HRESULT Screen::CreateDepthStencilData()
	{
		HRESULT hr;

		// Depth Buffer
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

		if (FAILED(hr = m_pd3dDevice->CreateTexture2D(&DescDepth, NULL, &m_pDepthStencilTexture)))
		{
			return hr;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		if (FAILED(hr = m_pd3dDevice->CreateDepthStencilView(m_pDepthStencilTexture, &dsvDesc, &m_pDepthStencilView)))
		{
			return hr;
		}
	}

	HRESULT Screen::CreateRenderTargetData()
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

		if (FAILED(hr = m_pd3dDevice->CreateTexture2D(&desc, NULL, &m_pTexture)))
		{
			return hr;
		}

		if (FAILED(hr = m_pd3dDevice->CreateRenderTargetView(m_pTexture, NULL, &m_pRenderTargetView)))
		{
			return hr;
		}

	}

	void Screen::CreateShader()
	{
		I_Shader.Load(kShaderPath + L"ScreenShader.hlsl", L"VS", L"PS", &m_pShader);
	}

	void Screen::SetMultiRenderTargetResult(MultiRenderTargetResult result)
	{
		m_ShaderResourceList[0] = result.PositionRenderTargetView;
		m_ShaderResourceList[1] = result.NormalRenderTargetView;
		m_ShaderResourceList[2] = result.ColorRenderTargetView;
		m_ShaderResourceList[3] = result.DepthRenderTargetView;
	}

	bool Screen::Init()
	{
		CreateShader();
		CreateVertexData();
		CreateVertexBuffer();
		CreateIndexData();
		CreateIndexBuffer();
		CreateVertexLayout();
		CreateDepthStencilData();
		CreateRenderTargetData();

		return true;
	}

	bool Screen::Frame()
	{
		return true;
	}

	bool Screen::Render()
	{
		//ID3D11RenderTargetView* nullRTV[16];
		//m_pImmediateContext->OMSetRenderTargets(16, nullRTV, NULL);
		m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

		UINT stride = sizeof(ScreenVertex);
		UINT offset = 0;

		m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
		m_pImmediateContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		m_pImmediateContext->VSSetShader(m_pShader->m_pVS, NULL, 0);
		m_pImmediateContext->PSSetShader(m_pShader->m_pPS, NULL, 0);
		m_pImmediateContext->IASetInputLayout(m_pVertexLayout);

		m_pImmediateContext->PSSetShaderResources(0, m_ReferecedTextureCount, m_ShaderResourceList);
		m_pImmediateContext->PSSetSamplers(0, 1, &DXState::g_pDefaultSS);

		if (m_pIndexBuffer == nullptr)
			m_pImmediateContext->Draw(m_VertexList.size(), 0);
		else
			m_pImmediateContext->DrawIndexed(m_IndexList.size(), 0, 0);

		return true;
	}

	bool Screen::Release()
	{
		if (m_pVertexBuffer) m_pVertexBuffer->Release();
		if (m_pIndexBuffer)m_pIndexBuffer->Release();
		if (m_pVertexLayout)m_pVertexLayout->Release();

		if (m_pDepthStencilTexture)
		{
			m_pDepthStencilTexture->Release();
			m_pDepthStencilTexture = nullptr;
		}

		if (m_pDepthStencilView != nullptr)
		{
			m_pDepthStencilView->Release();
			m_pDepthStencilView = nullptr;
		}

		if (m_pShader != nullptr)
		{
			m_pShader = nullptr;
		}

		if (m_pTexture != nullptr)
		{
			m_pTexture->Release();
			m_pTexture = nullptr;
		}

		if (m_pRenderTargetView != nullptr)
		{
			m_pRenderTargetView->Release();
			m_pRenderTargetView = nullptr;
		}

		return true;
	}
	ID3D11Texture2D* Screen::GetRenderTargetTexture()
	{
		return m_pTexture;
	}
}
