#include "TrailEffect.h"
#include "DXState.h"

bool	TrailEffect::Create(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext, std::wstring shaderPath, std::wstring texPath,
	std::wstring VSname, std::wstring PSname)
{
	m_VertexList.resize(100);
	m_IndexList.resize((100 - 2) * 3);
	BaseObject::Create(pd3dDevice, pContext, shaderPath, texPath, VSname, PSname);

	return true;
}

bool TrailEffect::Frame()
{
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
	BaseObject::PostRender();
	m_pImmediateContext->RSSetState(DXState::g_pDefaultRSSolid);
	return true;
}

void	TrailEffect::AddTrailPos(TVector3 low, TVector3 high)
{
	static int vPos = 0;

	Vertex v;
	v.p = low;
	v.n = TVector3(0, 0, 0);
	v.c = TVector4(1, 1, 1, 1);
	v.t = TVector2(1, 1);

	m_VertexList[vPos++] = v;
	//m_VertexList.push_back(v);

	v.p = high;
	m_VertexList[vPos++] = v;
	if (vPos == 100)
		vPos = 0;
	//m_VertexList.push_back(v);

	long dwTriCnt = m_VertexList.size() - 2;

	m_VertexList[0].p = TVector3(1, 1, 0);
	m_VertexList[1].p = TVector3(1, 0, 0);
	m_VertexList[3].p = TVector3(0, 0, 0);

	m_VertexList[2].p = TVector3(0, 1, 0);
	m_VertexList[3].p = TVector3(0, 0, 0);
	m_VertexList[0].p = TVector3(1, 1, 0);

	int i = 0;
	m_IndexList[i++] = 3;
	m_IndexList[i++] = 1;
	m_IndexList[i++] = 0;

	m_IndexList[i++] = 2;
	m_IndexList[i++] = 3;
	m_IndexList[i++] = 0;

	m_IndexList[i++] = 5;
	m_IndexList[i++] = 3;
	m_IndexList[i++] = 2;

	m_IndexList[i++] = 4;
	m_IndexList[i++] = 5;
	m_IndexList[i++] = 2;


	//0 - tri = 3, 1, 0;
	//1 - tri = 2, 3, 0;
	//
	//2 - tri = 5, 3, 2;
	//3 - tri = 4, 5, 2;

	for (UINT iTri = 0; iTri < dwTriCnt; iTri += 2)
	{
		DWORD tri[3];

		

		//m_IndexList.push_back(i + 3);
		//m_IndexList.push_back(i + 1);
		//m_IndexList.push_back(i + 0);
		//m_IndexList[iTri + 0] = i + 3;
		//m_IndexList[iTri + 1] = i + 1;
		//m_IndexList[iTri + 2] = i + 0;

		//m_IndexList.push_back(i + 2);
		//m_IndexList.push_back(i + 3);
		//m_IndexList.push_back(i + 0);
		//m_IndexList[i++] = i + 2;
		//m_IndexList[i++] = i + 3;
		//m_IndexList[i++] = i + 0;

		if (i > 80)
		{
			int a = 4;
		}
	}
}
