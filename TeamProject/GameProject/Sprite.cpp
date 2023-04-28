#include "Sprite.h"
#include "DXState.h"
#include "Input.h"

Sprite::Sprite()
{

}

Sprite::~Sprite()
{
}

bool Sprite::Init()
{
	BaseObject::Init();

	I_Shader.VSLoad(L"../../data/shader/DefaultParticle.hlsl", L"VS", &m_pSwapVS[0]);
	I_Shader.VSLoad(L"../../data/shader/DefaultParticle.hlsl", L"RibbonVS", &m_pSwapVS[1]);

	I_Shader.PSLoad(L"../../data/shader/DefaultParticle.hlsl", L"PS", &m_pSwapPS[0]);
	I_Shader.PSLoad(L"../../data/shader/DefaultParticle.hlsl", L"Distortion", &m_pSwapPS[1]);
	I_Shader.PSLoad(L"../../data/shader/DefaultParticle.hlsl", L"COLOR_PS", &m_pSwapPS[2]);
	I_Shader.PSLoad(L"../../data/shader/DefaultParticle.hlsl", L"OPACITY_PS", &m_pSwapPS[3]);

	m_pSwapGS[0] = NULL;
	I_Shader.GSLoad(L"../../data/shader/DefaultParticle.hlsl", L"RibbonGS", &m_pSwapGS[1]);
	return true;
}

bool Sprite::Frame()
{
	// UVRectList의 개수로 UV애니메이션 / 텍스쳐 애니메이션 구분
	// 프레임은 받아야

	//UpdateMatrix();
	BaseObject::Frame();

	//static int curMat = 0;
	//if (I_Input.GetKey('I') == KEY_PUSH)
	//{
	//	TVector3 pos = GetRandPos();
	//	D3DXMatrixTranslation(&m_matInstancing[curMat++], pos.x, pos.y, pos.z);
	//}
	
	return true;
}
bool Sprite::PreRender()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	m_pImmediateContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	m_pImmediateContext->VSSetShader(m_pVS ? m_pVS->m_pVS : NULL, NULL, 0);
	m_pImmediateContext->GSSetShader(m_pGS ? m_pGS->m_pGS : NULL, NULL, 0);
	m_pImmediateContext->PSSetShader(m_pPS ? m_pPS->m_pPS : NULL, NULL, 0);

	m_pImmediateContext->IASetInputLayout(m_pVertexLayout);

	m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTextureSRV);

	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	m_pImmediateContext->RSSetState(DXState::g_pNonCullRSSolid);
	return true;
}

bool Sprite::Render()
{
	PreRender();
	PostRender();
	return true;
}

bool Sprite::RenderInstancing(UINT size)
{
	PreRender();

	if (m_pIndexBuffer == nullptr)
		m_pImmediateContext->DrawInstanced(m_VertexList.size(), size, 0, 0);
	else
		m_pImmediateContext->DrawIndexedInstanced(m_IndexList.size(), size, 0, 0, 0);

	m_pImmediateContext->RSSetState(DXState::g_pDefaultRSSolid);
	return true;
}

bool Sprite::PostRender()
{
	BaseObject::PostRender();
	m_pImmediateContext->RSSetState(DXState::g_pDefaultRSSolid);
	return true;
}

bool Sprite::Release()
{
	BaseObject::Release();
	return true;
}