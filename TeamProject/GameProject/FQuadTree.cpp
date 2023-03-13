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
FQuadTree::FQuadTree(Camera* pCamera, MeshMap* pMap, int iMaxDepth, BYTE* fAlphaData)
{
    m_pCamera = pCamera;
    m_pMap = pMap;
    m_iMaxDepth = iMaxDepth;
    m_pRootNode = new FNode(nullptr, pMap, 0, m_pMap->m_dwNumColumns - 1, m_pMap->m_dwNumRows * (m_pMap->m_dwNumColumns - 1), m_pMap->m_dwNumRows * m_pMap->m_dwNumColumns - 1);

    m_constantDataMap.matWorld = XMMatrixIdentity();
    m_constantDataMap.matView = XMMatrixIdentity();
    m_constantDataMap.matProj = XMMatrixIdentity();
    m_pConstantBuffer = DX::CreateConstantBuffer(m_pd3dDevice, &m_constantDataMap, sizeof(m_constantDataMap));

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
    //_EngineSystem.GetRenderSystem()->UpdateConstantBuffer(m_pConstantBuffer, &m_constantDataMap);
    m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, NULL, NULL, &m_constantDataMap, NULL, NULL);

    m_pDrawLeafNodeList.clear();
    VisibleNode(m_pRootNode); //재귀로 VisibleNode체크

    //m_Select.SetMatrix(nullptr, &m_pCamera->m_matCamera, &m_pCamera->m_matProj);
}

void	FQuadTree::PreRender()
{
    m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
    m_pImmediateContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);
    m_pImmediateContext->VSSetShader(m_pVertexShader->m_pVS, NULL, 0);
    m_pImmediateContext->PSSetShader(m_pVertexShader->m_pPS, NULL, 0);

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
    for (int idx = 0;  idx < m_pDrawLeafNodeList.size(); idx++)
    {
        m_pImmediateContext->IASetIndexBuffer(m_pDrawLeafNodeList[idx]->m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

        m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);		//TrangleList를 Index로그린다
        m_pImmediateContext->DrawIndexed(m_pDrawLeafNodeList[idx]->m_dwFace * 3, 0, 0);
    }
}
