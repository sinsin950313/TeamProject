#include "FNode.h"

void FNode::CreateChildNode(FNode* pParent, MeshMap* pMap)
{
	// 0  1   2   3   4
	// 5  6   7   8   9
	// 10 11  12  13  14
	// 15 16  17  18  19
	// 20  21 22  23  24
	TRect rt = pParent->m_rt;
	DWORD dwLT = pParent->m_dwCorner[0];
	DWORD dwRT = pParent->m_dwCorner[1];
	DWORD dwLB = pParent->m_dwCorner[2];
	DWORD dwRB = pParent->m_dwCorner[3];
	DWORD dwCenterT = (dwLT + dwRT) / 2;
	DWORD dwCenterB = (dwLB + dwRB) / 2;
	DWORD dwCenterL = (dwLT + dwLB) / 2;
	DWORD dwCenterR = (dwRT + dwRB) / 2;
	DWORD dwCenter = (dwLT + dwRT + dwLB + dwRB) / 4;
	m_pChild[0] = new FNode(pParent, pMap, dwLT, dwCenterT, dwCenterL, dwCenter);
	m_pChild[1] = new FNode(pParent, pMap, dwCenterT, dwRT, dwCenter, dwCenterR);
	m_pChild[2] = new FNode(pParent, pMap, dwCenterL, dwCenter, dwLB, dwCenterB);
	m_pChild[3] = new FNode(pParent, pMap, dwCenter, dwCenterR, dwCenterB, dwRB);
}

void FNode::CreateIndexData(MeshMap* pMap)
{
	// 0  1   2   3   4
	// 5  6   7   8   9
	// 10 11  12  13  14
	// 15 16  17  18  19
	// 20  21 22  23  24
	DWORD dwLT = m_dwCorner[0];
	DWORD dwRT = m_dwCorner[1];
	DWORD dwLB = m_dwCorner[2];
	DWORD dwRB = m_dwCorner[3];
	DWORD dwNumRowCell = (dwLB - dwLT) / pMap->m_dwNumColumns;
	DWORD dwNumColCell = dwRT - dwLT;
	DWORD dwNumCells = dwNumRowCell * dwNumColCell;
	m_IndexList.resize(dwNumCells * 2 * 3); //mesh * 2 * 3(vertex)
	int iIndex = 0;

	m_Box.vMin.x = pMap->GetListVertex()[dwLB].pos.x;
	m_Box.vMin.y = 100000.0f;
	m_Box.vMin.z = pMap->GetListVertex()[dwLB].pos.z;
	m_Box.vMax.x = pMap->GetListVertex()[dwRT].pos.x;
	m_Box.vMax.y = -100000.0f;
	m_Box.vMax.z = pMap->GetListVertex()[dwRT].pos.z;

	for (int dwRow = 0; dwRow < dwNumRowCell; dwRow++)
	{
		for (int dwCol = 0; dwCol < dwNumColCell; dwCol++)
		{
			m_IndexList[iIndex + 0] = dwLT + dwCol + (dwRow * pMap->m_dwNumColumns);
			m_IndexList[iIndex + 1] = m_IndexList[iIndex + 0] + 1;
			m_IndexList[iIndex + 2] = dwLT + dwCol + ((dwRow+1) * pMap->m_dwNumColumns);
			m_IndexList[iIndex + 3] = m_IndexList[iIndex + 2];
			m_IndexList[iIndex + 4] = m_IndexList[iIndex + 1];
			m_IndexList[iIndex + 5] = m_IndexList[iIndex + 2] + 1;

			for (DWORD dwVertex = 0; dwVertex < 6; dwVertex++)
			{
				if (m_Box.vMin.y > pMap->GetListVertex()[m_IndexList[iIndex + dwVertex]].pos.y)
				{
					m_Box.vMin.y = pMap->GetListVertex()[m_IndexList[iIndex + dwVertex]].pos.y;
				}
				if (m_Box.vMax.y < pMap->GetListVertex()[m_IndexList[iIndex + dwVertex]].pos.y)
				{
					m_Box.vMax.y = pMap->GetListVertex()[m_IndexList[iIndex + dwVertex]].pos.y;
				}
			}
			iIndex += 6;
		}
	}
	m_dwFace = m_IndexList.size() / 3;
	m_Box.vCenter = (m_Box.vMax + m_Box.vMin) * 0.5f;
	m_Box.vAxis[0] = { 1,0,0 };
	m_Box.vAxis[1] = { 0,1,0 };
	m_Box.vAxis[2] = { 0,0,1 };
	m_Box.fExtent[0] = m_Box.vMax.x - m_Box.vCenter.x;
	m_Box.fExtent[1] = m_Box.vMax.y - m_Box.vCenter.y;
	m_Box.fExtent[2] = m_Box.vMax.z - m_Box.vCenter.z;
}

HRESULT FNode::CreateIndexBuffer(MeshMap* pMap)
{
	HRESULT hr;
	CreateIndexData(pMap);
	D3D11_BUFFER_DESC       bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.ByteWidth = sizeof(DWORD) * m_IndexList.size(); // 바이트 용량
	// GPU 메모리에 할당
	bd.Usage = D3D11_USAGE_DEFAULT; // 버퍼의 할당 장소 내지는 버퍼용도
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA  sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = &m_IndexList.at(0);
	hr = g_pDevice->CreateBuffer(
		&bd, // 버퍼 할당
		&sd, // 초기 할당된 버퍼를 체우는 CPU메모리 주소
		&m_pIndexBuffer);
	return hr;
}


FNode::FNode(FNode* pParent, MeshMap* pMap, DWORD dwLT, DWORD dwRT, DWORD dwLB, DWORD dwRB)
{
	if (pParent != nullptr)
	{
		m_iDepth = pParent->m_iDepth + 1;
	}
	m_dwCorner[0] = dwLT;
	m_dwCorner[1] = dwRT;
	m_dwCorner[2] = dwLB;
	m_dwCorner[3] = dwRB;
	m_bLeaf = FALSE;
	m_pChild.resize(4);

	// boundingBox( aabb, obb )
	// 0  1   2   3   4
	// 5  6   7   8   9
	// 10 11  12  13  14
	// 15 16  17  18  19
	// 20  21 22  23  24

	CreateIndexBuffer(pMap);
}

FNode::~FNode()
{
	if (m_pIndexBuffer) m_pIndexBuffer->Release();
	delete m_pChild[0];
	delete m_pChild[1];
	delete m_pChild[2];
	delete m_pChild[3];
	m_pChild[0] = nullptr;
	m_pChild[1] = nullptr;
	m_pChild[2] = nullptr;
	m_pChild[3] = nullptr;
}

//std::ofstream& operator<<(std::ostream& os, const FNode& pNode)
//{
//	os<<
//}
