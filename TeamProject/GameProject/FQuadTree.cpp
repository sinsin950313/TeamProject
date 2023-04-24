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

    m_ConstantData_Transform.matWorld = XMMatrixIdentity();
    m_ConstantData_Transform.matView = XMMatrixIdentity();
    m_ConstantData_Transform.matProj = XMMatrixIdentity();
    m_pConstantBuffer_Transform = DX::CreateConstantBuffer(pDevice, &m_ConstantData_Transform, sizeof(m_ConstantData_Transform));

    m_ConstantData_Map.worldSize = XMFLOAT2(m_pMap->m_dwNumColumns, m_pMap->m_dwNumRows);
    m_ConstantData_Map.cellDistance = m_pMap->m_fCellDistance;
    m_ConstantData_Map.tileCount = m_pMap->m_iTileCount;
    m_pConstantBuffer_Map = DX::CreateConstantBuffer(pDevice, &m_ConstantData_Map, sizeof(m_ConstantData_Map));

    m_pConstantBuffer_Light = DX::CreateConstantBuffer(pDevice, &m_ConstantData_Light, sizeof(m_ConstantData_Light));

    /*m_ConstantData_Fog.linearFogStart = 50.0f;
    m_ConstantData_Fog.linearFogEnd = 300.0f;
    m_ConstantData_Fog.expFogDensity = 0.0001f;
    m_pConstantBuffer_Fog = DX::CreateConstantBuffer(pDevice, &m_pConstantBuffer_Fog, sizeof(m_ConstantData_Fog));*/

  /*  m_constantDataMap.matWorld = XMMatrixIdentity();
    m_constantDataMap.matView = XMMatrixIdentity();
    m_constantDataMap.matProj = XMMatrixIdentity();
    m_pConstantBuffer = DX::CreateConstantBuffer(pDevice, &m_constantDataMap, sizeof(m_constantDataMap));*/

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
    m_ConstantData_Transform.matWorld = XMMatrixTransformation({ 0,0,0,1 }, { 0,0,0,1 }, scale, { 0,0,0,1 }, rotation, translation);
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

void FQuadTree::SetConstantData(ConstantData_Transform constantData)
{
    m_ConstantData_Transform = constantData;
}


void FQuadTree::SetConstantData(ConstantData_Map cc)
{
    m_ConstantData_Map = cc;
}

void FQuadTree::SetConstantData(ConstantData_Light constantData)
{
    //m_ConstantData_Light = constantData;
}


