#include "TrailEffect.h"
#include "DXState.h"

void	InterpolateCatmullRom(const std::vector<TVector3>& controlPos, std::vector<TVector3>* CatmullRomList, size_t controlSize, int numSamples)
{
	for (int i = 0; i < controlSize - 1; i++) {
		for (int j = 0; j < numSamples; j++) {
			float t = static_cast<float>(j) / numSamples;
			TVector3 p;
			D3DXVec3CatmullRom(&p,
				&controlPos[i > 0 ? i - 1 : i],
				&controlPos[i],
				&controlPos[i + 1],
				&controlPos[i < controlSize - 2 ? i + 2 : i + 1], t);
			CatmullRomList->push_back(p);
		}
	}
}

bool	TrailEffect::Create(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext, std::wstring shaderPath, std::wstring texPath,
	std::wstring VSname, std::wstring PSname)
{
	for (int i = 0; i < m_iCPSize; i++)
	{
		Vertex v;
		float x = i;
		v.p = { x, 2, 0 };
		v.c = { 1, 1, 1, 0.0 };
		v.t = { 1, 1 };
		v.n = { 0, 0, 0 };

		m_VertexList.push_back(v);

		v.p = { x, 0, 0 };
		v.t = { 0, 0 };
		m_VertexList.push_back(v);
	}

		m_vHighControlPosList.resize(m_iCPSize);
		m_vLowControlPosList.resize(m_iCPSize);
		m_VertexCatmullRomList.resize(m_iSampleNum * 100 * 2);
	

	m_VertexList.resize(m_iSampleNum * 100 * 2);

	long dwTriCnt = m_VertexList.size() - 2;
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
	// 콜리전 충돌
	// x-axis에 현재 캐릭터의 방향을 내적해서 + / - / 0에 따라서 
	// +-인 경우 
	// 
	// 그냥 T_BOX를 가지고 Plane 6개를 만드는 방식을 고려해봐야 할지도
	// 

	// 전체 사이즈를 반으로 나눠서 for문을 돌면 Low High의 별개 처리 가능
	// m_iTransPos의 값을 0으로 초기화 해줄 부분이 필요
	// 

	int iHalfSize = m_iCatmullRomSize / 2;
	for (int i = 0; i < iHalfSize; i++)
	{
		float delta = 1 - (i / (float)m_iCatmullRomSize);

		int low = i * 2 + 0;
		int high = i * 2 + 1;

		m_VertexCatmullRomList[low].t = TVector2(delta, 1);
		m_VertexCatmullRomList[high].t = TVector2(delta, 0);

		m_VertexCatmullRomList[low].c.w -= g_fSecondPerFrame * 4;
		m_VertexCatmullRomList[high].c.w -= g_fSecondPerFrame * 4;
	}
	m_VertexList = m_VertexCatmullRomList;

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
		//int iTriCnt = (m_iPos - 4);
		int iTriCnt = min((m_iPos * 2 * m_iSampleNum), m_iCatmullRomSize) - 4;
		if (iTriCnt > 1)
		{
			m_pImmediateContext->VSSetShader(m_pShader->m_pVS, NULL, 0);
			m_pImmediateContext->PSSetShader(m_pShader->m_pPS, NULL, 0);
			m_pImmediateContext->DrawIndexed(iTriCnt * 3, 0, 0);
		}
	}
	m_pImmediateContext->RSSetState(DXState::g_pDefaultRSSolid);
	return true;
}

#include "Player.h"
void	TrailEffect::AddTrailPos(TVector3 low, TVector3 high)
{
	if (m_iPos == m_iCPSize)
		return;

	TMatrix matInv = Player::GetInstance().m_matWorld.Invert();
	Vertex v;
	//v.p = low;
	D3DXVec3TransformCoord(&v.p, &low, &matInv);
	v.n = TVector3(0, 0, 0);
	v.c = TVector4(1, 1, 1, 1);
	v.t = TVector2(1, 1);

	m_vLowControlPosList[m_iPos] = v.p;

	//v.p = high;
	D3DXVec3TransformCoord(&v.p, &high, &matInv);
	m_vHighControlPosList[m_iPos] = v.p;
	m_iPos++;

	m_vLowCatmullRomList.clear();
	m_vHighCatmullRomList.clear();
	InterpolateCatmullRom(m_vLowControlPosList, &m_vLowCatmullRomList, m_iPos, m_iSampleNum);
	InterpolateCatmullRom(m_vHighControlPosList, &m_vHighCatmullRomList, m_iPos, m_iSampleNum);
	
	size_t minSize = min(m_vLowCatmullRomList.size(), m_vHighCatmullRomList.size());
	m_iCatmullRomSize = minSize * 2;
	m_iMaxCurveSize = max(m_iMaxCurveSize, m_iCatmullRomSize);

	int start = max(((m_iPos - 2) * m_iSampleNum), 0);
	for (int i = start; i < minSize; i++)
	{
		float delta = i / (float)minSize;
		//delta = i / (float)450;

		v.c = TVector4(1, 1, 1, 1);
		v.p = m_vLowCatmullRomList[i];
		m_VertexCatmullRomList[i * 2 + 0] = v;

		v.p = m_vHighCatmullRomList[i];
		m_VertexCatmullRomList[i * 2 + 1] = v;
	}
}

void	TrailEffect::ResetTrail(TMatrix* matWorld)
{
	m_matWorld = *matWorld;
	m_iPos = 0;
}