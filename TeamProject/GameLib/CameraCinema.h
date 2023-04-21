#pragma once
#include "Camera.h"

class CameraCinema : public Camera
{
public:
	void MoveCameraBezierSpline(float time, float duration, std::vector<CameraMove>& camMoveList, XMFLOAT3& getPos, XMFLOAT3& getDir);
	float BinomialCoefficient(int n, int k);
public:
	virtual bool Frame();
public:
	CameraCinema();
	virtual ~CameraCinema();
};
