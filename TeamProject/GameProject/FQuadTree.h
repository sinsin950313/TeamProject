#pragma once
#include "FNode.h"
#include "MeshMap.h"
#include "TextureMgr.h"
#include "ShaderMgr.h"
#include "CameraTPS.h"
#include "std.h"

class Object;
class FQuadTree
{
public:
	ID3D11Device* m_pd3dDevice = nullptr;
	ID3D11DeviceContext* m_pImmediateContext = nullptr;
	void	SetDevice(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	void	SetSplattingTexture(Texture* pTexture);

	void	SetTransform(Transform transform);
	void	SetTexture(Texture* pTexture);
	void	SetShader(std::wstring vsPath, Shader* pVertexShader, std::wstring psPath, Shader* pPixelShader);
	void	SetConstantData(constant_map cc);
	UINT	SelectVertexList(T_BOX& box, std::vector<FNode*>& selectNodeList);
	BOOL	AddObject(Object* pObj);
	BOOL	DeleteObject(Object* pObj);
	void	BuildTree(FNode* pNode, MeshMap* pMap);
	BOOL	IsSubDivide(FNode* pNode);
	FNode*	FindNode(FNode* pNode, Object* pObj);
	void	Reset(FNode* pNode);
	FNode*	VisibleNode(FNode* pNode);
	void	Update();
	void	PreRender();
	void	Render();

public:
	BYTE* m_fAlphaData;
	ID3D11Texture2D* m_pMaskAlphaTexture;
	ID3D11ShaderResourceView* m_pMaskAlphaSrv;
	HRESULT CreateAlphaTexture(DWORD dwWidth, DWORD dwHeight, BYTE* fAlphaData);

public:

	FQuadTree(Camera* pCamera, MeshMap* pMap, int iMaxDepth = 2, BYTE* fAlphaData = 0);
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
	constant_map m_constantDataMap;
	ID3D11Buffer* m_pConstantBuffer;

	std::wstring m_szVSPath;
	Shader* m_pVertexShader;
	std::wstring m_szPSPath;
	Shader* m_pPixelShader;

	Camera* m_pCamera = nullptr;
	//std::unordered_set<Object*> m_pAllObjectList;
	std::vector<FNode*> m_pLeafNodeList;
	std::vector<FNode*> m_pDrawLeafNodeList;

};

