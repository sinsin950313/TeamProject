#pragma once
#include "BaseObject.h"
class TrailEffect : public BaseObject
{
public:
	virtual bool	Create(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext, std::wstring shaderPath, std::wstring texPath,
		std::wstring VSname = L"VS", std::wstring PSname = L"PS");

	virtual bool Frame();
	virtual bool PreRender();
	virtual bool Render();
	virtual bool PostRender();

	void	AddTrailPos(TVector3 low, TVector3 high);
};

