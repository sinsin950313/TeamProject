#pragma once
#include "TMath.h"
#include "TCollision.h"
enum PLANE_COLTYPE
{
	P_BACK = 0,
	P_FRONT,
	P_ONPLANE,
	P_SPANNING,
};

struct Plane
{
	float a, b, c, d;
	void Create(XMVECTOR v0, XMVECTOR v1, XMVECTOR v2)
	{
		XMVECTOR vE0 = v1 - v0;
		XMVECTOR vE1 = v2 - v0;
		XMVECTOR vNormal = XMVector3Cross(vE0, vE1);
		vNormal = XMVector3Normalize(vNormal);
		a = XMVectorGetX(vNormal);
		b = XMVectorGetY(vNormal);
		c = XMVectorGetZ(vNormal);
		d = -XMVectorGetX(XMVector3Dot(vNormal, v0)); //거리
	}
	void  Create(XMVECTOR normal, XMVECTOR  v0)
	{
		normal = XMVector3Normalize(normal);
		a = XMVectorGetX(normal);
		b = XMVectorGetY(normal);
		c = XMVectorGetZ(normal);
		d = -XMVectorGetX(XMVector3Dot(normal, v0)); //거리
	}
};

class Frustum
{
	TMatrix m_matView;
	TMatrix m_matProj;

public:
	TVector3		m_vFrustum[8];
	Plane		m_Plane[6];
public:
	void	CreateFrustum(TMatrix* view, TMatrix* proj);
	//PLANE_COLTYPE	ClassifyPoint(TVector3 v);
	//PLANE_COLTYPE	ClassifySphere(CSphere sphere);
	//PLANE_COLTYPE	ClassifyAABB(C_AABB v); //OBB를 구현하면 그게 그거임
	//PLANE_COLTYPE	ClassifyOBB(C_OBB obb);
	//PLANE_COLTYPE	ClassifyBOX(C_BOX box);
	PLANE_COLTYPE	ClassifyOBB(T_BOX v);

};

