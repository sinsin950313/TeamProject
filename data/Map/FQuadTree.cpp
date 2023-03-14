#include "FQuadTree.h"
#include "ToolSystemMap.h"
#include "InputSystem.h"
#include "WindowSystem.h"

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
    if (FAILED(hr = g_pDevice->CreateTexture2D(&desc, &initData, &m_pMaskAlphaTexture)))
    {
        return hr;
    }
    if (FAILED(hr = g_pDevice->CreateShaderResourceView(m_pMaskAlphaTexture, NULL, &m_pMaskAlphaSrv)))
    {
        return hr;
    }

    return hr;
}

void FQuadTree::Splatting(XMVECTOR vIntersection, std::wstring szFullPath)
{
    UINT const DataSize = sizeof(BYTE) * 4;
    UINT const RowPitch = DataSize * m_pMap->m_dwNumRows;
    UINT const DepthPitch = 0;

    // pick data    ->  texture data
    // 0 ~ 64      ->   0 ~ 1
    // - 32 ~ +32  ->   0 ~ 1024 -> 0 ~ 1
    float fCellDistance = m_pMap->m_fCellDistance;
    XMFLOAT2 vTexIndex;
    XMFLOAT2 vUV;
    XMFLOAT2 vMaxSize = { (float)(m_pMap->m_dwNumRows / 2), (float)(m_pMap->m_dwNumColumns / 2) };
    XMFLOAT3 vTexPos;
    XMFLOAT3 vPickPos;
    XMStoreFloat3(&vPickPos, vIntersection);

    for (UINT y = 0; y < m_pMap->m_dwNumColumns; y++)
    {
        vTexIndex.y = y;
        for (UINT x = 0; x < m_pMap->m_dwNumRows; x++)
        {
            vTexIndex.x = x;
            // tex uv 0 ~ 1 to -1 ~ 1
            vUV = XMFLOAT2((vTexIndex.x / (float)m_pMap->m_dwNumRows) * 2.0f - 1.0f,
                -(vTexIndex.y / (float)m_pMap->m_dwNumColumns * 2.0f - 1.0f));
            // 맵사이즈크기만큼
            vTexPos = XMFLOAT3(vUV.x * vMaxSize.x * fCellDistance, m_pMap->m_ListVertex[m_pMap->m_dwNumRows * y + x].pos.y, vUV.y * vMaxSize.y * fCellDistance);  //x and z
            BYTE* pixel = &m_fAlphaData[m_pMap->m_dwNumRows * y * 4 + x * 4];
            XMFLOAT3 radius =  vPickPos - vTexPos;
            float fRadius = XMVectorGetX(XMVector3Length(XMLoadFloat3(&radius)));

            if (fRadius < m_fSplattingRadius)
            {
                float fDot = 1.0f - (fRadius / m_fSplattingRadius); //지점부터 범위까지, splattingRadius가 기준, 멀어질수록 fdot의값이 작아져 연해진다
                for(int idx = 0; idx < m_ListTextureSplatting.size(); idx++)
                    if (szFullPath == m_ListTextureSplatting[idx]->m_szFullPath && (fDot * 255) > pixel[idx])
                        pixel[idx] = fDot * 255;//rgba -> this size rimited under 4
            }
        }
    }
    g_pDeviceContext->UpdateSubresource(m_pMaskAlphaTexture, 0, nullptr, m_fAlphaData, RowPitch, DepthPitch);
}

