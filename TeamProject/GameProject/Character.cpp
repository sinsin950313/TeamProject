#include "Character.h"
#include "CollisionMgr.h"

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

SSB::OBBData Character::GetBoundingVolume()
{
	SSB::OBBData data = m_pModel->GetBoundingVolume();

	auto worldMatFloat44 = m_matWorld.operator DirectX::XMFLOAT4X4();
	FXMMATRIX worldMat = XMLoadFloat4x4(&worldMatFloat44);
	{
		auto result = XMVector3Transform(data.Position, worldMat);

		XMFLOAT4 tmp;
		XMStoreFloat4(&tmp, result);
		data.Position = { tmp.x, tmp.y, tmp.z };
	}

	{
		XMFLOAT4X4 tmp = data.Rotation;
		FXMMATRIX rotMat = XMLoadFloat4x4(&tmp);
		auto result = XMMatrixMultiply(worldMat, rotMat);

		XMStoreFloat4x4(&tmp, result);
		data.Rotation = tmp;
	}

	return data;
}

void Character::Scale(float val)
{
	m_vScale = TVector3(val, val, val);
}

bool	Character::Init()
{
	auto boundVolume = m_pModel->GetBoundingVolume();
    m_ColliderBox.CreateOBBBox(boundVolume.Width, boundVolume.Height, boundVolume.Depth);
    I_Collision.AddNpcBox(&m_ColliderBox, this);

	m_AttackBox.CreateOBBBox(1, 1, 1);
	m_AttackBoxLocalMatrix = TMatrix(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 100, -200, 1
	);

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
	UpdateBox();

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
	if (_objectToWorldTransformBuffer) _objectToWorldTransformBuffer->Release();
	if (_toViewSpaceTransformBuffer) _toViewSpaceTransformBuffer->Release();
	if (m_pModel)
	{
		m_pModel->Release();
		delete m_pModel;
		m_pModel = nullptr;
	}

	return true;
}

void	Character::UpdateBox()
{
	auto bv = m_pModel->GetBoundingVolume();
	TMatrix local = TMatrix::Identity;
	local._41 = bv.Position.x;
	local._42 = bv.Position.y;
	local._43 = bv.Position.z;
	TMatrix world = local * m_matWorld;
	m_ColliderBox.UpdateBox(world);

	TMatrix mat = m_AttackBoxLocalMatrix * m_matWorld;
	m_AttackBox.UpdateBox(mat);
}

void	Character::UpdateMatrix()
{
	TQuaternion q;
	D3DXQuaternionRotationYawPitchRoll(&q, m_vRotation.y, m_vRotation.x, m_vRotation.z);
	D3DXMatrixAffineTransformation(&m_matWorld, &m_vScale, nullptr, &q, &m_vPos);
}

TVector3 Character::GetCurSocketPos(std::string socket)
{
	TVector3 ret;
	TMatrix mat = m_pModel->GetSocketCurrentMatrix(socket);
	ret = TVector3(mat._41, mat._42, mat._43);
	D3DXVec3TransformCoord(&ret, &ret, &m_matWorld);
	return ret;
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
	UpdateBuffer();
}