BOOL FQuadTree::AddObject(Object* pObj)
{
    //return FALSE;
    m_pAllObjectList.insert(pObj);

    FNode* pFindNode = FindCollisionNode(m_pRootNode, pObj);
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

FNode* FQuadTree::FindCollisionNode(FNode* pNode, Object* pObj)
{
    for (int i = 0; i < 4; i++)
    {
        if (pNode->m_pChild[i] != nullptr)
        {
            if (TCollision::TCollision::ChkOBBToOBB(pNode->m_pChild[i]->m_Box, pObj->m_ColliderBox))
            {
                pNode = FindCollisionNode(pNode->m_pChild[i], pObj);
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
    PLANE_COLTYPE dwRet = m_pCurrentCamera->m_vFrustum.ClassifyOBB(pNode->m_Box);
    if (P_FRONT == dwRet)// 완전포함.
    {
        m_pDrawLeafNodeList.push_back(pNode);
        return pNode;
    }
    if (1) // 걸쳐있다.
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

void	FQuadTree::SetMatrix(TMatrix* matWorld, TMatrix* matView, TMatrix* matProj)
{
    //m_constantDataMap.matWorld;
    if (matWorld)
    {
        TMatrix world = *matWorld;
        m_ConstantData_Transform.matWorld = XMLoadFloat4x4(&world);
        //m_constantDataMap.matWorld = XMMatrixTranspose(world);
    }
    if (matView)
    {
        TMatrix view = *matView;
        m_ConstantData_Transform.matView = XMLoadFloat4x4(&view);
        //m_constantDataMap.matView = XMMatrixTranspose(view);
    }
    if (matProj)
    {
        TMatrix proj = *matProj;
        m_ConstantData_Transform.matProj = XMLoadFloat4x4(&proj);
        //m_constantDataMap.matProj = XMMatrixTranspose(proj);
    }
    m_pImmediateContext->UpdateSubresource(m_pConstantBuffer_Transform, NULL, NULL, &m_ConstantData_Transform, NULL, NULL);

    for (const auto& object : m_pAllObjectList)
    {   
        object->SetMatrix(matWorld, matView, matProj);
    }

    m_pSphereObject->SetMatrix(matWorld, matView, matProj);
}

void FQuadTree::Update()
{
    m_pSphereObject->Frame();

    m_pDrawLeafNodeList.clear();

    m_ConstantData_Light.cameraPosition = XMFLOAT4(
        XMVectorGetX(m_pCurrentCamera->m_matWorld.Forward()),
        XMVectorGetY(m_pCurrentCamera->m_matWorld.Forward()),
        XMVectorGetZ(m_pCurrentCamera->m_matWorld.Forward()),
        XMVectorGetW(m_pCurrentCamera->m_matWorld.Forward()));

    /*m_ConstantData_Fog.cameraPosition = m_pCurrentCamera->m_vPos;*/

    m_pImmediateContext->UpdateSubresource(m_pConstantBuffer_Map, NULL, NULL, &m_ConstantData_Map, NULL, NULL);
    m_pImmediateContext->UpdateSubresource(m_pConstantBuffer_Light, NULL, NULL, &m_ConstantData_Light, NULL, NULL);
    //m_pImmediateContext->UpdateSubresource(m_pConstantBuffer_Fog, NULL, NULL, &m_ConstantData_Fog, NULL, NULL);

    VisibleNode(m_pRootNode); //재귀로 VisibleNode체크

    for (const auto& object : m_pAllObjectList)
        object->Frame();
}

void	FQuadTree::PreRender()
{
    m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer_Transform);
    m_pImmediateContext->VSSetConstantBuffers(1, 1, &m_pConstantBuffer_Map);
    m_pImmediateContext->VSSetConstantBuffers(2, 1, &m_pConstantBuffer_Light);
    //m_pImmediateContext->VSSetConstantBuffers(3, 1, &m_pConstantBuffer_Fog);


    m_pImmediateContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer_Transform);
    m_pImmediateContext->PSSetConstantBuffers(1, 1, &m_pConstantBuffer_Map);
    m_pImmediateContext->PSSetConstantBuffers(2, 1, &m_pConstantBuffer_Light);


    m_pImmediateContext->VSSetShader(m_pVertexShader->m_pVS, NULL, 0);

    UINT stride = sizeof(PNCTVertex); //정점의크기
    UINT offset = 0;          //정점의오프셋
    m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pMap->m_pVertexBuffer, &stride, &offset);	// VertexBuffer를 세팅, 1은 버퍼의갯수
    m_pImmediateContext->IASetInputLayout(m_pMap->m_pVertexInputLayout);

    //for (int idx = 0; idx < m_ListTextureSplatting.size(); idx++)
    //    m_pImmediateContext->PSSetShaderResources(2 + idx, 1, &m_ListTextureSplatting[idx]->m_pTextureSRV);

    for (int idx = 0;  idx < m_pDrawLeafNodeList.size(); idx++)
    {
        m_pImmediateContext->IASetIndexBuffer(m_pDrawLeafNodeList[idx]->m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

        m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);		//TrangleList를 Index로그린다
        m_pImmediateContext->DrawIndexed(m_pDrawLeafNodeList[idx]->m_IndexList.size(), 0, 0);
    }

    for (const auto& object : m_pAllObjectList)
    {   
        object->PreRender();
    }

    //m_pSphereObject->SetMatrix(nullptr, &view, &proj);
    m_pSphereObject->PreRender();
}

void FQuadTree::Render()
{
    m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer_Transform);
    m_pImmediateContext->VSSetConstantBuffers(1, 1, &m_pConstantBuffer_Map);
    m_pImmediateContext->VSSetConstantBuffers(2, 1, &m_pConstantBuffer_Light);


    m_pImmediateContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer_Transform);
    m_pImmediateContext->PSSetConstantBuffers(1, 1, &m_pConstantBuffer_Map);
    m_pImmediateContext->PSSetConstantBuffers(2, 1, &m_pConstantBuffer_Light);


    m_pImmediateContext->VSSetShader(m_pVertexShader->m_pVS, NULL, 0);
    m_pImmediateContext->PSSetShader(m_pPixelShader->m_pPS, NULL, 0);

    UINT stride = sizeof(PNCTVertex); //정점의크기
    UINT offset = 0;          //정점의오프셋
    m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pMap->m_pVertexBuffer, &stride, &offset);	// VertexBuffer를 세팅, 1은 버퍼의갯수
    m_pImmediateContext->IASetInputLayout(m_pMap->m_pVertexInputLayout);

    m_pImmediateContext->VSSetShaderResources(0, 1, &m_pTexture->m_pTextureSRV);
    m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTexture->m_pTextureSRV);
    m_pImmediateContext->PSSetShaderResources(1, 1, &m_pMaskAlphaSrv);

    for (int idx = 0; idx < m_ListTextureSplatting.size(); idx++)
        m_pImmediateContext->PSSetShaderResources(2 + idx, 1, &m_ListTextureSplatting[idx]->m_pTextureSRV);

    for (int idx = 0;  idx < m_pDrawLeafNodeList.size(); idx++)
    {
        m_pImmediateContext->IASetIndexBuffer(m_pDrawLeafNodeList[idx]->m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

        m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);		//TrangleList를 Index로그린다
        m_pImmediateContext->DrawIndexed(m_pDrawLeafNodeList[idx]->m_IndexList.size(), 0, 0);
    }

    for (const auto& object : m_pAllObjectList)
    {   
        object->Render();
    }

    //m_pSphereObject->SetMatrix(nullptr, &view, &proj);
    m_pSphereObject->Render();
}

