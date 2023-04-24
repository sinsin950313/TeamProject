#pragma once
#include "Camera.h"
class CameraTPS :
    public Camera
{
public:
    virtual bool Frame();
    void CalcYawPitchRoll(float& fYaw, float& fPitch, float& fRoll);
public:
	float m_CharCamDist = 10.0f;
    TVector3* m_vFollowPos = nullptr;
};

