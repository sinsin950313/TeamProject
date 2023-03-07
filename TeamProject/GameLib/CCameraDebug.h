#pragma once
#include "CCamera.h"
class CCameraDebug :
    public CCamera
{
public:
	//virtual void CreateViewMatrix(TVector3 vEye, TVector3 vAt, TVector3 vUp);
	//virtual void CreateProjMatrix(float fNear, float fFar, float fFovY, float fAspectRatio);
	virtual bool Frame();
};

