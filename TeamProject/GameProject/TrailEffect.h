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

public:	
	int		m_iPos;

	static const int m_iTrailNum = 3;
	int		m_iCurTrail = 0;
	bool	m_isSetTrail[m_iTrailNum] = { false, };
	std::vector<Vertex> m_AttackTrail[m_iTrailNum];

	std::vector<TVector3>	m_vHighControlPosList[m_iTrailNum];
	std::vector<TVector3>	m_vHighCatmullRomList[m_iTrailNum];

	std::vector<TVector3>	m_vLowControlPosList[m_iTrailNum];
	std::vector<TVector3>	m_vLowCatmullRomList[m_iTrailNum];

	std::vector<Vertex>		m_VertexCatmullRomList[m_iTrailNum];
	int									m_iCatmullRomSize[m_iTrailNum];
};

static void	InterpolateCatmullRom(const std::vector<TVector3>& controlPos, std::vector<TVector3>* CatmullRomList, size_t controlSize, int numSamples);