#include "SimpleBox.h"
std::ostream& operator<<(std::ostream& os, const FQuadTree* pQuadTree)
{
    os << "m_pTexture:" << pQuadTree->m_pTexture->GetTextureName() << std::endl;

    os << "m_ListTextureSplatting:";
    for (const auto& texture : pQuadTree->m_ListTextureSplatting)
    {
        os << texture->GetTextureName() << ", ";
    }
    os << std::endl;

    os << "m_Transform:" << pQuadTree->m_Transform << std::endl;

    os << "m_iMaxDepth:" << pQuadTree->m_iMaxDepth << std::endl;

    os << "m_szVSName:" << pQuadTree->m_szVSPath << std::endl;

    os << "m_szPSName:" << pQuadTree->m_szPSPath << std::endl;

    os << "m_pMap:" << pQuadTree->m_pMap;

    os << "m_pAllObjectList:" << std::endl;
    for (const auto& object : pQuadTree->m_pAllObjectList)
    {
        if (object->GetSpecify() == OBJECT_SPECIFY::OBJECT_SIMPLE)
            os << (SimpleBox*)object << std::endl;
        else
            os << object << std::endl; 
    }

    os << "m_fAlphaData:";
    for (int idx = 0; idx < pQuadTree->m_pMap->m_dwNumRows * pQuadTree->m_pMap->m_dwNumColumns * 4; idx++)
        os << std::stoi(std::to_string(pQuadTree->m_fAlphaData[idx])) << " ";
    os << std::endl;
    return os;
}
//std::ifstream& operator>>(std::ifstream& is, FQuadTree* pQuadTree)
//{
//    Texture* pTexture = nullptr;
//    Transform mapTransform = {};
//    UINT iMaxDepth = 0;
//    std::wstring szVSPath;
//    std::wstring szPSPath;
//    void* shader_byte_code_vs = nullptr;
//    void* shader_byte_code_ps = nullptr;
//    size_t size_shader_vs = 0;
//    size_t size_shader_ps = 0;
//    MeshMap* pMapMesh = new MeshMap();
//    std::unordered_map<std::string, Transform> allObjectList;
//    BYTE* fAlphaData = nullptr;
//    std::string line;
//    while (std::getline(is, line))
//    {
//        std::istringstream iss(line);
//        std::string fieldName;
//        if (std::getline(iss, fieldName, ':'))
//        {
//            if (fieldName == "m_pTexture")
//            {
//                std::string textureName;
//                iss >> textureName;
//                pTexture = _EngineSystem.GetTextureSystem()->CreateTextureFromFile(_tomw(textureName));
//                _ToolSystemMap.GetListTexture().insert(_tomw(textureName));
//            }
//            else if (fieldName == "m_ListTextureSplatting")
//            {
//                std::string texturesStr;
//                std::getline(iss, texturesStr);
//                std::stringstream texturesStream(texturesStr);
//                std::string texturePath;
//                while (std::getline(texturesStream, texturePath, ','))
//                {
//                    if (texturePath.size() > 1)
//                    {
//                        texturePath.erase(std::remove(texturePath.begin(), texturePath.end(), ' '), texturePath.end());
//                        auto texture = _EngineSystem.GetTextureSystem()->CreateTextureFromFile(_tomw(texturePath));
//                        pQuadTree->m_ListTextureSplatting.push_back(texture);
//                    }
//                }
//            }
//            else if (fieldName == "m_Transform")
//            {
//                iss >> mapTransform;
//            }
//            else if (fieldName == "m_iMaxDepth")
//            {
//                iss >> iMaxDepth;
//            }
//            else if (fieldName == "m_szVSName")
//            {
//                std::string str;
//                std::getline(iss, str);
//                szVSPath = _tomw(str);
//            }
//            else if (fieldName == "m_szPSName")
//            {
//                std::string str;
//                std::getline(iss, str);
//                szPSPath = _tomw(str);
//            }
//            else if (fieldName == "m_pMap")
//            {
//                is >> pMapMesh;
//            }
//            else if (fieldName == "m_pAllObjectList")
//            {
//                std::streampos prevPos = is.tellg();
//                std::string str;
//                while (std::getline(is, str))
//                {
//                    if (str.find("m_fAlphaData:") != std::string::npos)
//                        break;
//                    std::stringstream texturesStream(str);
//                    std::string str;
//                    std::getline(texturesStream, str, ',');
//
//                    CULL_MODE cullMode;
//                    texturesStream >> cullMode;
//
//                    DRAW_MODE drawMode;
//                    texturesStream >> drawMode;
//
//                    INTERACTIVE_MODE interactiveMode;
//                    texturesStream >> interactiveMode;
//
//                    OBJECT_SPECIFY specifyMode;
//                    texturesStream >> specifyMode;
//
//                    Transform transform;
//                    texturesStream >> transform;
//                    allObjectList.insert(std::make_pair(str, transform));
//                    prevPos = is.tellg();
//                }
//                is.seekg(prevPos);
//            }
//            else if (fieldName == "m_fAlphaData")
//            {
//                fAlphaData = new BYTE[pMapMesh->m_dwNumRows * pMapMesh->m_dwNumColumns * 4];
//                for (int idx = 0; idx < pMapMesh->m_dwNumRows * pMapMesh->m_dwNumColumns * 4; idx++)
//                {
//                    int rgba = 0;
//                    iss >> rgba;
//                    fAlphaData[idx] = static_cast<uint8_t>(rgba);
//                }
//            }
//        }
//    }
//
//    constant_map cc;
//    cc.matWorld = XMMatrixIdentity();
//    cc.matView = pQuadTree->m_pCamera->m_matCamera;
//    cc.matProj = pQuadTree->m_pCamera->m_matProj;
//
//    _EngineSystem.GetMeshSystem()->AddResource(L"MapMesh", pMapMesh);
//
//    _EngineSystem.GetRenderSystem()->CompileVertexShader(szVSPath.c_str(), "vsmain", "vs_5_0", &shader_byte_code_vs, &size_shader_vs);
//    VertexShader* pVertexShader = _EngineSystem.GetRenderSystem()->CreateVertexShader(shader_byte_code_vs, size_shader_vs);
//    _EngineSystem.GetRenderSystem()->CompilePixelShader(szPSPath.c_str(), "psmain", "ps_5_0", &shader_byte_code_ps, &size_shader_ps);
//    PixelShader* pPixelShader = _EngineSystem.GetRenderSystem()->CreatePixelShader(shader_byte_code_ps, size_shader_ps);
//
//    VertexBuffer* pVertexBuffer = _EngineSystem.GetRenderSystem()->CreateVertexBuffer(&pMapMesh->GetListVertex()[0], sizeof(object), pMapMesh->GetListVertex().size(), shader_byte_code_vs, size_shader_vs);
//    IndexBuffer* pIndexBuffer = _EngineSystem.GetRenderSystem()->CreateIndexBuffer(&pMapMesh->GetListIndex()[0], pMapMesh->GetListIndex().size());
//
//    _EngineSystem.GetRenderSystem()->ReleaseBlob();
//
//    pMapMesh->m_pVertexBuffer = pVertexBuffer;
//    pMapMesh->m_pIndexBuffer = pIndexBuffer;
//
//    pQuadTree->SetConstantData(cc);
//    pQuadTree->SetTransform({ mapTransform.position, mapTransform.rotation, mapTransform.scale });
//    pQuadTree->SetTexture(pTexture);
//    pQuadTree->SetShader(szVSPath, pVertexShader, szPSPath, pPixelShader);
//    pQuadTree->SetDrawMode(DRAW_MODE::MODE_SOLID);
//
//    for (const auto& obj : allObjectList)
//    {
//        std::string szFullPath = obj.first;
//        _ToolSystemMap.GetListFbx().insert(_tomw(szFullPath));
//        Transform transform = obj.second;
//        _ToolSystemMap.CreateFbxObject(_tomw(szFullPath), transform.position, transform.rotation, transform.scale);
//    }
//
//    return is;
//}


