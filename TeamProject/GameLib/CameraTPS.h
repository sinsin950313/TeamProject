#pragma once
#include "Camera.h"
class CameraTPS :
    public Camera
{
public:
    virtual bool Frame();
public:
	float m_CharCamDist = 10.0f;
    TVector3* m_vFollowPos = nullptr;
};

