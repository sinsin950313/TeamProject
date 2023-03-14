#include "FQuadTree.h"

void	FQuadTree::SetDevice(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pd3dDevice = pDevice;
    m_pImmediateContext = pContext;
}

HRESULT FQuadTree::CreateAlphaTexture(DWORD dwWidth, DWORD dwHeight, BYTE* fAlphaData)
{
    HRESULT hr;
    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width = dwWidth;
    desc.Height = dwHeight;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    if (fAlphaData != nullptr)
        m_fAlphaData = fAlphaData;
    else
    {
        m_fAlphaData = new BYTE[dwWidth * dwHeight * 4];
        for (UINT y = 0; y < dwHeight; y++)
        {
            for (UINT x = 0; x < dwWidth; x++)
            {
                BYTE* pixel = &m_fAlphaData[dwWidth * y * 4 + x * 4];
                pixel[0] = 0;//r
                pixel[1] = 0;//g
                pixel[2] = 0;//b
                pixel[3] = 0;//a
            }
        }
    }
    
    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = m_fAlphaData;
    initData.SysMemPitch = sizeof(BYTE) * 4 * dwWidth;
    initData.SysMemSlicePitch = 0;
    if (FAILED(hr = m_pd3dDevice->CreateTexture2D(&desc, &initData, &m_pMaskAlphaTexture)))
    {
        return hr;
    }
    if (FAILED(hr = m_pd3dDevice->CreateShaderResourceView(m_pMaskAlphaTexture, NULL, &m_pMaskAlphaSrv)))
    {
        return hr;
    }

    return hr;
}

#include "BaseObject.h"
FQuadTree::FQuadTree(MeshMap* pMap, ID3D11Device* pDevice, ID3D11DeviceContext* pContext, int iMaxDepth, BYTE* fAlphaData)
{
    SetDevice(pDevice, pContext);
    m_pMap = pMap;
    m_iMaxDepth = iMaxDepth;
    m_pRootNode = new FNode(nullptr, pMap, 0, m_pMap->m_dwNumColumns - 1, m_pMap->m_dwNumRows * (m_pMap->m_dwNumColumns - 1), m_pMap->m_dwNumRows * m_pMap->m_dwNumColumns - 1);

    m_constantDataMap.matWorld = XMMatrixIdentity();
    m_constantDataMap.matView = XMMatrixIdentity();
    m_constantDataMap.matProj = XMMatrixIdentity();
    m_pConstantBuffer = DX::CreateConstantBuffer(pDevice, &m_constantDataMap, sizeof(m_constantDataMap));

    CreateAlphaTexture(m_pMap->m_dwNumRows, m_pMap->m_dwNumColumns, fAlphaData);
    BuildTree(m_pRootNode, pMap);
}

FQuadTree::~FQuadTree()
{
    Release();
}

/*
UINT FQuadTree::SelectVertexList(T_BOX& box, std::vector<FNode*>& selectNodeList)
{
    for (auto node : m_pDrawLeafNodeList)
    {
        if (node != nullptr)
        {
            TCollisionType ret = TCollision::BoxToBox(node->m_Box, box);
            if (ret > 0)
            {
                selectNodeList.push_back(node);
            }
        }
    }
    return selectNodeList.size();
}
*/

void FQuadTree::SetSplattingTexture(Texture* pTexture)
{
    m_ListTextureSplatting.push_back(pTexture);
}

void FQuadTree::SetTransform(Transform transform)
{
    m_Transform = transform;

    XMVECTOR scale = m_Transform.scale;
    XMFLOAT3 rotateAngle;
    XMStoreFloat3(&rotateAngle, m_Transform.rotation);
    XMVECTOR rotation =
        XMQuaternionRotationRollPitchYaw(
            XMConvertToRadians(rotateAngle.x),
            XMConvertToRadians(rotateAngle.y),
            XMConvertToRadians(rotateAngle.z));
    XMVECTOR translation = m_Transform.position;
    m_constantDataMap.matWorld = XMMatrixTransformation({ 0,0,0,1 }, { 0,0,0,1 }, scale, { 0,0,0,1 }, rotation, translation);
}

void FQuadTree::SetTexture(Texture* pTexture)
{
    m_pTexture = pTexture;
}

void FQuadTree::SetShader(std::wstring vsPath, Shader* pVertexShader, std::wstring psPath, Shader* pPixelShader)
{
    m_szVSPath = vsPath;
    m_pVertexShader = pVertexShader;
    m_szPSPath = psPath;
    m_pPixelShader = pPixelShader;
}


