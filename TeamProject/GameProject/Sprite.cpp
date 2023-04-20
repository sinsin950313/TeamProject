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
	return true;
}

bool Sprite::Frame()
{
	// UVRectList의 개수로 UV애니메이션 / 텍스쳐 애니메이션 구분
	// 프레임은 받아야

	BaseObject::Frame();

	//static int curMat = 0;
	//if (I_Input.GetKey('I') == KEY_PUSH)
	//{
	//	TVector3 pos = GetRandPos();
	//	D3DXMatrixTranslation(&m_matInstancing[curMat++], pos.x, pos.y, pos.z);
	//}
	
	return true;
}

bool Sprite::Render()
{
	BaseObject::Render();
	return true;
}

bool Sprite::RenderInstancing(UINT size)
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

	if (m_pIndexBuffer == nullptr)
		m_pImmediateContext->DrawInstanced(m_VertexList.size(), size, 0, 0);
	else
		m_pImmediateContext->DrawIndexedInstanced(m_IndexList.size(), size, 0, 0, 0);
	return true;
}

bool Sprite::Release()
{
	BaseObject::Release();
	return true;
}