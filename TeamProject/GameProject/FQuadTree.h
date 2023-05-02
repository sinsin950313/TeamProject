#pragma once
#include "FNode.h"
#include "MeshMap.h"
#include "TextureMgr.h"
#include "ShaderMgr.h"
#include "std.h"
//-----------------------------
#include <unordered_set>
#include "Object.h"
#include "CollisionMgr.h"
#include "CameraTPS.h"
#include "BaseObject.h"
#include "Foliage.h"
//-----------------------------

class Camera;
class Object;
class FQuadTree
{
public:
	ID3D11Device* m_pd3dDevice = nullptr;
	ID3D11DeviceContext* m_pImmediateContext = nullptr;
	void	SetDevice(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	void	SetSplattingTexture(Texture* pTexture);
	void	SetMatrix(TMatrix* matWorld, TMatrix* matView, TMatrix* matProj);
	void	SetTransform(Transform transform);
	void	SetTexture(Texture* pTexture);
	void	SetShader(std::wstring vsPath, Shader* pVertexShader, std::wstring psPath, Shader* pPixelShader);
	void	SetConstantData(ConstantData_Transform constantData);
	void	SetConstantData(ConstantData_Map constantData);
	void	SetConstantData(ConstantData_Light constantData);
	//UINT	SelectVertexList(T_BOX& box, std::vector<FNode*>& selectNodeList);
	BOOL	AddObject(Object* pObj);
	BOOL	DeleteObject(Object* pObj);
	void	BuildTree(FNode* pNode, MeshMap* pMap);
	BOOL	IsSubDivide(FNode* pNode);
	FNode*	FindCollisionNode(FNode* pNode, Object* pObj);
	void	Reset(FNode* pNode);
	FNode*	VisibleNode(FNode* pNode);
	void	Update();
	void	PreRender();
	void	Render();
	void	Release();

public:
	BYTE* m_fAlphaData;
	ID3D11Texture2D* m_pMaskAlphaTexture;
	ID3D11ShaderResourceView* m_pMaskAlphaSrv;
	HRESULT CreateAlphaTexture(DWORD dwWidth, DWORD dwHeight, BYTE* fAlphaData);

public:

	FQuadTree(MeshMap* pMap, ID3D11Device* pDevice, ID3D11DeviceContext* pContext, int iMaxDepth = 2, BYTE* fAlphaData = 0);
	~FQuadTree();

public:

	bool m_bSclupting;
	float m_fSculptRadius = 10.0f;
	float m_fSculptIntensity = 10.0f;

	bool m_bSimplePicking;
	float m_fObjLength = 1.0f;

	bool m_bFbxPicking;
	std::wstring m_szCurrentImage;

	bool m_bObjectPicking;
	std::wstring m_szCurrentFbx;

	bool m_bSplatting;
	float m_fSplattingRadius = 5.0f;
	std::wstring m_szCurrentSplat;

	Texture* m_pTexture = nullptr;
	std::vector<Texture*> m_ListTextureSplatting;

	int m_iMaxDepth;
	FNode* m_pRootNode;
	MeshMap* m_pMap = nullptr;
	
	
	Transform m_Transform;
	ID3D11Buffer* m_pConstantBuffer_Transform;
	ConstantData_Transform m_ConstantData_Transform;
	ID3D11Buffer* m_pConstantBuffer_Map;
	ConstantData_Map m_ConstantData_Map;
	ID3D11Buffer* m_pConstantBuffer_Light;
	ConstantData_Light m_ConstantData_Light;
	//ID3D11Buffer* m_pConstantBuffer_Fog;
	//ConstantData_Fog m_ConstantData_Fog;

	std::wstring m_szVSPath;
	Shader* m_pVertexShader;
	
	std::wstring m_szPSPath;
	Shader* m_pPixelShader;
	std::map<std::string, Transform> m_PlayerSpawnPoint;
	std::vector<std::pair<std::string, Transform>> m_EnemySpawnList;
	std::map<std::wstring, T_BOX> m_TriggerList;
	std::unordered_set<Object*> m_pAllObjectList;
	std::vector<FNode*> m_pLeafNodeList;
	std::vector<FNode*> m_pDrawLeafNodeList;

	//Camera
	Camera* m_pCurrentCamera = nullptr;

	//SkyDome
	BaseObject* m_pSphereObject = nullptr;

public:
	std::map<std::wstring, Cinema> m_CinemaList;
	Cinema m_CurrentCinema;
	float m_fCamMoveCurrent = 0.0f;
};


namespace MAPLOAD
{
	FQuadTree* OpenMap(std::wstring szFullPath, ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext);
	void	PathChanger(std::string& str);
};
using namespace MAPLOAD;