void FQuadTree::SetConstantData(constant_map cc)
{
    m_constantDataMap = cc;
}

BOOL FQuadTree::AddObject(Object* pObj)
{
    return FALSE;
    //m_pAllObjectList.insert(pObj);

    FNode* pFindNode = FindNode(m_pRootNode, pObj);
    if (pFindNode != nullptr)
    {
        pFindNode->m_pDynamicObjectList.push_back(pObj);
        //pFindNode->m_pStaticObjectList.push_back(pObj);
        return TRUE;
    }
    return FALSE;
}

BOOL FQuadTree::DeleteObject(Object* pObj)
{
    return FALSE;
    //return m_pAllObjectList.erase(pObj);
}

void FQuadTree::BuildTree(FNode* pNode, MeshMap* pMap)
{
    if (pNode == nullptr) return;
    if (!IsSubDivide(pNode))
    {
        pNode->m_bLeaf = true;
        m_pLeafNodeList.push_back(pNode);
        return;
    }
    pNode->CreateChildNode(pNode, pMap);

    BuildTree(pNode->m_pChild[0], pMap);
    BuildTree(pNode->m_pChild[1], pMap);
    BuildTree(pNode->m_pChild[2], pMap);
    BuildTree(pNode->m_pChild[3], pMap);
}

BOOL FQuadTree::IsSubDivide(FNode* pNode)
{
    if ((pNode->m_dwCorner[1] - pNode->m_dwCorner[0]) == 1) return FALSE; //이이상 분해할수 없으면 
    if (pNode->m_iDepth < m_iMaxDepth) return TRUE;
    return FALSE;
}

FNode* FQuadTree::FindNode(FNode* pNode, Object* pObj)
{
    for (int i = 0; i < 4; i++)
    {
        if (pNode->m_pChild[i] != nullptr)
        {
            //if (TCollision::BoxToBox(pNode->m_pChild[i]->m_Box, pObj->m_Box))
            //{
            //    pNode = FindNode(pNode->m_pChild[i], pObj);
            //    break;
            //}
        }
    }
    return pNode;
}

void FQuadTree::Reset(FNode* pNode)
{
    if (pNode == nullptr) return;
    pNode->m_pDynamicObjectList.clear();
    Reset(pNode->m_pChild[0]);
    Reset(pNode->m_pChild[1]);
    Reset(pNode->m_pChild[2]);
    Reset(pNode->m_pChild[3]);
}

FNode* FQuadTree::VisibleNode(FNode* pNode)
{
    if (pNode->m_bLeaf)
    {
        m_pDrawLeafNodeList.push_back(pNode);
        return pNode;
    }
    for (int i = 0; i < 4; i++)
    {
        VisibleNode(pNode->m_pChild[i]);
    }
    //F_POSITION dwRet = m_pCamera->m_Frustum.ClassifyBox(pNode->m_Box);
    //if (F_FRONT == dwRet)// 완전포함.
    //{
    //    m_pDrawLeafNodeList.push_back(pNode);
    //    return pNode;
    //}
    //if (F_SPANNING == dwRet) // 걸쳐있다.
    //{
    //    if (pNode->m_bLeaf)
    //    {
    //        m_pDrawLeafNodeList.push_back(pNode);
    //    }
    //    else
    //    {
    //        for (int iNode = 0; iNode < 4; iNode++)
    //        {
    //            VisibleNode(pNode->m_pChild[iNode]);
    //        }
    //    }
    //}
    //return pNode;
}

void	FQuadTree::SetMatrix(TMatrix* matWorld, TMatrix* matView, TMatrix* matProj)
{
    //m_constantDataMap.matWorld;
    if (matWorld)
    {
        m_constantDataMap.matWorld = XMLoadFloat4x4(&(*matWorld));
        //m_constantDataMap.matWorld = XMMatrixTranspose(world);
    }
    if (matView)
    {
        m_constantDataMap.matView = XMLoadFloat4x4(&(*matView));
        //m_constantDataMap.matView = XMMatrixTranspose(view);
    }
    if (matProj)
    {
        m_constantDataMap.matProj = XMLoadFloat4x4(&(*matProj));
        //m_constantDataMap.matProj = XMMatrixTranspose(proj);
    }
    m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, NULL, NULL, &m_constantDataMap, NULL, NULL);
}

