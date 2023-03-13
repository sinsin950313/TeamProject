#pragma once
#include "FNode.h"
#include "MeshMap.h"
#include "FSelect.h"
#include "Object.h"

class FQuadTree
{

public:
	void	SetPickingSimple(bool bPicking, float fLength);
	void	SetPickingFbx(std::wstring szCurrentFbx, bool bPicking);
	void	SetPickingObject(bool bPicking);
	void	SetPickingSculpt(bool bPicking);
	void	SetSculptRadius(float fRadius);
	void	SetSculptIntensity(float fIntensity);
	void	SetSplatRadius(float fRadius);
	void	SetSplattingTexture(Texture* pTexture);
	void	SetSplatting(std::wstring szCurrentSplat, bool bSplatting);

	void	SetTransform(Transform transform);
	void	SetMaterial(Material* pMaterial);
	void	SetTexture(Texture* pTexture);
	void	SetShader(std::wstring vsPath, VertexShader* pVertexShader, std::wstring psPath, PixelShader* pPixelShader);
	void	SetConstantData(constant_map cc);
	void	SetDrawMode(DRAW_MODE mode);
	void	SetSpecify(OBJECT_SPECIFY specify);
	UINT	SelectVertexList(T_BOX& box, std::vector<FNode*>& selectNodeList);
	void	UpdateNode(FNode* pNode);
	Object* GetPickingObject();
	BOOL	AddObject(Object* pObj);
	BOOL	DeleteObject(Object* pObj);
	void	BuildTree(FNode* pNode, MeshMap* pMap);
	BOOL	IsSubDivide(FNode* pNode);
	FNode*	FindNode(FNode* pNode, Object* pObj);
	void	Reset(FNode* pNode);
	FNode*	VisibleNode(FNode* pNode);
	bool	GetInterSection();
	Object*	ObjectPicking();
	void	Update();
	void	Render();

public:
	BYTE* m_fAlphaData;
	ID3D11Texture2D* m_pMaskAlphaTexture;
	ID3D11ShaderResourceView* m_pMaskAlphaSrv;
	HRESULT CreateAlphaTexture(DWORD dwWidth, DWORD dwHeight, BYTE* fAlphaData);
	void    Splatting(XMVECTOR vIntersection, std::wstring szFullPath);

public:
	friend std::ostream& operator<<(std::ostream& os, const FQuadTree* pQuadTree);
	//friend std::ifstream& operator>>(std::ifstream& is, FQuadTree* pQuadTree);

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
	std::vector<Texture*> m_ListTextureSplatting;

	int m_iMaxDepth;
	FNode* m_pRootNode;
	FSelect m_Select;
	MeshMap* m_pMap = nullptr;
	Material* m_pMaterial;
	Texture* m_pTexture;
	
	Transform m_Transform;
	constant_map m_constantDataMap;
	ConstantBuffer* m_pConstantBuffer;
	std::wstring m_szVSPath;
	VertexShader* m_pVertexShader;
	std::wstring m_szPSPath;
	PixelShader* m_pPixelShader;
	Camera* m_pCamera = nullptr;
	DRAW_MODE m_DrawMode = DRAW_MODE::MODE_SOLID;
	OBJECT_SPECIFY m_Specify = OBJECT_SPECIFY::OBJECT_MAP;
	std::unordered_set<Object*> m_pAllObjectList;
	std::vector<FNode*> m_pLeafNodeList;
	std::vector<FNode*> m_pDrawLeafNodeList;

	friend class FNode;
	friend class ToolSystemMap;
};

