#pragma once
#include "Map.h"
#include "Character.h"
class Object : public Character
{
protected:
	std::wstring mtw(std::string str);
	std::string wtm(std::wstring str);
	std::vector<std::wstring> SplitPath(std::wstring fullPath);

public:
	virtual bool Frame() override;
	virtual bool Render() override;
public:
	Object();
	Object(std::string szFullPath, Transform transform, XMFLOAT3 vAxis0, XMFLOAT3 vAxis1, XMFLOAT3 vAxis2, float fExtentX, float fExtentY, float fExtentZ, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~Object();
};