void FQuadTree::Update()
{
    //m_constantDataMap.matView = _CameraSystem.GetCurrentCamera()->m_matCamera;
    //m_constantDataMap.m_camera_position = XMFLOAT4(
    //    XMVectorGetX(_CameraSystem.GetCurrentCamera()->m_matWorld.r[3]),
    //    XMVectorGetY(_CameraSystem.GetCurrentCamera()->m_matWorld.r[3]),
    //    XMVectorGetZ(_CameraSystem.GetCurrentCamera()->m_matWorld.r[3]),
    //    XMVectorGetW(_CameraSystem.GetCurrentCamera()->m_matWorld.r[3]));
    m_constantDataMap.m_world_size = XMFLOAT2(m_pMap->m_dwNumColumns, m_pMap->m_dwNumRows);
    m_constantDataMap.m_cell_distance = m_pMap->m_fCellDistance;
    //_EngineSystem.GetRenderSystem()->UpdateConstantBuffer(m_pConstantBuffer, &m_constantDataMap);

    m_pDrawLeafNodeList.clear();
    VisibleNode(m_pRootNode); //재귀로 VisibleNode체크

    //m_Select.SetMatrix(nullptr, &m_pCamera->m_matCamera, &m_pCamera->m_matProj);

    m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, NULL, NULL, &m_constantDataMap, NULL, NULL);
}

void	FQuadTree::PreRender()
{
    m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
    m_pImmediateContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);
    m_pImmediateContext->VSSetShader(m_pVertexShader->m_pVS, NULL, 0);
    m_pImmediateContext->PSSetShader(m_pPixelShader->m_pPS, NULL, 0);

    UINT stride = sizeof(PTNC); //정점의크기
    UINT offset = 0;          //정점의오프셋
    m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pMap->m_pVertexBuffer, &stride, &offset);	// VertexBuffer를 세팅, 1은 버퍼의갯수
    m_pImmediateContext->IASetInputLayout(m_pMap->m_pVertexInputLayout);

    m_pImmediateContext->VSSetShaderResources(0, 1, &m_pTexture->m_pTextureSRV);
    m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTexture->m_pTextureSRV);
    m_pImmediateContext->PSSetShaderResources(1, 1, &m_pMaskAlphaSrv);

    for (int idx = 0; idx < m_ListTextureSplatting.size(); idx++)
        m_pImmediateContext->PSSetShaderResources(2 + idx, 1, &m_ListTextureSplatting[idx]->m_pTextureSRV);
}

void FQuadTree::Render()
{
    PreRender();
    for (int idx = 0;  idx < m_pDrawLeafNodeList.size(); idx++)
    {
        m_pImmediateContext->IASetIndexBuffer(m_pDrawLeafNodeList[idx]->m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

        m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);		//TrangleList를 Index로그린다
        m_pImmediateContext->DrawIndexed(m_pDrawLeafNodeList[idx]->m_IndexList.size(), 0, 0);
    }
}

void	FQuadTree::Release()
{
    if (m_fAlphaData)
    {
        delete[] m_fAlphaData;
        m_fAlphaData = nullptr;
    }
    if (m_pMaskAlphaTexture)
    {
        m_pMaskAlphaTexture->Release();
        m_pMaskAlphaTexture = nullptr;
    }
    if (m_pMaskAlphaSrv)
    {
        m_pMaskAlphaSrv->Release();
        m_pMaskAlphaSrv = nullptr;
    }
    if (m_pRootNode)
    {
        delete m_pRootNode;
        m_pRootNode = nullptr;
    }
    if (m_pConstantBuffer)
    {
        m_pConstantBuffer->Release();
        m_pConstantBuffer = nullptr;
    }
    if (m_pMap)
    {
        delete m_pMap;
        m_pMap = nullptr;
    }
}


