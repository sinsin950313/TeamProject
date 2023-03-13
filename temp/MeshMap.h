#pragma once
#include "Mesh.h"
#include "CameraSystem.h"
struct face_info
{
	UINT     vertexArray[3];
	XMVECTOR vNormal;
};
struct vertex_info
{
	std::vector<UINT> faceIndexArray;
	XMVECTOR vNormal;
};

namespace MAPLOAD
{
	void OpenFile(std::wstring szFullPath);
};

class MeshMap : public Mesh
{
public:
	std::vector<object>& GetListVertex();
	std::vector<DWORD>& GetListIndex();
	void	UpdateBuffer(Camera* pCamera);
	float	GetHeightmap(int row, int col);
	float	GetHeight(float fPosX, float fPosZ);
	void	LoadHeightMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, std::wstring szFullPath);	//높이맵을만드는함수
	void	GenerateVertexNormal();																		//각 버텍스정점의 법선노말생성
	XMVECTOR ComputeFaceNormal(UINT i0, UINT i1, UINT i2);
	void	ComputeVertexNormal(UINT iVertex);
	friend std::ostream& operator<<(std::ostream& os, const MeshMap* pMap);
	friend std::ifstream& operator>>(std::ifstream& is, MeshMap* pMap);

public:
	MeshMap();
	MeshMap(UINT iWidth, UINT iHeight, float fCellDistance);
	virtual ~MeshMap();
public:
	DWORD m_dwNumRows;
	DWORD m_dwNumColumns;
	DWORD m_dwFace;
	VertexBuffer* m_pVertexBuffer;
	IndexBuffer* m_pIndexBuffer;
	std::vector<object> m_ListVertex;	//정점정보
	std::vector<DWORD> m_ListIndex; //인덱스정보
	std::vector<float> m_fHeightList;	//높이맵의높이정보
	std::vector<vertex_info> m_ListVertexInfo;	//정점법선노말
	std::vector<face_info> m_ListFaceNormal;	//표면법선노말
	float m_fCellDistance = 1.0f;
	friend class FNode;
	friend class FQuadTree;
	friend class ToolSystemMap;
};