FQuadTree::FQuadTree(Camera* pCamera, MeshMap* pMap, int iMaxDepth, BYTE* fAlphaData)
{
    m_pCamera = pCamera;
    m_pMap = pMap;
    m_iMaxDepth = iMaxDepth;
    m_pRootNode = new FNode(nullptr, pMap, 0, m_pMap->m_dwNumColumns - 1, m_pMap->m_dwNumRows * (m_pMap->m_dwNumColumns - 1), m_pMap->m_dwNumRows * m_pMap->m_dwNumColumns - 1);

    m_constantDataMap.matWorld = XMMatrixIdentity();
    m_constantDataMap.matView = XMMatrixIdentity();
    m_constantDataMap.matProj = XMMatrixIdentity();
    m_pConstantBuffer = _EngineSystem.GetRenderSystem()->CreateConstantBuffer(&m_constantDataMap, sizeof(m_constantDataMap));

    CreateAlphaTexture(m_pMap->m_dwNumRows, m_pMap->m_dwNumColumns, fAlphaData);
    BuildTree(m_pRootNode, pMap);
}

FQuadTree::~FQuadTree()
{
    if (m_fAlphaData) delete m_fAlphaData;
    if (m_pMaskAlphaTexture) m_pMaskAlphaTexture->Release();
    if (m_pMaskAlphaSrv) m_pMaskAlphaSrv->Release();
    if (m_pRootNode) delete m_pRootNode;
    if (m_pConstantBuffer) delete m_pConstantBuffer;
    if (m_pVertexShader) delete m_pVertexShader;
    if (m_pPixelShader) delete m_pPixelShader;
}


void FQuadTree::UpdateNode(FNode* pNode)
{
    pNode->CreateIndexData(m_pMap);
    if (pNode->m_bLeaf)
        return;
    for (int iChild = 0; iChild < 4; iChild++)
        UpdateNode(pNode->m_pChild[iChild]);
}

