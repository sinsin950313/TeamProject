#pragma once
#include "Camera.h"
class CameraTPS :
    public Camera
{
public:
    virtual bool Frame();

public:
    TVector3* m_vFollowPos = nullptr;
};

