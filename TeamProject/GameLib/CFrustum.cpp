#include "CFrustum.h"

void	CFrustum::CreateFrustum(TMatrix* view, TMatrix* proj)
{
	TMatrix matView = *view;
	TMatrix matProj = *proj;

	TMatrix matInvViewProj = matView * matProj;
	//CYJMATH::Invert(&matInvViewProj, &matInvViewProj);
	D3DXMatrixInverse(&matInvViewProj, nullptr, &matInvViewProj);

	m_vFrustum[0] = TVector3(-1.0f, -1.0f, 0.0f);
	m_vFrustum[1] = TVector3(-1.0f, 1.0f, 0.0f);
	m_vFrustum[2] = TVector3(1.0f, 1.0f, 0.0f);
	m_vFrustum[3] = TVector3(1.0f, -1.0f, 0.0f);
	m_vFrustum[4] = TVector3(-1.0f, -1.0f, 1.0f);
	m_vFrustum[5] = TVector3(-1.0f, 1.0f, 1.0f);
	m_vFrustum[6] = TVector3(1.0f, 1.0f, 1.0f);
	m_vFrustum[7] = TVector3(1.0f, -1.0f, 1.0f);

	for (int iVer = 0; iVer < 8; iVer++)
	{
		D3DXVec3TransformCoord(&m_vFrustum[iVer], &m_vFrustum[iVer], &matInvViewProj);
	}
	// 5	6
	// 4	7
	//
	// 1	2
	// 0	3

	// LEFT
	m_Plane[0] = TPlane(m_vFrustum[5], m_vFrustum[0], m_vFrustum[1]);
	// RIGHT
	m_Plane[1] = TPlane(m_vFrustum[6], m_vFrustum[2], m_vFrustum[7]);
	// TOP
	m_Plane[2] = TPlane(m_vFrustum[1], m_vFrustum[2], m_vFrustum[5]);
	// BOTTOM
	m_Plane[3] = TPlane(m_vFrustum[0], m_vFrustum[4], m_vFrustum[7]);
	// NEAR
	m_Plane[4] = TPlane(m_vFrustum[2], m_vFrustum[1], m_vFrustum[0]);
	// FAR
	m_Plane[5] = TPlane(m_vFrustum[4], m_vFrustum[5], m_vFrustum[6]);
}
/*
PLANE_COLTYPE	CFrustum::ClassifyPoint(TVector3 v)
{
	for (int iPlane = 0; iPlane < 6; iPlane++)
	{
		float fDistance =
			m_Plane[iPlane].x * v.x +
			m_Plane[iPlane].y * v.y +
			m_Plane[iPlane].z * v.z +
			m_Plane[iPlane].w;
		if (fDistance < 0) 
			return P_BACK;
		//if (fDistance == 0)
		//	return P_ONPLANE;
	}
	return P_FRONT;
}
PLANE_COLTYPE	CFrustum::ClassifySphere(CSphere sphere)
{
	float fPlaneToCenter;
	PLANE_COLTYPE dwRet = P_FRONT;
	for (int iPlane = 0; iPlane < 6; iPlane++)
	{
		fPlaneToCenter =
			m_Plane[iPlane].a * sphere.m_vCenter.x +
			m_Plane[iPlane].b * sphere.m_vCenter.y +
			m_Plane[iPlane].c * sphere.m_vCenter.z +
			m_Plane[iPlane].d;

		if (fPlaneToCenter > 0)
		{
			if (fPlaneToCenter < sphere.m_fRadius)
			{
				dwRet = P_SPANNING;
				break;
			}
		}
		else if (fPlaneToCenter < 0)
		{
			dwRet = P_BACK;
			if (-fPlaneToCenter < sphere.m_fRadius)
			{
				dwRet = P_SPANNING;
			}
			break;
		}

		//if (fPlaneToCenter < -sphere.m_fRadius)
		//	return false;
	}
	return dwRet;
}
//bool	CFrustum::ClassifyAABB(C_AABB v)
//{
//	return true;
//}
PLANE_COLTYPE	CFrustum::ClassifyOBB(C_OBB obb)
{
	float	fPlaneToCenter = 0.0f;
	float	fDistance = 0.0f;
	TVector3 vDir;
	PLANE_COLTYPE dwRet = P_FRONT;
	for (int iPlane = 0; iPlane < 6; iPlane++)
	{
		vDir = obb.vAxis[0] * obb.fDistance[0];
		fDistance = fabs(m_Plane[iPlane].a * vDir.x) + fabs(m_Plane[iPlane].b * vDir.y) + fabs(m_Plane[iPlane].c * vDir.z);
		vDir = obb.vAxis[1] * obb.fDistance[1];
		fDistance += fabs(m_Plane[iPlane].a * vDir.x) + fabs(m_Plane[iPlane].b * vDir.y) + fabs(m_Plane[iPlane].c * vDir.z);
		vDir = obb.vAxis[2] * obb.fDistance[2];
		fDistance += fabs(m_Plane[iPlane].a * vDir.x) + fabs(m_Plane[iPlane].b * vDir.y) + fabs(m_Plane[iPlane].c * vDir.z);
		
		fPlaneToCenter = m_Plane[iPlane].a * obb.vCenter.x +
			m_Plane[iPlane].b * obb.vCenter.y +
			m_Plane[iPlane].c * obb.vCenter.z +
			m_Plane[iPlane].d;

		// obb의 중앙과의 거리가 평면의 방정식에 의해서 -가 나오면서 
		// 가장 가까운 점과 평면과의 거리값에 -를 해서 이미 들어온 점은 무조건 +가 나오기 때문에
		// 들어온 값은 무조건 통과시키면서 밖에 있는 점과의 거리 비교만이 가능하게 함
		//if (fPlaneToCenter < -fDistance) 
		//{
		//	return false;
		//}
		if (fPlaneToCenter <= fDistance)
		{
			dwRet = P_SPANNING;
		}

		if (fPlaneToCenter + 1.5f < -fDistance)
		{
			return P_BACK;
		}
		//if (fPlaneToCenter >= 0)
		//{
		//	if (fPlaneToCenter < fDistance)
		//	{
		//		dwRet = P_SPANNING;
		//		break;
		//	}
		//}
		//else if (fPlaneToCenter < 0)
		//{
		//	dwRet = P_BACK;
		//	if (-fPlaneToCenter < fDistance)
		//	{
		//		dwRet = P_SPANNING;
		//	}
		//	break;
		//}
	}
	return dwRet;
}
PLANE_COLTYPE	CFrustum::ClassifyBOX(C_BOX box)
{
	C_OBB obb;
	memcpy(obb.vAxis, box.vAxis, sizeof(TVector3) * 3);
	memcpy(obb.fDistance, box.fExtent, sizeof(float) * 3);
	obb.vCenter = box.vCenter;

	return ClassifyOBB(obb);
}
*/