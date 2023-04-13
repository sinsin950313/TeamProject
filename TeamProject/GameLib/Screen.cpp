#include "Screen.h"

namespace SSB
{
	bool	Screen::Create(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext)
	{
		m_pd3dDevice = pd3dDevice;
		m_pImmediateContext = pContext;
		if (FAILED(CreateVertexBuffer()))
			return false;
		if (FAILED(CreateIndexBuffer()))
			return false;
		if (FAILED(I_Shader.Load(shaderPath, VSname, PSname, &m_pShader)))
			return false;
		if (FAILED(CreateVertexLayout()))
			return false;
		I_Tex.Load(texPath, &m_pTexture);
		if (m_pTexture != nullptr)
		{
			m_pTextureSRV = m_pTexture->m_pTextureSRV;
		}
		else
		{
			m_szPSname = L"COLOR_PS";
			if (FAILED(I_Shader.Load(shaderPath, VSname, m_szPSname, &m_pShader)))
				return false;
			return true;
		}

		return true;
	}

	void	Screen::CreateVertexData()
	{
		if (m_VertexList.size() > 0)
			return;

		m_VertexList.resize(4);
		m_VertexList[0].p = { -1.0f, 1.0f, 0.0f };
		m_VertexList[1].p = { +1.0f, 1.0f,  0.0f };
		m_VertexList[2].p = { -1.0f, -1.0f, 0.0f };
		m_VertexList[3].p = { 1.0f, -1.0f, 0.0f };
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

	bool Screen::Init()
	{
		return true;
	}

	bool Screen::Frame()
	{
		return true;
	}

	bool Screen::PreRender()
	{
		UINT stride = sizeof(ScreenVertex);
		UINT offset = 0;

		m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
		m_pImmediateContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		m_pImmediateContext->VSSetShader(m_pShader->m_pVS, NULL, 0);
		m_pImmediateContext->PSSetShader(m_pShader->m_pPS, NULL, 0);
		m_pImmediateContext->IASetInputLayout(m_pVertexLayout);

		m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTextureSRV);

		return true;
	}

	bool Screen::Render()
	{
		PreRender();
		PostRender();

		return true;
	}

	bool Screen::PostRender()
	{
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
		return true;
	}
}
