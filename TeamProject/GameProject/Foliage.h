#pragma once
#include "Object.h"

class Foliage : public Object
{
public:
	bool Frame() override;
	virtual bool PreRender() override;
	bool Render() override;

public:
	Foliage(std::string szFullPath, std::vector<InstanceData>* pListInstanceData, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~Foliage();
public:
	std::vector<InstanceData> m_ListInstanceData;
	ID3D11Buffer* m_pInstanceBuffer = nullptr;
};

