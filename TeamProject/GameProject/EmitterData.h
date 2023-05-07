#pragma once
#include "TMath.h"
#include <atlstr.h>

struct BasicData
{
	bool	isVisible;
	CString Name;
	UINT iSpawnCount;
	bool	isInfinite;
	UINT iInheritPosType;
	UINT iInheritRotType;
	UINT iInheritScaleType;
	bool isLifeTime;
	float fLifeTime;
	float fLifeTimeDiff;
	float fSpawnRate;
	float fSpawnRateDiff;
	float fInitDelay;
	float fInitDelayDiff;
};

struct TransformData
{
	//Fix Data;PVA;Easing;
	UINT	iType;
	TVector3 vFix;

	TVector3 vPVA;
	TVector3 vPVADiff;
	TVector3 vPVAVelo;
	TVector3 vPVAVeloDiff;
	TVector3 vPVAAcc;
	TVector3 vPVAAccDiff;

	TVector3 vEasingStart;
	TVector3 vEasingStartDiff;
	TVector3 vEasingEnd;
	TVector3 vEasingEndDiff;
};

struct BasicRenderData
{
	UINT iMaterial;
	std::wstring texPath;
	UINT iBlendType;
	bool	isEmissive;
};

struct RenderSetData
{
	UINT iRenderType;
	UINT iColorType;

	TColor vFixColor;

	TColor vRandMinColor;
	TColor vRandMaxColor;

	TColor vStartColor;
	TColor vStartDiffColor;
	TColor vEndColor;
	TColor vEndDiffColor;

	UINT iBillboard;
};