#pragma once
#include "TMath.h"
#include <vector>

struct T_PLANE
{
	float	fA, fB, fC, fD;
	bool	CreatePlane(TVector3 v0, TVector3 v1, TVector3 v2);
	bool	CreatePlane(TVector3 vNormal, TVector3 v0);
	void	Normalize()
	{
		float fMag = sqrt(fA * fA + fB * fB + fC * fC);
		fA = fA / fMag;
		fB = fB / fMag;
		fC = fC / fMag;
		fD = fD / fMag;
	}

};
struct T_SPHERE
{
	TVector3		vCenter;
	float			fRadius;
};
struct T_BOX
{
	void	CreateOBBBox(float fExtX = 1.0f, float fExtY = 1.0f, float fExtZ = 1.0f,
		TVector3 vCenter = TVector3(0.0f, 0.0f, 0.0f),
		TVector3 vDirX = TVector3(1.0f, 0.0f, 0.0f),
		TVector3 vDirY = TVector3(0.0f, 1.0f, 0.0f),
		TVector3 vDirZ = TVector3(0.0f, 0.0f, 1.0f));
	void	CreateAABBBox(TVector3 vMax = TVector3(1.0f, 1.0f, 1.0f),
		TVector3 vMin = TVector3(-1.0f, -1.0f, -1.0f));
	void	UpdateBox(TMatrix& world);

	// Common
	TVector3		vCenter;
	TVector3		vPos[8];
	// AABB
	TVector3		vMax;
	TVector3		vMin;
	// OBB
	TVector3		vAxis[3];
	float			fExtent[3];
	void Set(XMFLOAT3 max, XMFLOAT3	min)
	{
		vMax = max;
		vMin = min;
		vCenter = (vMax + vMin) * 0.5f;
		vAxis[0] = { 1,0,0 };
		vAxis[1] = { 0,1,0 };
		vAxis[2] = { 0,0,1 };
		fExtent[0] = vMax.x - vCenter.x;
		fExtent[1] = vMax.y - vCenter.y;
		fExtent[2] = vMax.z - vCenter.z;

		vPos[0] = XMFLOAT3(vMin.x, vMin.y, vMin.z);
		vPos[1] = XMFLOAT3(vMin.x, vMin.y, vMax.z);
		vPos[2] = XMFLOAT3(vMax.x, vMin.y, vMax.z);
		vPos[3] = XMFLOAT3(vMax.x, vMin.y, vMin.z);
		vPos[4] = XMFLOAT3(vMin.x, vMax.y, vMin.z);
		vPos[5] = XMFLOAT3(vMin.x, vMax.y, vMax.z);
		vPos[6] = XMFLOAT3(vMax.x, vMax.y, vMax.z);
		vPos[7] = XMFLOAT3(vMax.x, vMax.y, vMin.z);
	}
};

struct T_RAY
{
	float			fExtent;
	TVector3		vOrigin;
	TVector3		vDirection;
	T_RAY()
	{
		//  fExtent > 0 : 세그먼트로 사용 
		fExtent = -1; 
	}
};

class TSelect
{
public:
	T_RAY		m_Ray;
	TVector3	m_vIntersection;
	TMatrix		m_matWorld;
	TMatrix		m_matView;
	TMatrix		m_matProj;
	TVector3	m_vDxR;
	TVector3	m_vCenter;
	TVector3	m_vSrcVex[3];
	float		m_fPickDistance;
	TMatrix		m_matWorldPick;
	TVector3	qvec;
	TVector3	tvec;
	TVector3	pvec;
public:
	void SetMatrix(TMatrix* pWorld, TMatrix* pView, TMatrix* pProj);
	void Update();
	bool IntersectRayToSphere(T_SPHERE* pSphere, T_RAY* pRay = NULL);
	bool ChkOBBToRay(T_BOX* pBox, T_RAY* pRay = NULL);
	bool IntersectBox(T_BOX* pBox, T_RAY* pRay = NULL);
	bool AABBtoRay(T_BOX* a, T_RAY* pRay = NULL);
	bool OBBtoRay(T_BOX* a, T_RAY* pRay = NULL);
	TVector3 GetIntersection();
	// 교점을 찾는다.
	bool    GetIntersection(TVector3 vStart,TVector3 vEnd,TVector3 vNormal,TVector3 v0,TVector3 v1,	TVector3 v2,float* fPercentage = nullptr);
	// 폴리곤안에 위치하는지 판단한다.
	bool    PointInPolygon(TVector3 vert,TVector3 faceNormal,TVector3 v0,TVector3 v1,TVector3 v2);
	bool	ChkPick(TVector3& v0, TVector3& v1, TVector3& v2);
	bool	IntersectTriangle(const TVector3& orig, const TVector3& dir,TVector3& v0, TVector3& v1, TVector3& v2,FLOAT* t, FLOAT* u, FLOAT* v);
public:
	TSelect(void);
	virtual ~TSelect(void);
};