Object* FQuadTree::GetPickingObject()
{
    return ObjectPicking();
}

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

void FQuadTree::SetPickingSimple(bool bPicking, float fLength)
{
    m_bSimplePicking = bPicking;
    m_fObjLength = fLength;
}

void FQuadTree::SetPickingFbx(std::wstring szCurrentFbx, bool bPicking)
{
    m_szCurrentFbx = szCurrentFbx;
    m_bFbxPicking = bPicking;
}

void FQuadTree::SetPickingObject(bool bPicking)
{
    m_bObjectPicking = bPicking;
}

void FQuadTree::SetPickingSculpt(bool bPicking)
{
    m_bSclupting = bPicking;
}

void FQuadTree::SetSculptRadius(float fRadius)
{
    m_fSculptRadius = fRadius;
}

void FQuadTree::SetSculptIntensity(float fIntensity)
{
    m_fSculptIntensity = fIntensity;
}

void FQuadTree::SetSplatRadius(float fRadius)
{
    m_fSplattingRadius = fRadius;
}

void FQuadTree::SetSplattingTexture(Texture* pTexture)
{
    m_ListTextureSplatting.push_back(pTexture);
}

void FQuadTree::SetSplatting(std::wstring szCurrentSplat, bool bSplatting)
{
    m_szCurrentSplat = szCurrentSplat;
    m_bSplatting = bSplatting;
}

void FQuadTree::SetTransform(Transform transform)
{
    m_Transform = transform;

    XMVECTOR scale = m_Transform.scale;
    XMFLOAT3 rotateAngle;
    XMStoreFloat3(&rotateAngle, m_Transform.rotation);
    XMVECTOR rotation =
        XMQuaternionRotationRollPitchYaw(
            _DegreeToRadian(rotateAngle.x),
            _DegreeToRadian(rotateAngle.y),
            _DegreeToRadian(rotateAngle.z));
    XMVECTOR translation = m_Transform.position;
    m_constantDataMap.matWorld = XMMatrixTransformation({ 0,0,0,1 }, { 0,0,0,1 }, scale, { 0,0,0,1 }, rotation, translation);
}


void FQuadTree::SetMaterial(Material* pMaterial)
{
    m_pMaterial = pMaterial;
}

void FQuadTree::SetTexture(Texture* pTexture)
{
    m_pTexture = pTexture;
}

void FQuadTree::SetShader(std::wstring vsPath, VertexShader* pVertexShader, std::wstring psPath, PixelShader* pPixelShader)
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

void FQuadTree::SetDrawMode(DRAW_MODE mode)
{
    m_DrawMode = mode;
}

void FQuadTree::SetSpecify(OBJECT_SPECIFY specify)
{
    m_Specify = specify;
}

BOOL FQuadTree::AddObject(Object* pObj)
{
    m_pAllObjectList.insert(pObj);

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
    return m_pAllObjectList.erase(pObj);
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
            if (TCollision::BoxToBox(pNode->m_pChild[i]->m_Box, pObj->m_Box))
            {
                pNode = FindNode(pNode->m_pChild[i], pObj);
                break;
            }
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
    F_POSITION dwRet = m_pCamera->m_Frustum.ClassifyBox(pNode->m_Box);
    if (F_FRONT == dwRet)// 완전포함.
    {
        m_pDrawLeafNodeList.push_back(pNode);
        return pNode;
    }
    if (F_SPANNING == dwRet) // 걸쳐있다.
    {
        if (pNode->m_bLeaf)
        {
            m_pDrawLeafNodeList.push_back(pNode);
        }
        else
        {
            for (int iNode = 0; iNode < 4; iNode++)
            {
                VisibleNode(pNode->m_pChild[iNode]);
            }
        }
    }
    return pNode;
}

