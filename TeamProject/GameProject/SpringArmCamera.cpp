#include "SpringArmCamera.h"
#include "CollisionMgr.h"

namespace SSB
{
	bool SpringArmCamera::CheckIntersectionWithMap()
	{
		XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(-m_fCameraPitchAngle, m_fCameraYawAngle, 0);
		XMVECTOR DefaultForward = { 0, 0, 1, 0 };
		XMVECTOR camPosition = XMVector3TransformNormal(DefaultForward, camRotationMatrix);
		camPosition = XMVector3Normalize(camPosition);

		m_Select.m_Ray.vOrigin = m_vTarget;
		m_Select.m_Ray.vOrigin.y += 1;
		XMStoreFloat3(&m_Select.m_Ray.vDirection, camPosition);
		m_Select.m_Ray.vDirection.Normalize();

		m_Select.m_Ray.fExtent = _kMaxDistance;

		for (auto node : _map->m_pDrawLeafNodeList)
		{
			if (m_Select.OBBtoRay(&node->m_Box))
			{
				UINT index = 0;
				UINT iNumFace = node->m_IndexList.size() / 3;
				for (UINT face = 0; face < iNumFace; face++)
				{
					UINT i0 = node->m_IndexList[index + 0];
					UINT i1 = node->m_IndexList[index + 1];
					UINT i2 = node->m_IndexList[index + 2];
					TVector3 v0 = _map->m_pMap->m_ListVertex[i0].pos;
					TVector3 v1 = _map->m_pMap->m_ListVertex[i1].pos;
					TVector3 v2 = _map->m_pMap->m_ListVertex[i2].pos;
					if (m_Select.ChkPick(v0, v1, v2, _kMaxDistance))
					{
						return true;
					}
					index += 3;
				}
			}
		}

		return false;
	}

	float SpringArmCamera::ShotRay()
	{
		float ret = _kMaxDistance;

		if (CheckIntersectionWithMap())
		{
			ret = min(_kMaxDistance, TVector3::Distance(m_Select.m_vIntersection, m_vTarget));
		}

		//TVector3 center;
		//TVector3 cameraPosition;
		//{
		//	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(-m_fCameraPitchAngle, m_fCameraYawAngle, 0);
		//	XMVECTOR DefaultForward = { 0, 0, 1, 0 };
		//	XMVECTOR camPosition = XMVector3TransformNormal(DefaultForward, camRotationMatrix);
		//	camPosition = XMVector3Normalize(camPosition);
		//	camPosition = (camPosition * _kMaxDistance) + m_vTarget;
		//	XMStoreFloat3(&cameraPosition, camPosition);

		//	center = (m_vTarget + cameraPosition) * 0.5f;
		//}
		//TVector3 delta = cameraPosition - m_vTarget;
		//TVector3 y(0, 1, 0);

		//TVector3 z = delta;
		//z.Normalize();

		//TVector3 x;
		//x = y.Cross(z);
		//x.Normalize();

		//float width = 0.001f;
		//float height = 0.001f;
		//float depth = sqrt((delta.x * delta.x) + (delta.y * delta.y) + (delta.z * delta.z)) * 0.5f;

		//_collideCheckBox.CreateOBBBox(width, height, depth, center, x, y, z);
		//{
		//	TMatrix tmpWorld;
		//	TQuaternion q;
		//	XMVECTOR quat = XMQuaternionRotationMatrix(XMMatrixRotationRollPitchYaw(-m_fCameraPitchAngle, m_fCameraYawAngle, 0));
		//	XMStoreFloat4(&q, quat);
		//	D3DXMatrixAffineTransformation(&tmpWorld, 1, nullptr, &q, &center);
		//	_collideCheckBox.UpdateBox(tmpWorld);
		//}

		//if (I_Collision.IsCollide(&_collideCheckBox))
		//{
		//	auto datas = I_Collision.GetCollideBoxList(&_collideCheckBox, true);
		//	for (auto data : datas)
		//	{
		//		auto collisionData = I_Collision.GetCollideData(_collideCheckBox, data);
		//		for (auto tmp : collisionData)
		//		{
		//			ret = min(ret, _kMaxDistance - tmp.CollisionDepth);
		//		}
		//	}
		//}

		return ret;
	}
	void SpringArmCamera::Initialize_SetMap(FQuadTree* map)
	{
		_map = map;
	}
	bool SpringArmCamera::Frame()
	{
		m_Select.SetMatrix(nullptr, &m_matView, &m_matProj);

		m_CharCamDist = max(0.1f, ShotRay());

		CameraTPS::Frame();

		return true;
	}
}
