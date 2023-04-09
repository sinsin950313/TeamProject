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
	void	StartTrail(TMatrix* matWorld);

public:	
	const int		m_iCPSize = 100;
	const int		m_iSampleNum = 30;

	int		m_iPos;

	std::vector<TVector3>	m_vHighControlPosList;
	std::vector<TVector3>	m_vHighCatmullRomList;

	std::vector<TVector3>	m_vLowControlPosList;
	std::vector<TVector3>	m_vLowCatmullRomList;

	std::vector<Vertex>		m_VertexCatmullRomList;
	int									m_iCatmullRomSize;
};

static void	InterpolateCatmullRom(const std::vector<TVector3>& controlPos, std::vector<TVector3>* CatmullRomList, size_t controlSize, int numSamples);