bool FQuadTree::GetInterSection()
{
    std::map<float, XMVECTOR> chkDist;
    for (const auto& node : m_pDrawLeafNodeList)
    {
        UINT index = 0;
        UINT iNumFace = node->m_IndexList.size() / 3;
        for (UINT face = 0; face < iNumFace; face++)
        {
            UINT i0 = node->m_IndexList[index + 0];
            UINT i1 = node->m_IndexList[index + 1];
            UINT i2 = node->m_IndexList[index + 2];
            XMFLOAT3 v0 = m_pMap->GetListVertex()[i0].pos;
            XMFLOAT3 v1 = m_pMap->GetListVertex()[i1].pos;
            XMFLOAT3 v2 = m_pMap->GetListVertex()[i2].pos;
            float fDist;
            if (m_Select.ChkPick(XMLoadFloat3(&v0), XMLoadFloat3(&v1), XMLoadFloat3(&v2), fDist))
            //if(DirectX::TriangleTests::Intersects(m_Select.m_Ray.vOrigin, m_Select.m_Ray.vDirection, XMLoadFloat3(&v0), XMLoadFloat3(&v1), XMLoadFloat3(&v2), fDist))
            {
                //m_Select.m_vIntersection = m_Select.m_Ray.vOrigin + m_Select.m_Ray.vDirection * fDist;
                chkDist.insert(std::make_pair(fDist, m_Select.m_vIntersection));
                /*return true;*/
            }
            index += 3;
        }
    }
    if(!chkDist.size())
        return false;

    float maxFloat = 99999999.9f;
    for (const auto& vec : chkDist)
    {
        if (vec.first < maxFloat)
        {
            maxFloat = vec.first;
            m_Select.m_vIntersection = vec.second;
        }
    }
    return true;
}

Object* FQuadTree::ObjectPicking()
{
    //교점체크
    for (const auto& node : m_pDrawLeafNodeList)
    {
        for (const auto& object : node->m_pDynamicObjectList)
        {
            for (const auto& meshnode : object->m_pMesh->GetMeshNodeList())
            {
                UINT index = 0;
                UINT iNumFace = meshnode->GetListIndex().size() / 3;
                for (UINT face = 0; face < iNumFace; face++)
                {
                    UINT i0 = meshnode->GetListIndex()[index + 0];
                    UINT i1 = meshnode->GetListIndex()[index + 1];
                    UINT i2 = meshnode->GetListIndex()[index + 2];
                    XMFLOAT3 v0 = meshnode->GetListVertex()[i0].pos;
                    XMFLOAT3 v1 = meshnode->GetListVertex()[i1].pos;
                    XMFLOAT3 v2 = meshnode->GetListVertex()[i2].pos;
                    XMVECTOR v_0 = XMVector3TransformCoord(XMLoadFloat3(&v0), object->m_ConstantData.matWorld);
                    XMVECTOR v_1 = XMVector3TransformCoord(XMLoadFloat3(&v1), object->m_ConstantData.matWorld);
                    XMVECTOR v_2 = XMVector3TransformCoord(XMLoadFloat3(&v2), object->m_ConstantData.matWorld);
                    float fDist;
                    if (m_Select.ChkPick(v_0, v_1, v_2, fDist))
                    {
                        return object;
                    }
                    index += 3;
                }
            }
        }
    }
    return nullptr;
}

