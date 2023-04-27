#include "FieldBoss.h"
#include "MeshMap.h"
#include "CollisionMgr.h"
#include "ModelMgr.h"
#include "Player.h"

namespace SSB
{
	DebugBox box;

	float FieldBoss::GetBattleRange()
	{
		return m_BattleRange;
	}
	float FieldBoss::GetSpotRange()
	{
		return m_SpotRange;
	}
	FieldBoss::FieldBoss()
	{
		for (int i = 0; i < 10; ++i)
		{
			Arrow* arrow = new Arrow();
			arrow->m_vRotation = TVector3(0, 0, 1.5708);

			_unActiveArrows.push(arrow);
		}
	}
	FieldBoss::~FieldBoss()
	{
		Release();

		while(!_unActiveArrows.empty())
		{
			auto arrow = _unActiveArrows.front();
			_unActiveArrows.pop();
			delete arrow;
		}
	}
	void FieldBoss::Attack()
	{
		if (!_unActiveArrows.empty())
		{
			auto arrow = _unActiveArrows.front();
			_unActiveArrows.pop();

			arrow->SetDevice(m_pd3dDevice, m_pImmediateContext);
			arrow->SetMatrix(nullptr, &m_matView, &m_matProj);
			arrow->Active(m_pMap, g_fGameTimer, GetCurSocketPos("BowBegin"), Player::GetInstance().m_vPos);
			_activeArrows.insert(arrow);
		}
	}
	void FieldBoss::SetMatrix(TMatrix* matWorld, TMatrix* matView, TMatrix* matProj)
	{
		Character::SetMatrix(matWorld, matView, matProj);
		for (auto arrow : _activeArrows)
		{
			arrow->SetMatrix(matWorld, matView, matProj);
		}
	}
	bool FieldBoss::Frame()
	{
		Character::Frame();

		std::set<Arrow*> eraseSet;
		for (auto arrow : _activeArrows)
		{
			if (arrow->IsDead())
			{
				eraseSet.insert(arrow);
			}
			else
			{
				arrow->Frame();
			}
		}

		for (auto arrow : eraseSet)
		{
			_activeArrows.erase(arrow);
			arrow->UnActive();
			_unActiveArrows.push(arrow);
		}

		return true;
	}
	bool FieldBoss::Render()
	{
		Character::Render();
		for (auto arrow : _activeArrows)
		{
			arrow->Render();
		}
		return true;
	}
	bool FieldBoss::Release()
	{
		for (auto arrow : _activeArrows)
		{
			arrow->UnActive();
			_unActiveArrows.push(arrow);
		}
		_activeArrows.clear();

		return true;
	}
	FieldBoss::Arrow::Arrow()
	{
		m_fSpeed = 30;
		I_Model.Load("Arrow", "", &m_pModel);
	}
	FieldBoss::Arrow::~Arrow()
	{
		Character::Release();
	}
	void FieldBoss::Arrow::Move()
	{
		m_vPos = m_vPos + m_vDirection * g_fSecondPerFrame * m_fSpeed;

		if (!_isHit)
		{
			m_matWorld.Translation(m_vPos);
		}

		{
			auto bv = m_pModel->GetBoundingVolume();
			TMatrix local = TMatrix::Identity;
			local._41 = bv.Position.x;
			local._42 = bv.Position.y;
			local._43 = bv.Position.z;
			TMatrix world = local * m_matWorld;
			m_ColliderBox.UpdateBox(world);
		}
	}
	void FieldBoss::Arrow::CollisionCheck()
	{
		if (CollisionMgr::GetInstance().IsCollide(&m_ColliderBox))
		{
			auto collideCharacterBoxList = CollisionMgr::GetInstance().GetHitCharacterList(&m_ColliderBox);
			for (auto box : collideCharacterBoxList)
			{
				if(box == &Player::GetInstance())
				{
					if (!Player::GetInstance().IsDash())
					{
						if (!_isHit)
						{
							Player::GetInstance().Damage(_damage);
							Player::GetInstance().m_pInterGageHP->m_pWorkList.push_back(new InterfaceSetGage((float)Player::GetInstance().m_HealthPoint / Player::GetInstance().m_kHealthPointMax, 1.0f));
							Player::GetInstance().m_pInterDamageBlood->m_pWorkList.push_back(new InterfaceFadeOut(1.0f));
							_isDead = true;
						}
					}
					break;
				}
			}

			if (!IsDead())
			{
				auto collideMapObjectBoxList = CollisionMgr::GetInstance().GetCollideBoxList(&m_ColliderBox, true);
				if (!collideMapObjectBoxList.empty())
				{
					_isHit = true;
				}
			}
		}

		FXMMATRIX world = XMLoadFloat4x4(&m_matWorld);
		XMVECTOR charPosition = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		charPosition = XMVector3TransformCoord(charPosition, world);
		float x = XMVectorGetX(charPosition);
		float z = XMVectorGetZ(charPosition);

		if (XMVectorGetY(charPosition) <= m_pMap->GetHeight(x, z))
			_isHit = true;
	}
	void FieldBoss::Arrow::Active(MeshMap* map, float timeStamp, TVector3 pos, TVector3 target)
	{
		_isDead = false;
		_isHit = false;

		SetMap(map);

		_startTimeStamp = timeStamp;

		m_vPos = pos;

		m_vDirection = target - pos;;
		m_vDirection.y = 0;
		m_vDirection.Normalize();

		auto lookAtMatrix = XMMatrixLookToLH(m_vPos, m_vDirection, TVector3(0, 1, 0));

		TMatrix rotXMatrix;
		D3DXMatrixRotationX(&rotXMatrix, -1.5708);

		TMatrix rotYMatrix;
		TVector3 forward(0, 0, 1);

		auto crossVec = XMVector3Cross(forward, m_vDirection);
		if (XMVectorGetY(crossVec) > 0)
		{
			D3DXMatrixRotationY(&rotYMatrix, acos(D3DXVec3Dot(&forward, &m_vDirection)));
		}
		else
		{
			D3DXMatrixRotationY(&rotYMatrix, -acos(D3DXVec3Dot(&forward, &m_vDirection)));
		}

		float scale = 0.1f;
		TMatrix scaleMatrix(
			scale, 0, 0, 0, 
			0, scale, 0, 0,
			0, 0, scale, 0,
			0, 0, 0, 1);

		m_matWorld = TMatrix();
		m_matWorld = m_matWorld * rotXMatrix * rotYMatrix * scaleMatrix;
		m_matWorld.Translation(m_vPos);

		auto boundVolume = m_pModel->GetBoundingVolume();
		m_ColliderBox.CreateOBBBox(boundVolume.Width, boundVolume.Height, boundVolume.Depth);
	}
	void FieldBoss::Arrow::UnActive()
	{
	}
	bool FieldBoss::Arrow::IsDead()
	{
		if (_kLiveTime < g_fGameTimer - _startTimeStamp)
			_isDead = true;

		return _isDead;
	}
	bool FieldBoss::Arrow::Init()
	{
		return true;
	}
	bool FieldBoss::Arrow::Render()
	{
		Character::Render();

		// È®ÀÎ¾ÈµÊ
		//static bool tmp = false;
		//if (!tmp)
		//{
		//	box.Create(m_pd3dDevice, m_pImmediateContext);
		//	tmp = true;
		//}
		//box.SetMatrix(&m_matView, &m_matProj);
		//box.SetBox(m_ColliderBox);
		//box.SetColor(TColor(0, 0, 1, 0));
		//box.UpdateBuffer();
		//box.Render();

		return true;
	}
	bool FieldBoss::Arrow::Frame()
	{
		Move();
		CollisionCheck();

		if (!IsDead())
		{
			m_pModel->Frame();
			UpdateBuffer();
		}
		return true;
	}
	bool FieldBoss::Arrow::Release()
	{
		return true;
	}
}
