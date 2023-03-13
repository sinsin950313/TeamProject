#include "Character.h"

void	Character::SetDevice(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pd3dDevice = pDevice;
	m_pImmediateContext = pContext;
	CreateBuffer();
}

void	Character::CreateBuffer()
{
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(ToViewSpaceTransformData);
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));
		sd.pSysMem = &_toViewSpaceTransformData;
		HRESULT hr = m_pd3dDevice->CreateBuffer(&desc, &sd, &_toViewSpaceTransformBuffer);
		if (FAILED(hr))
		{
			assert(SUCCEEDED(hr));
		}
	}
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(ObjectToWorldTransformData);
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));
		sd.pSysMem = &_objectToWorldTransformData;
		HRESULT hr = m_pd3dDevice->CreateBuffer(&desc, &sd, &_objectToWorldTransformBuffer);
		if (FAILED(hr))
		{
			assert(SUCCEEDED(hr));
		}
	}
}

void	Character::UpdateBuffer()
{
	D3DXMatrixTranspose(&_objectToWorldTransformData.World, &m_matWorld);
	m_pImmediateContext->UpdateSubresource(_objectToWorldTransformBuffer, 0, nullptr, &_objectToWorldTransformData, 0, 0);

	D3DXMatrixTranspose(&_toViewSpaceTransformData.matView, &m_matView);
	D3DXMatrixTranspose(&_toViewSpaceTransformData.matProj, &m_matProj);
	m_pImmediateContext->UpdateSubresource(_toViewSpaceTransformBuffer, 0, nullptr, &_toViewSpaceTransformData, 0, 0);
}

bool	Character::Init()
{
	return true;
}

bool	Character::Frame()
{
	//_toViewSpaceTransformData.Position = g_dxWindow->GetMainCamera()->GetMatrix().Transpose();
	//_toViewSpaceTransformData.View = g_dxWindow->GetMainCamera()->GetViewMatrix().Transpose();
	//_toViewSpaceTransformData.Projection = g_dxWindow->GetMainCamera()->GetProjectionMatrix().Transpose();
	m_pModel->Frame();

	UpdateMatrix();
	UpdateBuffer();

	return true;
}

bool	Character::Render()
{
	m_pImmediateContext->VSSetConstantBuffers(0, 1, &_toViewSpaceTransformBuffer);
	m_pImmediateContext->VSSetConstantBuffers(1, 1, &_objectToWorldTransformBuffer);
	m_pModel->Render();
	return true;
}

bool	Character::Release()
{
	if (m_pModel)
	{
		m_pModel->Release();
		delete m_pModel;
	}

	if (_objectToWorldTransformBuffer) _objectToWorldTransformBuffer->Release();
	if (_toViewSpaceTransformBuffer) _toViewSpaceTransformBuffer->Release();

	return true;
}

void	Character::UpdateMatrix()
{
	TQuaternion q;
	D3DXQuaternionRotationYawPitchRoll(&q, m_vRotation.y, m_vRotation.x, m_vRotation.z);
	D3DXMatrixAffineTransformation(&m_matWorld, &m_vScale, nullptr, &q, &m_vPos);
}

void	Character::SetMatrix(TMatrix* matWorld, TMatrix* matView, TMatrix* matProj)
{
	if (!m_pModel)
		return;
	if (matWorld)
	{
		m_matWorld = *matWorld;
	}
	if (matView)
	{
		m_matView = *matView;
	}
	if (matProj)
	{
		m_matProj = *matProj;
	}
}