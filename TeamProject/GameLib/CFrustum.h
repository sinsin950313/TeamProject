#pragma once
#include "TMath.h"

enum PLANE_COLTYPE
{
	P_BACK = 0,
	P_FRONT,
	P_ONPLANE,
	P_SPANNING,
};

class CFrustum
{
	TMatrix m_matView;
	TMatrix m_matProj;

public:
	TVector3		m_vFrustum[8];
	TPlane		m_Plane[6];
public:
	void	CreateFrustum(TMatrix* view, TMatrix* proj);
	//PLANE_COLTYPE	ClassifyPoint(TVector3 v);
	//PLANE_COLTYPE	ClassifySphere(CSphere sphere);
	//PLANE_COLTYPE	ClassifyAABB(C_AABB v); //OBB를 구현하면 그게 그거임
	//PLANE_COLTYPE	ClassifyOBB(C_OBB obb);
	//PLANE_COLTYPE	ClassifyBOX(C_BOX box);
};