void	FQuadTree::Release()
{
    if (m_pSphereObject)
    {
        m_pSphereObject->Release();
        delete m_pSphereObject;
        m_pSphereObject = nullptr;
    }

    for (auto obj : m_pAllObjectList)
    {
        obj->Release();
        delete obj;
    }
    m_pAllObjectList.clear();

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
    if (m_pConstantBuffer_Transform)
    {
        m_pConstantBuffer_Transform->Release();
        m_pConstantBuffer_Transform = nullptr;
    }
    if (m_pConstantBuffer_Map)
    {
        m_pConstantBuffer_Map->Release();
        m_pConstantBuffer_Map = nullptr;
    }
    if (m_pConstantBuffer_Light)
    {
        m_pConstantBuffer_Light->Release();
        m_pConstantBuffer_Light = nullptr;
    }
    if (m_pMap)
    {
        delete m_pMap;
        m_pMap = nullptr;
    }
}

namespace MAPLOAD
{
#define _DegreeToRadian(X) X*(XM_PI/180.0f)
    static XMFLOAT2 operator+(XMFLOAT2& a, XMFLOAT2& b)
    {
        return XMFLOAT2(a.x + b.x, a.y + b.y);
    }

    static XMFLOAT2 operator-(XMFLOAT2& a, XMFLOAT2& b)
    {
        return XMFLOAT2(a.x - b.x, a.y - b.y);
    }

    static XMFLOAT2 operator*(XMFLOAT2& a, float scala)
    {
        return XMFLOAT2(a.x * scala, a.y * scala);
    }

    static XMFLOAT2 operator/(XMFLOAT2& a, float scala)
    {
        return XMFLOAT2(a.x / scala, a.y / scala);
    }

    static XMFLOAT2 operator+=(XMFLOAT2& a, XMFLOAT2& b)
    {
        a.x += b.x;
        a.y += b.y;
        return a;
    }

    static XMFLOAT2 operator-=(XMFLOAT2& a, XMFLOAT2& b)
    {
        a.x -= b.x;
        a.y -= b.y;
        return a;
    }

    static XMFLOAT2 operator*=(XMFLOAT2& a, XMFLOAT2& b)
    {
        a.x *= b.x;
        a.y *= b.y;
        return a;
    }

    static XMFLOAT2 operator/=(XMFLOAT2& a, XMFLOAT2& b)
    {
        a.x /= b.x;
        a.y /= b.y;
        return a;
    }

    static bool operator==(XMFLOAT2& a, XMFLOAT2& b)
    {
        if ((fabs(a.x - b.x) <= _Epsilon) && (fabs(a.y - b.y) <= _Epsilon))
            return true;
        else
            return false;
    }

    static bool operator<=(XMFLOAT2& a, XMFLOAT2& b)
    {
        if (a.x <= b.x && a.y <= b.y)
            return true;
        else
            return false;
    }

