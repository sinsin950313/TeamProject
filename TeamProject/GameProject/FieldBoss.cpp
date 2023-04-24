#include "FieldBoss.h"
#include "MeshMap.h"
#include "CollisionMgr.h"
#include "ModelMgr.h"
#include "Player.h"

namespace SSB
{
	float FieldBoss::GetBattleRange()
	{
		return m_BattleRange;
	}
	float FieldBoss::GetSpotRange()
	{
		return m_SpotRange;
	}
	FieldBoss::~FieldBoss()
	{
		Release();
	}
	void FieldBoss::Attack()
	{
		_arrows.insert(new Arrow(m_pMap, g_fGameTimer));
	}
	void FieldBoss::SetMatrix(TMatrix* matWorld, TMatrix* matView, TMatrix* matProj)
	{
		Character::SetMatrix(matWorld, matView, matProj);
		for (auto arrow : _arrows)
		{
			arrow->SetMatrix(matWorld, matView, matProj);
		}
	}
	bool FieldBoss::Frame()
	{
		std::set<Arrow*> eraseSet;
		for (auto arrow : _arrows)
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

		for (auto erase : eraseSet)
		{
			erase->Release();
			_arrows.erase(erase);
			delete erase;
		}
		return true;
	}
	bool FieldBoss::Release()
	{
		for (auto arrow : _arrows)
		{
			arrow->Release();
			delete arrow;
		}
		_arrows.clear();
		return true;
	}
	FieldBoss::Arrow::Arrow(MeshMap* map, float timeStamp) : _startTimeStamp(timeStamp)
	{
		SetMap(map);
		m_fSpeed = 30;
		I_Model.Load("Arrow", "", &m_pModel);
	}
	void FieldBoss::Arrow::Move()
	{
		XMVECTOR position = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMMATRIX world = XMLoadFloat4x4(&m_matWorld);
		position = XMVector3TransformCoord(position, world);

		float frameTime = g_fSecondPerFrame;
		float local = m_fSpeed * frameTime;
		float timeFactor = 1.0f;	// You can speed up or slow down time by changing this

		TVector3 direction = m_matWorld.Forward();
		direction.y = -9.8f;
		direction = direction * local;
		direction.Normalize();

		XMVECTOR destinationVector = direction * local;

		position = position + destinationVector;

		{
			XMStoreFloat3(&m_vPos, position);
			UpdateMatrix();

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
					Player::GetInstance().Damage(_damage);
					_isDead = true;
					break;
				}
			}

			if (!IsDead())
			{
				auto collideMapObjectBoxList = CollisionMgr::GetInstance().GetCollideBoxList(&m_ColliderBox, true);
				if (!collideMapObjectBoxList.empty())
				{
					_isDead = true;
				}
			}
		}

		FXMMATRIX world = XMLoadFloat4x4(&m_matWorld);
		XMVECTOR charPosition = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		charPosition = XMVector3TransformCoord(charPosition, world);
		float x = XMVectorGetX(charPosition);
		float z = XMVectorGetZ(charPosition);

		if (m_pMap->GetHeight(x, z) < XMVectorGetY(charPosition))
			_isDead = true;
	}
	bool FieldBoss::Arrow::IsDead()
	{
		return _isDead;
	}
	bool FieldBoss::Arrow::Frame()
	{
		Move();
		CollisionCheck();

		if (_kLiveTime < g_fGameTimer - _startTimeStamp)
			_isDead = true;

		if (!IsDead())
		{
			m_pModel->Frame();
			UpdateBuffer();
		}
		return true;
	}
}