namespace MAPLOAD
{
	FQuadTree* OpenMap(std::wstring szFullPath, ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext)
	{
		Texture* pTexture = nullptr;
		std::vector<Texture*> pTexList;
		Transform mapTransform = {};
		UINT iMaxDepth = 0;
		std::wstring szVSPath;
		std::wstring szPSPath;
		void* shader_byte_code_vs = nullptr;
		void* shader_byte_code_ps = nullptr;
		size_t size_shader_vs = 0;
		size_t size_shader_ps = 0;
		MeshMap* pMapMesh = new MeshMap();
		pMapMesh->SetDevice(pd3dDevice, pContext);
		//std::unordered_set<Object*> allObjectList;
		BYTE* fAlphaData = nullptr;
		std::ifstream is(szFullPath);
		std::string line;
		while (std::getline(is, line))
		{
			std::istringstream iss(line);
			std::string fieldName;
			if (std::getline(iss, fieldName, ':'))
			{
				if (fieldName == "m_pTexture")
				{
					std::string textureName;
					iss >> textureName;
                    PathChanger(textureName);
					I_Tex.Load(to_mw(textureName), &pTexture);
				}
				else if (fieldName == "m_ListTextureSplatting")
				{
					std::string texturesStr;
					std::getline(iss, texturesStr);
					std::stringstream texturesStream(texturesStr);
					std::string texturePath;
					while (std::getline(texturesStream, texturePath, ','))
					{
						if (texturePath.size() > 1)
						{
							texturePath.erase(std::remove(texturePath.begin(), texturePath.end(), ' '), texturePath.end());
							Texture* texture;
                            PathChanger(texturePath);
							I_Tex.Load(to_mw(texturePath), &texture);
							pTexList.push_back(texture);
						}
					}
				}
				else if (fieldName == "m_Transform")
				{
					iss >> mapTransform;
				}
				else if (fieldName == "m_iMaxDepth")
				{
					iss >> iMaxDepth;
				}
				else if (fieldName == "m_szVSName")
				{
					std::string str;
					std::getline(iss, str);
					szVSPath = to_mw(str);
				}
				else if (fieldName == "m_szPSName")
				{
					std::string str;
					std::getline(iss, str);
					szPSPath = to_mw(str);
				}
				else if (fieldName == "m_pMap")
				{
					is >> pMapMesh;
				}
				else if (fieldName == "m_pAllObjectList")
				{
					std::streampos prevPos = is.tellg();
					std::string str;
					while (std::getline(is, str))
					{
						if (str.find("m_fAlphaData:") != std::string::npos)
							break;
						std::stringstream texturesStream(str);
						std::string strName;
						std::getline(texturesStream, strName, ',');

						size_t  pos_start = texturesStream.str().find("m_CullMode:") + strlen("m_CullMode:");
						size_t  pos_end = texturesStream.str().find(",", pos_start);
						std::string cullMode = texturesStream.str().substr(pos_start, pos_end - pos_start);

						/*
						if (pos_str == "CULL_MODE_FRONT")
							mode = CULL_MODE::CULL_MODE_FRONT;
						else if (pos_str == "CULL_MODE_BACK")
							mode = CULL_MODE::CULL_MODE_BACK;
						else if (pos_str == "CULL_MODE_NONE")
							mode = CULL_MODE::CULL_MODE_NONE;
						*/

						//CULL_MODE cullMode;
						//texturesStream >> cullMode;

						pos_start = texturesStream.str().find("m_DrawMode:") + strlen("m_DrawMode:");
						pos_end = texturesStream.str().find(",", pos_start);
						std::string drawMode = texturesStream.str().substr(pos_start, pos_end - pos_start);

						//DRAW_MODE drawMode;
						//texturesStream >> drawMode;

						pos_start = texturesStream.str().find("m_InteractiveMode:") + strlen("m_InteractiveMode:");
						pos_end = texturesStream.str().find(",", pos_start);
						std::string interactiveMode = texturesStream.str().substr(pos_start, pos_end - pos_start);

						//INTERACTIVE_MODE interactiveMode;
						//texturesStream >> interactiveMode;

						pos_start = texturesStream.str().find("m_Specify:") + strlen("m_Specify:");
						pos_end = texturesStream.str().find(",", pos_start);
						std::string specifyMode = texturesStream.str().substr(pos_start, pos_end - pos_start);

						//OBJECT_SPECIFY specifyMode;
						//texturesStream >> specifyMode;

						Transform transform;
						texturesStream >> transform;

						float length;

						if (specifyMode == "OBJECT_SIMPLE")
						{
							// pos 값을 추출합니다.
							size_t pos_start = texturesStream.str().find("m_fLength:") + strlen("m_fLength:");
							size_t pos_end = texturesStream.str().find(",", pos_start);
							std::string pos_str = texturesStream.str().substr(pos_start, pos_end - pos_start);
							std::istringstream pos_stream(pos_str);
							pos_stream >> length;
						}

						//Object* pObject = nullptr;
						//if (specifyMode == OBJECT_SPECIFY::OBJECT_SIMPLE)
						//    pObject = CreateSimpleBox(length, transform.position, transform.rotation, transform.scale);
						//else if (specifyMode == OBJECT_SPECIFY::OBJECT_STATIC)
						//    pObject = CreateFbxObject(_tomw(strName), transform.position, transform.rotation, transform.scale);
						//else if (specifyMode == OBJECT_SPECIFY::OBJECT_SKELETON)
						//    pObject = CreateFbxObject(_tomw(str), transform.position, transform.rotation, transform.scale);
						//
						//allObjectList.insert(pObject);
						prevPos = is.tellg();
					}
					is.seekg(prevPos);
				}
				else if (fieldName == "m_fAlphaData")
				{
					fAlphaData = new BYTE[pMapMesh->m_dwNumRows * pMapMesh->m_dwNumColumns * 4];
					for (int idx = 0; idx < pMapMesh->m_dwNumRows * pMapMesh->m_dwNumColumns * 4; idx++)
					{
						int rgba = 0;
						iss >> rgba;
						fAlphaData[idx] = static_cast<uint8_t>(rgba);
					}
				}
			}
		}

		is.close();

		constant_map cc;
		cc.matWorld = XMMatrixIdentity();
		cc.matView = XMMatrixIdentity();
		cc.matProj = XMMatrixIdentity();

		//_EngineSystem.GetMeshSystem()->AddResource(L"MapMesh", pMapMesh);

        std::wstring DefaultShaderPath = L"../../data/shader/MAP/";
		Shader* pVertexShader;
		I_Shader.VSLoad(DefaultShaderPath + szVSPath, L"vsmain", &pVertexShader);
		Shader* pPixelShader;
		I_Shader.PSLoad(DefaultShaderPath + szPSPath, L"psmain", &pPixelShader);

		//VertexBuffer* pVertexBuffer = _EngineSystem.GetRenderSystem()->CreateVertexBuffer(&pMapMesh->GetListVertex()[0], sizeof(PTNC), pMapMesh->GetListVertex().size(), shader_byte_code_vs, size_shader_vs);
		//IndexBuffer* pIndexBuffer = _EngineSystem.GetRenderSystem()->CreateIndexBuffer(&pMapMesh->GetListIndex()[0], pMapMesh->GetListIndex().size());

		pMapMesh->m_pVertexBuffer =
			DX::CreateVertexBuffer(pd3dDevice, &pMapMesh->GetListVertex()[0], sizeof(PTNC), pMapMesh->GetListVertex().size());
        D3D11_INPUT_ELEMENT_DESC layout[] =
        {
            //SEMANTIC NAME, SEMANTIC INDEX, FORMAT, INPUT SLOT, ALIGNED BYTE OFFSET, INPUT SLOT CLASS, INSTANCE DATA STEP RATE, 
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},		//POSITION0
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
        };