    static bool operator>=(XMFLOAT2& a, XMFLOAT2& b)
    {
        if (a.x >= b.x && a.y >= b.y)
            return true;
        else
            return false;
    }



    static XMFLOAT3 operator+(XMFLOAT3& a, XMFLOAT3& b)
    {
        return XMFLOAT3(a.x + b.x, a.y + b.y, a.z + b.z);
    }

    static XMFLOAT3 operator-(XMFLOAT3& a, XMFLOAT3& b)
    {
        return XMFLOAT3(a.x - b.x, a.y - b.y, a.z - b.z);
    }

    static XMFLOAT3 operator*(XMFLOAT3& a, float scala)
    {
        return XMFLOAT3(a.x * scala, a.y * scala, a.z * scala);
    }

    static XMFLOAT3 operator/(XMFLOAT3& a, float scala)
    {
        return XMFLOAT3(a.x / scala, a.y / scala, a.z / scala);
    }

    static XMFLOAT3 operator+=(XMFLOAT3& a, XMFLOAT3& b)
    {
        a.x += b.x;
        a.y += b.y;
        a.z += b.z;
        return a;
    }

    static XMFLOAT3 operator-=(XMFLOAT3& a, XMFLOAT3& b)
    {
        a.x -= b.x;
        a.y -= b.y;
        a.z -= b.z;
        return a;
    }

    static XMFLOAT3 operator*=(XMFLOAT3& a, XMFLOAT3& b)
    {
        a.x *= b.x;
        a.y *= b.y;
        a.z *= b.z;
        return a;
    }

    static XMFLOAT3 operator/=(XMFLOAT3& a, XMFLOAT3& b)
    {
        a.x /= b.x;
        a.y /= b.y;
        a.z /= b.z;
        return a;
    }

    static bool operator==(XMFLOAT3& a, XMFLOAT3& b)
    {
        if ((fabs(a.x - b.x) <= _Epsilon) && (fabs(a.y - b.y) <= _Epsilon) && (fabs(a.z - b.z) <= _Epsilon))
            return true;
        else
            return false;
    }

    static bool operator<=(XMFLOAT3& a, XMFLOAT3& b)
    {
        if (a.x <= b.x && a.y <= b.y && a.z <= b.z)
            return true;
        else
            return false;
    }

    static bool operator>=(XMFLOAT3& a, XMFLOAT3& b)
    {
        if (a.x >= b.x && a.y >= b.y && a.z >= b.z)
            return true;
        else
            return false;
    }
    struct S_BOX
    {
        // Common
        XMFLOAT3		vCenter;
        XMFLOAT3		vPos[8];
        // AABB
        XMFLOAT3		vMax;
        XMFLOAT3		vMin;
        // OBB
        XMFLOAT3		vAxis[3];
        float		    fExtent[3];

        unsigned int vRectIndices[36] =
        {
            0, 1, 2, 2, 3, 0,
            3, 2, 6, 6, 7, 3,
            7, 6, 5, 5, 4, 7,
            4, 5, 1, 1, 0, 4,
            4, 0, 3, 3, 7, 4,
            1, 5, 6, 6, 2, 1
        };

