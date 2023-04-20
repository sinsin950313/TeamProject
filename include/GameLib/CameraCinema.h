#pragma once
#include "Camera.h"
class CameraCinema : public Camera
{
public:
	void MoveCameraBezierSpline(float time, float duration, XMFLOAT3 p0, XMFLOAT3 p1, XMFLOAT3 p2, XMFLOAT3 p3, XMFLOAT3 d0, XMFLOAT3 d1, XMFLOAT3 d2, XMFLOAT3 d3, XMFLOAT3& getPos, XMFLOAT3& getDir);
public:
	virtual bool Frame();
public:
	CameraCinema();
	virtual ~CameraCinema();
};
