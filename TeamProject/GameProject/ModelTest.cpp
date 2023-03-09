#include "ModelTest.h"
#include "Input.h"

bool	ModelTest::Init()
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
			return false;
		}
	}

	return true;
}

void	ModelTest::UpdateBuffer()
{
	TMatrix world;
	TQuaternion q;
	D3DXQuaternionRotationYawPitchRoll(&q, m_vRotation.y, m_vRotation.x, m_vRotation.z);
	D3DXMatrixAffineTransformation(&world, &m_vScale, nullptr, &q, &m_vPos);
	D3DXMatrixTranspose(&_objectToWorldTransformData.World, &world);

	m_pImmediateContext->UpdateSubresource(_objectToWorldTransformBuffer, 0, nullptr, &_objectToWorldTransformData, 0, 0);
	m_pImmediateContext->UpdateSubresource(_toViewSpaceTransformBuffer, 0, nullptr, &_toViewSpaceTransformData, 0, 0);
}

bool	ModelTest::Frame()
{
	//_toViewSpaceTransformData.Position = g_dxWindow->GetMainCamera()->GetMatrix().Transpose();
	//_toViewSpaceTransformData.View = g_dxWindow->GetMainCamera()->GetViewMatrix().Transpose();
	//_toViewSpaceTransformData.Projection = g_dxWindow->GetMainCamera()->GetProjectionMatrix().Transpose();
	m_pModel->Frame();
	
	if (I_Input.GetKey('W') == KEY_HOLD)
	{
		m_vPos.z += 0.01f;
	}
	if (I_Input.GetKey('S') == KEY_HOLD)
	{
		m_vPos.z += -0.01f;
	}
	if (I_Input.GetKey('A') == KEY_HOLD)
	{
		m_vPos.x += -0.01f;
	}
	if (I_Input.GetKey('D') == KEY_HOLD)
	{
		m_vPos.x += 0.01f;
	}

	UpdateBuffer();

	return true;
}

bool	ModelTest::Render()
{
	m_pImmediateContext->VSSetConstantBuffers(0, 1, &_toViewSpaceTransformBuffer);
	m_pImmediateContext->VSSetConstantBuffers(1, 1, &_objectToWorldTransformBuffer);
	m_pModel->Render();
	return true;
}

bool	ModelTest::Release()
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

void	ModelTest::SetMatrix(TMatrix* matWorld, TMatrix* matView, TMatrix* matProj)
{
	if (!m_pModel)
		return;
	if (matWorld)
	{
		//_objectToWorldTransformData.World = *matWorld;
		D3DXMatrixTranspose(&_objectToWorldTransformData.World, &(*matWorld));
	}
	if (matView)
	{
		//_toViewSpaceTransformData.matView = *matView;
		D3DXMatrixTranspose(&_toViewSpaceTransformData.matView, &(*matView));
	}
	if (matProj)
	{
		//_toViewSpaceTransformData.matProj = *matProj;
		D3DXMatrixTranspose(&_toViewSpaceTransformData.matProj, &(*matProj));
	}
}