        S_BOX()
        {

        }
        S_BOX(XMFLOAT3	max, XMFLOAT3 min)
        {
            Set(max, min);
        }
        void Set(XMFLOAT3 max, XMFLOAT3	min)
        {
            vMax = max;
            vMin = min;
            vCenter = (vMax + vMin);
            vCenter =  vCenter * 0.5f;
            vAxis[0] = { 1,0,0 };
            vAxis[1] = { 0,1,0 };
            vAxis[2] = { 0,0,1 };
            fExtent[0] = vMax.x - vCenter.x;
            fExtent[1] = vMax.y - vCenter.y;
            fExtent[2] = vMax.z - vCenter.z;

            vPos[0] = XMFLOAT3(vMin.x, vMin.y, vMin.z);
            vPos[1] = XMFLOAT3(vMin.x, vMin.y, vMax.z);
            vPos[2] = XMFLOAT3(vMax.x, vMin.y, vMax.z);
            vPos[3] = XMFLOAT3(vMax.x, vMin.y, vMin.z);
            vPos[4] = XMFLOAT3(vMin.x, vMax.y, vMin.z);
            vPos[5] = XMFLOAT3(vMin.x, vMax.y, vMax.z);
            vPos[6] = XMFLOAT3(vMax.x, vMax.y, vMax.z);
            vPos[7] = XMFLOAT3(vMax.x, vMax.y, vMin.z);
        }
        void Set(XMMATRIX matWorld)
        {
            
            // 중심점을 월드 변환 행렬로 변환
            XMVECTOR center = XMLoadFloat3(&vCenter);
            center = XMVector3Transform(center, matWorld);
            XMStoreFloat3(&vCenter, center);

            // 각 포인트를 월드 변환 행렬로 변환
            for (int i = 0; i < 8; ++i)
            {
                XMVECTOR point = XMLoadFloat3(&vPos[i]);
                point = XMVector3Transform(point, matWorld);
                XMStoreFloat3(&vPos[i], point);
            }

            // AABB의 최소, 최대 점을 월드 변환 행렬로 변환
            XMVECTOR minPoint = XMLoadFloat3(&vMin);
            minPoint = XMVector3Transform(minPoint, matWorld);
            XMStoreFloat3(&vMin, minPoint);

            XMVECTOR maxPoint = XMLoadFloat3(&vMax);
            maxPoint = XMVector3Transform(maxPoint, matWorld);
            XMStoreFloat3(&vMax, maxPoint);

            // OBB의 축 벡터를 월드 변환 행렬로 변환
            for (int i = 0; i < 3; ++i)
            {
                XMVECTOR axis = XMLoadFloat3(&vAxis[i]);
                axis = XMVector3Transform(axis, matWorld);
                axis = XMVector3Normalize(axis);
                XMStoreFloat3(&vAxis[i], axis);
            }
            fExtent[0] = vMax.x - vCenter.x;
            fExtent[1] = vMax.y - vCenter.y;
            fExtent[2] = vMax.z - vCenter.z;
        }

        friend std::ostream& operator<<(std::ostream& os, const S_BOX& tBox)
        {
            os << "vAxis:" << tBox.vAxis[0].x << " " << tBox.vAxis[0].y << " " << tBox.vAxis[0].z << " "
                << tBox.vAxis[1].x << " " << tBox.vAxis[1].y << " " << tBox.vAxis[1].z << " "
                << tBox.vAxis[2].x << " " << tBox.vAxis[2].y << " " << tBox.vAxis[2].z << ", ";
            os << "fExtent:" << tBox.fExtent[0] << " " << tBox.fExtent[1] << " " << tBox.fExtent[2];
            return os;
        }

