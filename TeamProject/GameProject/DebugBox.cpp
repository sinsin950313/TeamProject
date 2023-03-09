#include "DebugBox.h"

// 디바이스 컨텍스트 멤버 변수
// 지오메트리 쉐이더 생성
// 프레임 랜더 작성

// 객체의 포지션 로테이션 스케일 받아서 - SetBox
// 카메라 행렬 담고 SetMatrix
// 해당하는 값을 업데이트 후 랜더
bool	DebugBox::Create(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext)
{
	m_pd3dDevice = pd3dDevice;
	m_pImmediateContext = pContext;

	if (FAILED(CreateConstantBuffer()))
		return false;
	if (FAILED(CreateVertexBuffer()))
		return false;
	if (FAILED(CreateShader()))
		return false;
	if (FAILED(CreateVertexLayout()))
		return false;
	
	return true;
}

HRESULT DebugBox::CreateVertexBuffer()
{
	HRESULT hr;

	m_Vertex.p = TVector3(0, 0, 0);
	m_Vertex.c = TVector4(0, 0, 1, 0.2f);

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.ByteWidth = sizeof(PC_Vertex);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = &m_Vertex;
	hr = m_pd3dDevice->CreateBuffer(
		&bd,
		&sd,
		&m_pVertexBuffer);

	return hr;
}

HRESULT DebugBox::CreateConstantBuffer()
{
	HRESULT hr;

	m_cbBox.matView = TMatrix::Identity;
	m_cbBox.matProj = TMatrix::Identity;
	m_cbBox.matComb = TMatrix::Identity;
	m_cbBox.vXAxis = TVector4::UnitX;
	m_cbBox.vYAxis = TVector4::UnitY;
	m_cbBox.vZAxis = TVector4::UnitZ;
	m_cbBox.vSize = TVector4::One;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.ByteWidth = sizeof(BOX_CONSTANT_BUFFER) * 1;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = &m_cbBox;
	hr = m_pd3dDevice->CreateBuffer(
		&bd,
		&sd,
		&m_pConstantBuffer);

	return hr;
}

HRESULT DebugBox::CreateVertexLayout()
{
	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,12,D3D11_INPUT_PER_VERTEX_DATA, 0},
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

void		DebugBox::UpdateBuffer()
{
	m_pImmediateContext->UpdateSubresource(
		m_pVertexBuffer, 0, nullptr, &m_Vertex, 0, 0);
	m_pImmediateContext->UpdateSubresource(
		m_pConstantBuffer, 0, nullptr, &m_cbBox, 0, 0);
}

HRESULT	DebugBox::CreateShader(std::wstring filename)
{
	if (FAILED(I_Shader.Load(filename, &m_pShader)))
	{
		return E_FAIL;
	}
	if (FAILED(CreateGS(filename)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT	DebugBox::CreateGS(std::wstring filename)
{
	ID3DBlob* pErrorCode = nullptr;
	DWORD dwShaderFlags = 0;
#ifdef _DEBUG
	dwShaderFlags |= (D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION);
#endif

	std::string str = std::to_string(MAXLIGHTNUM);
	const D3D_SHADER_MACRO defines[] =
	{
		"EXAMPLE_DEFINE", "1",
		"MAXLIGHTNUM", str.c_str(),
		NULL, NULL
	};

	HRESULT hr = D3DCompileFromFile(
		filename.c_str(),
		defines,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"GS",
		"gs_5_0",
		dwShaderFlags,
		0,
		&m_pGSCode,
		&pErrorCode);
	if (FAILED(hr))
	{
		if (pErrorCode)
		{
			OutputDebugStringA((char*)pErrorCode->GetBufferPointer());
			pErrorCode->Release();
		}
		return hr;
	}
	hr = m_pd3dDevice->CreateGeometryShader(
		m_pGSCode->GetBufferPointer(),
		m_pGSCode->GetBufferSize(),
		NULL,
		&m_pGS);

	return hr;
}

void	DebugBox::SetBox(TVector3 pos, TVector3 rot, TVector3 size)
{
	m_vPos = pos;

	TQuaternion q;
	D3DXQuaternionRotationYawPitchRoll(&q, rot.y, rot.x, rot.z);
	TMatrix m;
	D3DXMatrixRotationQuaternion(&m, &q);

	D3DXVec3TransformNormal(&m_vAxis[0], &TVector3::UnitX, &m);
	D3DXVec3TransformNormal(&m_vAxis[1], &TVector3::UnitY, &m);
	D3DXVec3TransformNormal(&m_vAxis[2], &TVector3::UnitZ, &m);

	for (int i = 0; i < 3; i++)
	{
		m_fExtent[i] = size[i] * 0.5f;
	}

	m_Vertex.p = m_vPos;
	XMStoreFloat4(&m_cbBox.vXAxis, m_vAxis[0]);
	XMStoreFloat4(&m_cbBox.vYAxis, m_vAxis[1]);
	XMStoreFloat4(&m_cbBox.vZAxis, m_vAxis[2]);
	m_cbBox.vXAxis.w = 1.0f;
	m_cbBox.vYAxis.w = 1.0f;
	m_cbBox.vZAxis.w = 1.0f;

	m_cbBox.vSize = TVector4(m_fExtent);
	m_cbBox.vSize.w = 1.0f;
}

void	DebugBox::SetColor(TColor color)
{
	m_Vertex.c = color.ToVector4();
}

void	DebugBox::SetMatrix(TMatrix* matView, TMatrix* matProj)
{
	m_cbBox.matView = (*matView).Transpose();
	m_cbBox.matProj = (*matProj).Transpose();
	m_cbBox.matComb = ((*matView) * (*matProj)).Transpose();
}

bool	DebugBox::Init()
{
	return true;
}

bool	DebugBox::Frame()
{
	return true;
}

bool	DebugBox::Render()
{
	m_pImmediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	m_pImmediateContext->IASetInputLayout(m_pVertexLayout);
	m_pImmediateContext->VSSetShader(m_pShader->m_pVS, NULL, 0);
	m_pImmediateContext->GSSetShader(m_pGS, NULL, 0);
	m_pImmediateContext->PSSetShader(m_pShader->m_pPS, NULL, 0);
	UINT stride = sizeof(PC_Vertex); // 정점1개의 바이트용량
	UINT offset = 0; // 정점버퍼에서 출발지점(바이트)
	m_pImmediateContext->IASetVertexBuffers(0, 1,
		&m_pVertexBuffer, &stride, &offset);
	m_pImmediateContext->GSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	m_pImmediateContext->Draw(1, 0);

	m_pImmediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pImmediateContext->GSSetShader(NULL, NULL, 0);
	return true;
}

bool	DebugBox::Release()
{
	if (m_pGS) m_pGS->Release();
	if (m_pGSCode) m_pGSCode->Release();

	if (m_pVertexBuffer) m_pVertexBuffer->Release();
	if (m_pConstantBuffer) m_pConstantBuffer->Release();
	if (m_pVertexLayout) m_pVertexLayout->Release();
	return true;
}