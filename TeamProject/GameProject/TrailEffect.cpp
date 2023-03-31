#include "TrailEffect.h"
#include "DXState.h"

bool	TrailEffect::Create(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext, std::wstring shaderPath, std::wstring texPath,
	std::wstring VSname, std::wstring PSname)
{
	//m_VertexList.resize(100);

	for (int i = 0; i < 100; i++)
	{
		Vertex v;
		float x = i;
		v.p = { x, 2, 0 };
		v.c = { 1, 1, 1, 0.0 };
		v.t = { 1, 1 };
		v.n = { 0, 0, 0 };

		m_VertexList.push_back(v);

		v.p = { x, 0, 0 };
		m_VertexList.push_back(v);
	}
	m_VertexList.push_back(Vertex());

	//m_IndexList.resize((100 - 2) * 3);
	long dwTriCnt = m_VertexList.size() - 3;
	int p = m_VertexList.size() - 1;
	for (UINT iTri = 0; iTri < dwTriCnt; iTri += 2)
	{
		m_IndexList.push_back(iTri + 3);
		m_IndexList.push_back(iTri + 1);
		m_IndexList.push_back(iTri + 0);

		m_IndexList.push_back(iTri + 2);
		m_IndexList.push_back(iTri + 3);
		m_IndexList.push_back(iTri + 0);
	}

	BaseObject::Create(pd3dDevice, pContext, shaderPath, texPath, VSname, PSname);

	return true;
}

bool TrailEffect::Frame()
{
	//
	// high 에서 low를 빼고 normal을 하면 right 
	// 두 위치의 평균이 중심 
	//  두 위치의 거리는 매번 계산 안하는게 좋을듯
	// counter로 매번 값을 변경해야겠네 
	// m_iPos는 그대로 사용하는데 m_iPos를 기반으로 사용중인 VertexList의 사이즈를 얻고 
	// 그걸 기반으로 Index 숫자를 구할 수 있음
	// 
	// 콜리전 충돌
	// x-axis에 현재 캐릭터의 방향을 내적해서 + / - / 0에 따라서 
	// +-인 경우 
	// 
	// 그냥 T_BOX를 가지고 Plane 6개를 만드는 방식을 고려해봐야 할지도
	// 
	//

	for (int i = 0; i < m_VertexList.size(); i++)
	{
		m_VertexList[i].c.w -= g_fSecondPerFrame * 2;
		if (m_VertexList[i].c.w < 0.0f)
		{

		}
	}

	return BaseObject::Frame();
}

bool TrailEffect::PreRender()
{
	return BaseObject::PreRender();
}

bool TrailEffect::Render()
{
	PreRender();
	PostRender();

	return true;
}

bool TrailEffect::PostRender()
{
	m_pImmediateContext->RSSetState(DXState::g_pNonCullRSSolid);
	if (m_pIndexBuffer == nullptr)
		m_pImmediateContext->Draw(m_VertexList.size(), 0);
	else
	{
		int iTriCnt = (m_iPos - 4);
		if (iTriCnt > 1)
			m_pImmediateContext->DrawIndexed(iTriCnt * 3, 0, 0);
	}
	m_pImmediateContext->RSSetState(DXState::g_pDefaultRSSolid);
	return true;
}

void	TrailEffect::AddTrailPos(TVector3 low, TVector3 high)
{
	Vertex v;
	v.p = low;
	v.n = TVector3(0, 0, 0);
	v.c = TVector4(1, 1, 1, 1);
	v.t = TVector2(0, 0);

	if (m_iPos == m_VertexList.size() - 1)
		return;

	m_VertexList[m_iPos++] = v;
	//m_VertexList.push_back(v);

	v.p = high;
	m_VertexList[m_iPos++] = v;
	
	/*
	int iCatmullRomCount = m_iPos / 2;
	int iEndIndex = iCatmullRomCount * 2 + (m_VertexList.size() - 1);

	iCatmullRomIndex[2] = iEndIndex - 2;
	iCatmullRomIndex[3] = iEndIndex;
	*/
}
