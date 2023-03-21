#include "BaseObject.h"
namespace DX
{
	ID3D11Buffer* CreateVertexBuffer(ID3D11Device* pd3dDevice,
		void* pDataAddress,
		UINT iNumVertex,
		UINT iVertexSize)
	{
		HRESULT hr;
		ID3D11Buffer* pVB = nullptr;
		D3D11_BUFFER_DESC       bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = iVertexSize * iNumVertex; // 바이트 용량
		// GPU 메모리에 할당
		bd.Usage = D3D11_USAGE_DEFAULT; // 버퍼의 할당 장소 내지는 버퍼용도
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA  sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = pDataAddress;
		hr = pd3dDevice->CreateBuffer(
			&bd, // 버퍼 할당
			&sd, // 초기 할당된 버퍼를 체우는 CPU메모리 주소
			&pVB);
		return pVB;
	}
	ID3D11Buffer* CreateIndexBuffer(ID3D11Device* pd3dDevice,
		void* pDataAddress,
		UINT iDataNum,
		UINT iDataSize)
	{
		HRESULT hr;
		ID3D11Buffer* pVB = nullptr;

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = iDataSize * iDataNum;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = pDataAddress;
		hr = pd3dDevice->CreateBuffer(
			&bd,
			&sd,
			&pVB);

		return pVB;
	}
	ID3D11Buffer* CreateConstantBuffer(ID3D11Device* pd3dDevice,
		void* pDataAddress,
		UINT iDataSize)
	{
		ID3D11Buffer* pVB = nullptr;

		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = iDataSize;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA  sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = pDataAddress;
		HRESULT hr = pd3dDevice->CreateBuffer(
			&bd, // 버퍼 할당
			&sd, // 초기 할당된 버퍼를 체우는 CPU메모리 주소
			&pVB);

		if (SUCCEEDED(hr))
			return pVB;
		else
			return nullptr;
	}
	bool	BaseObject::Create(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext, std::wstring shaderPath, std::wstring texPath,
		std::wstring VSname, std::wstring PSname)
	{
		m_pd3dDevice = pd3dDevice;
		m_pImmediateContext = pContext;
		m_szTexPath = texPath;
		m_szShaderPath = shaderPath;
		m_szVSname = VSname;
		m_szPSname = PSname;
		if (FAILED(CreateConstantBuffer()))
			return false;
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

	void	BaseObject::CreateVertexData()
	{
		if (m_VertexList.size() > 0)
			return;

		m_VertexList.resize(4);
		m_VertexList[0].p = { -1.0f, 1.0f, 0.0f };
		m_VertexList[1].p = { +1.0f, 1.0f,  0.0f };
		m_VertexList[2].p = { -1.0f, -1.0f, 0.0f };
		m_VertexList[3].p = { 1.0f, -1.0f, 0.0f };

		m_VertexList[0].c = { 1.0f, 1.0f, 1.0f, 1.0f };
		m_VertexList[1].c = { 1.0f, 1.0f, 1.0f, 1.0f };
		m_VertexList[2].c = { 1.0f, 1.0f, 1.0f, 1.0f };
		m_VertexList[3].c = { 1.0f, 1.0f, 1.0f, 1.0f };

		m_VertexList[0].t = { 0.0f, 0.0f };
		m_VertexList[1].t = { 1.0f, 0.0f };
		m_VertexList[2].t = { 0.0f, 1.0f };
		m_VertexList[3].t = { 1.0f, 1.0f };

		m_VertexList[0].n = { 0.0f, 0.0f, -1.0f };
		m_VertexList[1].n = { 0.0f, 0.0f, -1.0f };
		m_VertexList[2].n = { 0.0f, 0.0f, -1.0f };
		m_VertexList[3].n = { 0.0f, 0.0f, -1.0f };
	}
	void	BaseObject::CreateIndexData()
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

	void	BaseObject::CreateConstantData()
	{
		m_cbData.matWorld = TMatrix::Identity;
		m_cbData.matView = TMatrix::Identity;
		m_cbData.matProj = TMatrix::Identity;
		m_cbData.matComb = TMatrix::Identity;
		m_cbData.vTemp = { 0, 0, 0 };
		m_cbData.fTimer = 0.0f;
	}

	HRESULT BaseObject::CreateVertexBuffer()
	{
		HRESULT hr;

		CreateVertexData();

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = sizeof(Vertex) * m_VertexList.size();
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
	HRESULT BaseObject::CreateIndexBuffer()
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

	HRESULT BaseObject::CreateConstantBuffer()
	{
		HRESULT hr;

		CreateConstantData();

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = sizeof(VS_CONSTANT_BUFFER) * 1;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = &m_cbData;
		hr = m_pd3dDevice->CreateBuffer(
			&bd,
			&sd,
			&m_pConstantBuffer);

		return hr;
	}

	HRESULT BaseObject::CreateVertexLayout()
	{
		HRESULT hr;
		D3D11_INPUT_ELEMENT_DESC ied[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,24,D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,40,D3D11_INPUT_PER_VERTEX_DATA, 0},
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

	void	BaseObject::SetMatrix(TMatrix* matWorld, TMatrix* matView, TMatrix* matProj)
	{
		if (matWorld != nullptr)
		{
			m_matWorld = *matWorld;
		}
		if (matView != nullptr)
		{
			m_matView = *matView;
		}
		if (matProj != nullptr)
		{
			m_matProj = *matProj;
		}
		UpdateConstantBuffer();
	}

	bool BaseObject::Init()
	{
		return true;
	}

	bool BaseObject::Frame()
	{
		UpdateVertexBuffer();
		UpdateConstantBuffer();
		return true;
	}

	bool BaseObject::PreRender()
	{
		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
		m_pImmediateContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		m_pImmediateContext->VSSetShader(m_pShader->m_pVS, NULL, 0);
		m_pImmediateContext->PSSetShader(m_pShader->m_pPS, NULL, 0);
		m_pImmediateContext->IASetInputLayout(m_pVertexLayout);

		m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTextureSRV);

		m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);

		return true;
	}

	bool BaseObject::Render()
	{
		PreRender();
		PostRender();

		return true;
	}

	bool BaseObject::PostRender()
	{
		if (m_pIndexBuffer == nullptr)
			m_pImmediateContext->Draw(m_VertexList.size(), 0);
		else
			m_pImmediateContext->DrawIndexed(m_IndexList.size(), 0, 0);
		return true;
	}

	void BaseObject::UpdateVertexBuffer()
	{
		m_pImmediateContext->UpdateSubresource(
			m_pVertexBuffer, 0, nullptr, &m_VertexList.at(0), 0, 0);
	}

	void	BaseObject::UpdateConstantBuffer()
	{
		m_cbData.matComb = m_matWorld * m_matView * m_matProj;

		m_cbData.matWorld = m_matWorld.Transpose();
		m_cbData.matView = m_matView.Transpose();
		m_cbData.matProj = m_matProj.Transpose();
		m_cbData.matComb = m_cbData.matComb.Transpose();

		D3DXMatrixInverse(&m_cbData.matNormal, nullptr, &m_matWorld);
		m_cbData.matNormal = m_cbData.matNormal.Transpose();

		m_pImmediateContext->UpdateSubresource(
			m_pConstantBuffer, 0, nullptr, &m_cbData, 0, 0);
	}

	bool BaseObject::Release()
	{
		if (m_pConstantBuffer) m_pConstantBuffer->Release();
		if (m_pVertexBuffer) m_pVertexBuffer->Release();
		if (m_pIndexBuffer)m_pIndexBuffer->Release();
		if (m_pVertexLayout)m_pVertexLayout->Release();
		return true;
	}
};