void FQuadTree::Update()
{
    m_constantDataMap.matView = _CameraSystem.GetCurrentCamera()->m_matCamera;
    m_constantDataMap.m_camera_position = XMFLOAT4(
        XMVectorGetX(_CameraSystem.GetCurrentCamera()->m_matWorld.r[3]),
        XMVectorGetY(_CameraSystem.GetCurrentCamera()->m_matWorld.r[3]),
        XMVectorGetZ(_CameraSystem.GetCurrentCamera()->m_matWorld.r[3]),
        XMVectorGetW(_CameraSystem.GetCurrentCamera()->m_matWorld.r[3]));
    m_constantDataMap.m_world_size = XMFLOAT2(m_pMap->m_dwNumColumns, m_pMap->m_dwNumRows);
    m_constantDataMap.m_cell_distance = m_pMap->m_fCellDistance;
    _EngineSystem.GetRenderSystem()->UpdateConstantBuffer(m_pConstantBuffer, &m_constantDataMap);

    m_pDrawLeafNodeList.clear();
    VisibleNode(m_pRootNode); //재귀로 VisibleNode체크
    m_Select.SetMatrix(nullptr, &m_pCamera->m_matCamera, &m_pCamera->m_matProj);

    //MapCreateObject simple
    if (m_bSimplePicking && (_InputSystem.GetKey(VK_RBUTTON) == KEY_STATE::KEY_DOWN) && GetInterSection()) 
    {
        _ToolSystemMap.CreateSimpleBox(m_fObjLength, m_Select.m_vIntersection);
    }

    //MapCreateObject fbx
    if (m_bFbxPicking && (_InputSystem.GetKey(VK_RBUTTON) == KEY_STATE::KEY_DOWN) && GetInterSection())
    {
        _ToolSystemMap.CreateFbxObject(m_szCurrentFbx, m_Select.m_vIntersection);
    }

    //ObjectPicking
    /*if (m_bObjectPicking &&(_InputSystem.GetKey(VK_RBUTTON) == KEY_STATE::KEY_DOWN))
    {
        pPickingObj = ObjectPicking();
    }*/

    //Sculpting
    if (m_bSclupting && _InputSystem.GetKey(VK_RBUTTON) && GetInterSection())
    {
        std::vector<FNode*> nodelist;
        T_BOX box;
        XMFLOAT3 vMin, vMax;
        XMFLOAT3 vRange(10, 10, 10);
        XMStoreFloat3(&vMax, m_Select.m_vIntersection + XMLoadFloat3(&vRange));
        XMStoreFloat3(&vMin, m_Select.m_vIntersection - XMLoadFloat3(&vRange));
        box.Set(vMax, vMin);
        if (SelectVertexList(box, nodelist) > 0)
        {
            int iVertex = nodelist[0]->m_IndexList[0];
            int iVertexSize = nodelist[nodelist.size() - 1]->m_IndexList[nodelist[nodelist.size() - 1]->m_IndexList.size() - 1];
            for (iVertex; iVertex < iVertexSize; iVertex++)
            {
                XMFLOAT3 v0 = m_pMap->GetListVertex()[iVertex].pos;
                XMVECTOR v = XMLoadFloat3(&v0) - m_Select.m_vIntersection;
                float fDistance = XMVectorGetX(XMVector3Length(v));
                if (fDistance <= m_fSculptRadius)
                {
                    float fValue = (fDistance / m_fSculptRadius) * 90.0f;
                    float fdot = cosf(_DegreeToRadian(fValue));
                    m_pMap->GetListVertex()[iVertex].pos.y += fdot * m_fSculptIntensity;
                }
            }
           
            for (const auto& node : nodelist)
            {
                for (const auto& object : node->m_pDynamicObjectList)
                {
                    XMFLOAT3 pos;
                    XMStoreFloat3(&pos, object->GetPosition());
                    object->SetTransform({ {pos.x, m_pMap->GetHeight(pos.x, pos.z), pos.z}, object->GetRotation(), object->GetScale() });
                }
            }

            UpdateNode(m_pRootNode);
            m_pMap->GenerateVertexNormal();
            _EngineSystem.GetRenderSystem()->UpdateVertexBuffer(m_pMap->m_pVertexBuffer, &m_pMap->GetListVertex()[0]);
        }
    }

    //Splating
    if (m_bSplatting && _InputSystem.GetKey(VK_RBUTTON) && GetInterSection())
    {
        Splatting(m_Select.m_vIntersection, m_szCurrentSplat);
    }
}

void FQuadTree::Render()
{
    _EngineSystem.GetRenderSystem()->SetWireFrame(m_DrawMode);
    _EngineSystem.GetRenderSystem()->SetConstantBuffer(m_pVertexShader, m_pConstantBuffer);
    _EngineSystem.GetRenderSystem()->SetConstantBuffer(m_pPixelShader, m_pConstantBuffer);
    _EngineSystem.GetRenderSystem()->SetVertexShader(m_pVertexShader);
    _EngineSystem.GetRenderSystem()->SetPixelShader(m_pPixelShader);
    _EngineSystem.GetRenderSystem()->SetVertexBuffer(m_pMap->m_pVertexBuffer);
    _EngineSystem.GetRenderSystem()->setTexture(m_pVertexShader, m_pTexture);
    _EngineSystem.GetRenderSystem()->setTexture(m_pPixelShader, m_pTexture);
    g_pDeviceContext->PSSetShaderResources(1, 1, &m_pMaskAlphaSrv);
    for (int idx = 0; idx < m_ListTextureSplatting.size(); idx++)
        g_pDeviceContext->PSSetShaderResources(2 + idx, 1, &m_ListTextureSplatting[idx]->m_pShaderResourceView);
    
    for (int idx = 0;  idx < m_pDrawLeafNodeList.size(); idx++)
    {
        g_pDeviceContext->IASetIndexBuffer(m_pDrawLeafNodeList[idx]->m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
        _EngineSystem.GetRenderSystem()->drawIndexedTriangleList(m_pDrawLeafNodeList[idx]->m_dwFace * 3, 0, 0);
    }
}