        UINT iSizeLayout = ARRAYSIZE(layout);
        if (FAILED(pd3dDevice->CreateInputLayout(layout, iSizeLayout,
            pVertexShader->m_pVSCode->GetBufferPointer(), 
            pVertexShader->m_pVSCode->GetBufferSize(), 
            &pMapMesh->m_pVertexInputLayout)))
            throw std::exception("InputLayout not create successfully");

		pMapMesh->m_pIndexBuffer =
			DX::CreateIndexBuffer(pd3dDevice, &pMapMesh->GetListIndex()[0], sizeof(DWORD), pMapMesh->GetListIndex().size());

		FQuadTree* pQuadTree = new FQuadTree(pMapMesh, pd3dDevice, pContext, iMaxDepth, fAlphaData);
		pQuadTree->SetConstantData(cc);
		pQuadTree->SetTransform({ mapTransform.position, mapTransform.rotation, mapTransform.scale });
		pQuadTree->SetTexture(pTexture);
		for (const auto& texture : pTexList)
			pQuadTree->SetSplattingTexture(texture);
		pQuadTree->SetShader(szVSPath, pVertexShader, szPSPath, pPixelShader);
		//pQuadTree->SetDrawMode(DRAW_MODE::MODE_SOLID);

		//for (const auto& obj : allObjectList)
		//{
		//    if (obj->GetSpecify() != OBJECT_SPECIFY::OBJECT_SIMPLE)
		//        m_ListFbx.insert(obj->GetObjectName());
		//    pQuadTree->AddObject(obj);
		//}

		return pQuadTree;
	}

    void	PathChanger(std::string& str)
    {
        std::string::size_type pos = str.find("D:\\sylProject\\Project41\\");
        std::string::size_type end = str.find("Asset");
        str.replace(pos, end - 1, "../../data");

        pos = str.find('\\');
        while (pos != std::string::npos) 
        {
            str.replace(pos, 1, 1, '/');
            pos = str.find('\\');
        }
        //"D:\\sylProject\\Project41\\Assets\\Textures\\sand.jpg"

        //str.
    }
};