        friend std::stringstream& operator>>(std::stringstream& is, S_BOX& tBox)
        {
            /*std::string line;
            std::getline(is, line);*/

            // axis 값을 추출합니다.
            size_t axis_start = is.str().find("vAxis:") + strlen("vAxis:");
            size_t axis_end = is.str().find(",", axis_start);
            std::string axis_str = is.str().substr(axis_start, axis_end - axis_start);
            std::istringstream axis_stream(axis_str);
            XMFLOAT3 axis[3];
            axis_stream >> axis[0].x >> axis[0].y >> axis[0].z >> axis[1].x >> axis[1].y >> axis[1].z >> axis[2].x >> axis[2].y >> axis[2].z;
            tBox.vAxis[0] = axis[0];
            tBox.vAxis[1] = axis[1];
            tBox.vAxis[2] = axis[2];

            // extent 값을 추출합니다.
            size_t extent_start = is.str().find("fExtent:") + strlen("fExtent:");
            size_t extent_end = is.str().find(",", extent_start);
            std::string extent_str = is.str().substr(extent_start, extent_end - extent_start);
            std::istringstream extent_stream(extent_str);
            float extent[3];
            extent_stream >> extent[0] >> extent[1] >> extent[2];
            tBox.fExtent[0] = extent[0];
            tBox.fExtent[1] = extent[1];
            tBox.fExtent[2] = extent[2];

            return is;
        }
    };
	FQuadTree* OpenMap(std::wstring szFullPath, ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext)
	{
		Texture* pTexture = nullptr;
		std::vector<Texture*> pTexList;
		Transform mapTransform = {};
		UINT iMaxDepth = 0;
		std::wstring szVSPath;
		std::wstring szPSPath;
        std::vector<Cinema> CinemaList;
		MeshMap* pMapMesh = new MeshMap();
		pMapMesh->SetDevice(pd3dDevice, pContext);
        std::vector<std::pair<std::string, Transform>> spawnList;
        std::map<std::wstring, T_BOX> triggerList;
		std::unordered_set<Object*> allObjectList;
		BYTE* fAlphaData = nullptr;
		std::ifstream is(szFullPath);
		std::string line;

        BaseObject* pSphereObject = nullptr;

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
                else if (fieldName == "m_CinemaList")
                {
                    std::streampos prevPos = is.tellg();
                    std::string cinemaLine;
                    while (std::getline(is, cinemaLine))
                    {
                        Cinema cinema;
                        if (cinemaLine.find("m_pMap:") != std::string::npos)
                            break;
                        is.seekg(prevPos);
                        is >> cinema;
                        CinemaList.push_back(cinema);
                        prevPos = is.tellg();
                    }
                    is.seekg(prevPos);
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

                        MAPLOAD::S_BOX box;
                        texturesStream >> box;

						float length;
                        float fRadius;
                        UINT iSliceCount;
                        UINT iStackCount;
						if (specifyMode == "OBJECT_SIMPLE" || specifyMode == "OBJECT_COLLIDER" || specifyMode == "OBJECT_TRIGGER" || specifyMode == "OBJECT_SPAWN")
						{
							// length 값을 추출합니다.
							size_t length_start = texturesStream.str().find("m_fLength:") + strlen("m_fLength:");
							size_t length_end = texturesStream.str().find(",", length_start);
							std::string length_str = texturesStream.str().substr(length_start, length_end - length_start);
							std::istringstream length_stream(length_str);
                            length_stream >> length;
						}
                        else if (specifyMode == "OBJECT_SKYDOME")
                        {
                            // radius 값을 추출합니다.
                            size_t radius_start = texturesStream.str().find("m_fRadius:") + strlen("m_fRadius:");
                            size_t radius_end = texturesStream.str().find(",", radius_start);
                            std::string radius_str = texturesStream.str().substr(radius_start, radius_end - radius_start);
                            std::istringstream radius_stream(radius_str);
                            radius_stream >> fRadius;

                            // slice 값을 추출합니다.
                            size_t slice_start = texturesStream.str().find("m_iSliceCount:") + strlen("m_iSliceCount:");
                            size_t slice_end = texturesStream.str().find(",", slice_start);
                            std::string slice_str = texturesStream.str().substr(slice_start, slice_end - slice_start);
                            std::istringstream slice_stream(slice_str);
                            slice_stream >> iSliceCount;

                            // stack 값을 추출합니다.
                            size_t stack_start = texturesStream.str().find("m_iStackCount:") + strlen("m_iStackCount:");
                            size_t stack_end = texturesStream.str().find(",", stack_start);
                            std::string stack_str = texturesStream.str().substr(stack_start, stack_end - stack_start);
                            std::istringstream stack_stream(stack_str);
                            stack_stream >> iStackCount;
                        }

                        if (specifyMode == "OBJECT_SIMPLE" || specifyMode == "OBJECT_COLLIDER" )
                        {
                            T_BOX mePeedBox;

                            XMFLOAT3 scale;
                            XMStoreFloat3(&scale, transform.scale);
                            XMFLOAT3 rot;
                            XMStoreFloat3(&rot, transform.rotation);
                            XMFLOAT3 translation;
                            XMStoreFloat3(&translation, transform.position);

                            XMMATRIX matWorld = XMMatrixTransformation({ 0,0,0,1 }, { 0,0,0,1 }, transform.scale, { 0,0,0,1 }, XMQuaternionRotationRollPitchYaw(
                                _DegreeToRadian(rot.x),
                                _DegreeToRadian(rot.y),
                                _DegreeToRadian(rot.z)), transform.position);
                            for (int i = 0; i < 3; ++i)
                            {
                                XMVECTOR axis = XMLoadFloat3(&box.vAxis[i]);
                                axis = XMVector3TransformNormal(axis, matWorld);
                                //axis = XMVector3Normalize(axis);
                                XMStoreFloat3(&box.vAxis[i], axis);
                            }
                            
                            //mePeedBox.CreateOBBBox(box.fExtent[0] * scale.x, box.fExtent[1] * scale.y, box.fExtent[2] * scale.z, TVector3(translation), box.vAxis[0], box.vAxis[1], box.vAxis[2]);
                            TVector3 dirX = box.vAxis[0];
                            dirX.Normalize();
                            TVector3 dirY = box.vAxis[1];
                            dirY.Normalize();
                            TVector3 dirZ = box.vAxis[2];
                            dirZ.Normalize();
                            mePeedBox.CreateOBBBox(box.fExtent[0] * scale.x, box.fExtent[1] * scale.y, box.fExtent[2] * scale.z, TVector3(translation), dirX, dirY, dirZ);
                            I_Collision.AddMapCollisionBox(mePeedBox);
                        }
                        else if (specifyMode == "OBJECT_TRIGGER")
                        {
                            T_BOX mePeedBox;

                            XMFLOAT3 scale;
                            XMStoreFloat3(&scale, transform.scale);
                            XMFLOAT3 rot;
                            XMStoreFloat3(&rot, transform.rotation);
                            XMFLOAT3 translation;
                            XMStoreFloat3(&translation, transform.position);

                            XMMATRIX matWorld = XMMatrixTransformation({ 0,0,0,1 }, { 0,0,0,1 }, transform.scale, { 0,0,0,1 }, XMQuaternionRotationRollPitchYaw(
                                _DegreeToRadian(rot.x),
                                _DegreeToRadian(rot.y),
                                _DegreeToRadian(rot.z)), transform.position);
                            for (int i = 0; i < 3; ++i)
                            {
                                XMVECTOR axis = XMLoadFloat3(&box.vAxis[i]);
                                axis = XMVector3TransformNormal(axis, matWorld);
                                //axis = XMVector3Normalize(axis);
                                XMStoreFloat3(&box.vAxis[i], axis);
                            }

                            mePeedBox.CreateOBBBox(box.fExtent[0] * scale.x, box.fExtent[1] * scale.y, box.fExtent[2] * scale.z, TVector3(translation), box.vAxis[0], box.vAxis[1], box.vAxis[2]);
                            triggerList.insert(std::make_pair(to_mw(strName), mePeedBox));
                        }
                        else if (specifyMode == "OBJECT_SPAWN")
                        {
                            spawnList.push_back(std::make_pair(strName, transform));
                        }
                        else if (specifyMode == "OBJECT_SKYDOME")
                        {
                            pSphereObject = new BaseObject();

    
                            float phiStep = XM_PI / iStackCount;
                            float thetaStep = 2.0f * XM_PI / iSliceCount;
                            for (UINT i = 0; i <= iStackCount; i++)
                            {
                                float phi = i * phiStep;

                                for (UINT j = 0; j <= iSliceCount; j++)
                                {
                                    float theta = j * thetaStep;

                                    Vertex vertex;

                                    vertex.p.x = fRadius * sinf(phi) * cosf(theta);
                                    vertex.p.y = fRadius * cosf(phi);
                                    vertex.p.z = fRadius * sinf(phi) * sinf(theta);

                                    vertex.n = vertex.p;
                                    XMStoreFloat3(&vertex.n, XMVector3Normalize(XMLoadFloat3(&vertex.n)));

                                    vertex.c = TVector4(1.0f, 1.0f, 1.0f, 1.0f);

                                    vertex.t.x = (float)j / (float)iSliceCount;
                                    vertex.t.y = (float)i / (float)iStackCount;

                                    pSphereObject->m_VertexList.push_back(vertex);
                                }
                            }

                            for (UINT i = 0; i < iStackCount; i++)
                            {
                                for (UINT j = 0; j < iSliceCount; j++)
                                {
                                    UINT index1 = i * (iSliceCount + 1) + j;
                                    UINT index2 = i * (iSliceCount + 1) + j + 1;
                                    UINT index3 = (i + 1) * (iSliceCount + 1) + j;
                                    UINT index4 = (i + 1) * (iSliceCount + 1) + j + 1;

                                    pSphereObject->m_IndexList.push_back(index1);
                                    pSphereObject->m_IndexList.push_back(index3);
                                    pSphereObject->m_IndexList.push_back(index2);

                                    pSphereObject->m_IndexList.push_back(index2);
                                    pSphereObject->m_IndexList.push_back(index3);
                                    pSphereObject->m_IndexList.push_back(index4);
                                }
                            }
                         
                            PathChanger(strName);
                            pSphereObject->Create(pd3dDevice, pContext, L"../../data/shader/MAP/SkyDomeShader.hlsl", mtw(strName));
                            pSphereObject->Init();
                        }
                        else
                        {
                            Object* pObject = new Object(strName, transform, box.vAxis[0], box.vAxis[1], box.vAxis[2], box.fExtent[0], box.fExtent[1], box.fExtent[2], pd3dDevice, pContext);
                            //if (specifyMode == OBJECT_SPECIFY::OBJECT_SIMPLE)
                            //    pObject = CreateSimpleBox(length, transform.position, transform.rotation, transform.scale);
                            //else if (specifyMode == OBJECT_SPECIFY::OBJECT_STATIC)
                            //    pObject = CreateFbxObject(_tomw(strName), transform.position, transform.rotation, transform.scale);
                            //else if (specifyMode == OBJECT_SPECIFY::OBJECT_SKELETON)
                            //    pObject = CreateFbxObject(_tomw(str), transform.position, transform.rotation, transform.scale);
                            //
                            allObjectList.insert(pObject);
                        }
                         
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

        ConstantData_Transform constantData_Transform;
        constantData_Transform.matWorld = XMMatrixIdentity();
        constantData_Transform.matView = XMMatrixIdentity();
        constantData_Transform.matProj = XMMatrixIdentity();

    std::wstring DefaultShaderPath = L"../../data/shader/MAP/";
		Shader* pVertexShader;
		I_Shader.VSLoad(DefaultShaderPath + szVSPath, L"vsmain", &pVertexShader);
		Shader* pPixelShader;
		I_Shader.PSLoad(DefaultShaderPath + szPSPath, L"psmain", &pPixelShader);

		pMapMesh->m_pVertexBuffer =
			DX::CreateVertexBuffer(pd3dDevice, &pMapMesh->GetListVertex()[0], sizeof(PNCTVertex), pMapMesh->GetListVertex().size());

        if (FAILED(pd3dDevice->CreateInputLayout(layoutPNCT, size_layoutPNCT,
            pVertexShader->m_pVSCode->GetBufferPointer(), 
            pVertexShader->m_pVSCode->GetBufferSize(), 
            &pMapMesh->m_pVertexInputLayout)))
            throw std::exception("InputLayout not create successfully");

		pMapMesh->m_pIndexBuffer =
			DX::CreateIndexBuffer(pd3dDevice, &pMapMesh->GetListIndex()[0], sizeof(DWORD), pMapMesh->GetListIndex().size());

		FQuadTree* pQuadTree = new FQuadTree(pMapMesh, pd3dDevice, pContext, iMaxDepth, fAlphaData);
		pQuadTree->SetConstantData(constantData_Transform);
		pQuadTree->SetTransform({ mapTransform.position, mapTransform.rotation, mapTransform.scale });
		pQuadTree->SetTexture(pTexture);
		for (const auto& texture : pTexList)
			pQuadTree->SetSplattingTexture(texture);
		pQuadTree->SetShader(szVSPath, pVertexShader, szPSPath, pPixelShader);
		//pQuadTree->SetDrawMode(DRAW_MODE::MODE_SOLID);

        for (const auto& spawn : spawnList)
        {
            if (spawn.first == "Player" || spawn.first.find("Cine") != std::string::npos)
                pQuadTree->m_PlayerSpawnPoint.insert(spawn);
            else
                pQuadTree->m_EnemySpawnList.push_back(spawn);
        }
        
        for (const auto& trigger : triggerList)
        {
            pQuadTree->m_TriggerList.insert(trigger);
        }

		for (const auto& obj : allObjectList)
		{
		    pQuadTree->AddObject(obj);
		}

        if (pSphereObject)
            pQuadTree->m_pSphereObject = pSphereObject;

        for (int idx = 0; idx < CinemaList.size(); idx++)
        {
            pQuadTree->m_CinemaList.insert(std::make_pair(CinemaList[idx].szCinemaName, CinemaList[idx]));
        }

		return pQuadTree;
	}

    void	PathChanger(std::string& str)
    {
        //std::string::size_type pos = str.find("D:\\");
        std::string::size_type pos = 0;
        std::string::size_type end = str.find("Assets");
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