void Character::MoveChar(XMVECTOR& destinationDirection, XMMATRIX& worldMatrix)
{
	float frameTime = g_fSecondPerFrame;
	//m_vOldDirection;

	destinationDirection = XMVector3Normalize(destinationDirection);

	if (XMVectorGetX(XMVector3Dot(destinationDirection, m_vOldDirection)) == -1)
		m_vOldDirection += XMVectorSet(0.4f, 0.0f, -0.4f, 0.0f);

	XMVECTOR charPosition = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	charPosition = XMVector3TransformCoord(charPosition, worldMatrix);

	float destDirLength = 20.0f * frameTime;

	XMVECTOR currCharDirection = (m_vOldDirection)+(destinationDirection * destDirLength);	// Get the characters direction (based off time, old position, and desired
	
	currCharDirection = XMVector3Normalize(currCharDirection);

	XMVECTOR DefaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	float charDirAngle = XMVectorGetX(XMVector3AngleBetweenNormals(XMVector3Normalize(currCharDirection), XMVector3Normalize(DefaultForward)));
	if (XMVectorGetY(XMVector3Cross(currCharDirection, DefaultForward)) > 0.0f)
		charDirAngle = -charDirAngle;

	float speed = m_fSpeed * frameTime;
	charPosition = charPosition + (destinationDirection * speed);

	//// Update characters world matrix
	//XMMATRIX rotationMatrix;
	////XMMATRIX Scale = XMMatrixScaling(0.25f, 0.25f, 0.25f);
	//XMMATRIX Scale = XMMatrixIdentity();
	//XMMATRIX Translation = XMMatrixTranslation(XMVectorGetX(charPosition), 0.0f, XMVectorGetZ(charPosition));
	//rotationMatrix = XMMatrixRotationY(charDirAngle - 3.14159265f);		// Subtract PI from angle so the character doesn't run backwards

	//m_vScale = TVector3(1, 1, 1);
	float ry = charDirAngle - M_PI;
	m_vRotation = TVector3(0, ry, 0);
	m_vPos = TVector3(XMVectorGetX(charPosition), 0, XMVectorGetZ(charPosition));

	// Set the characters old direction
	m_vOldDirection = currCharDirection;
	//m_vOldDirection = TVector3(XMVectorGetX(currCharDirection), XMVectorGetY(currCharDirection), XMVectorGetZ(currCharDirection));
	m_vDirection = TVector3(XMVectorGetX(currCharDirection), XMVectorGetY(currCharDirection), XMVectorGetZ(currCharDirection));

	// Update our animation
	float timeFactor = 1.0f;	// You can speed up or slow down time by changing this
	//UpdateMD5Model(NewMD5Model, time * timeFactor, 0);

	if (CollisionMgr::GetInstance().IsCollide(&m_ColliderBox))
	{
		charPosition = charPosition - (destinationDirection * (speed + 0.1f));
		m_vPos = TVector3(XMVectorGetX(charPosition), 0, XMVectorGetZ(charPosition));
		//oldCharDirection = currCharDirection;
		//m_vDirection = TVector3(XMVectorGetX(currCharDirection), XMVectorGetY(currCharDirection), XMVectorGetZ(currCharDirection));
	}
}

void Character::Initialize_SetPosition(TVector3 pos)
{
	m_vPos = pos;
}

TVector3 Character::GetPosition()
{
	XMMATRIX world = XMLoadFloat4x4(&m_matWorld);
	XMVECTOR charPosition = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	charPosition = XMVector3TransformCoord(charPosition, world);

	TVector3 pos;
	XMStoreFloat3(&pos, charPosition);

	return pos;
}

bool Character::IsDead()
{
	return m_HealthPoint <= 0;
}

void Character::DamagingCharacter(Character* character)
{
	m_DamagedCharacters.insert(character);
}

bool Character::IsAlreadyDamagedCurrentState(Character* character)
{
	return m_DamagedCharacters.find(character) != m_DamagedCharacters.end();
}

void Character::Damage(int damage)
{
	m_HealthPoint -= damage;
	if (!IsDead())
	{
		_damagedSound->Play();
	}
}

void Character::ResetStateElapseTime()
{
	m_fStateElapseTime = 0;
	m_fBeforeTime = g_fGameTimer;
	m_fStateTImeStamp = g_fGameTimer;
	m_DamagedCharacters.clear();
}

float Character::GetStateElapseTime()
{
	float elapseTime = g_fGameTimer - m_fBeforeTime;
	m_fStateElapseTime += elapseTime;
	m_fBeforeTime = g_fGameTimer;

	return m_fStateElapseTime;
}

float Character::GetStateTimeStamp()
{
	return m_fStateTImeStamp;
}
