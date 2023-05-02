#include "Foliage.h"
#include "FileIOObject.h"
#include "ModelMgr.h"

bool Foliage::Frame()
{
	Character::Frame();
	m_pImmediateContext->UpdateSubresource(m_pInstanceBuffer, NULL, NULL, &m_ListInstanceData[0], NULL, NULL);
	return true;
}

bool Foliage::Render()
{
	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pImmediateContext->VSSetConstantBuffers(0, 1, &_toViewSpaceTransformBuffer);
	m_pImmediateContext->VSSetConstantBuffers(1, 1, &_objectToWorldTransformBuffer);
	m_pImmediateContext->PSSetShader(m_pModel->_ps->m_pPS, NULL, 0);
	for (auto material : m_pModel->_materials)
	{
		material.second->Render();
	}
	for (auto mesh : m_pModel->_meshes)
	{
		//m_pImmediateContext->VSSetConstantBuffers(4, 1, &static_cast<SSB::Mesh_Vertex_PCNT_Animatable*>(mesh.second)->_meshBuffer);
		m_pImmediateContext->IASetInputLayout(static_cast<SSB::Mesh<SSB::Mesh_Vertex_PCNT_Animatable>*>(mesh.second)->_vertexLayout);
		UINT stride = sizeof(SSB::Vertex_PCNT);
		UINT offset = 0;
		m_pImmediateContext->IASetVertexBuffers(0, 1, &static_cast<SSB::Mesh<SSB::Mesh_Vertex_PCNT_Animatable>*>(mesh.second)->_vertexBuffer, &stride, &offset);
		stride = sizeof(InstanceData);
		offset = 0;
		m_pImmediateContext->IASetVertexBuffers(1, 1, &m_pInstanceBuffer, &stride, &offset);
		m_pImmediateContext->IASetIndexBuffer(static_cast<SSB::Mesh<SSB::Mesh_Vertex_PCNT_Animatable>*>(mesh.second)->_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		m_pImmediateContext->VSSetShader(static_cast<SSB::Mesh<SSB::Mesh_Vertex_PCNT_Animatable>*>(mesh.second)->_vs->m_pVS, NULL, 0);

		m_pImmediateContext->DrawIndexedInstanced(static_cast<SSB::Mesh<SSB::Mesh_Vertex_PCNT_Animatable>*>(mesh.second)->_indexList.size(), m_ListInstanceData.size(), 0, 0, 0);
	}
	/*m_pImmediateContext->VSSetConstantBuffers(4, 1, &m_pModel->_meshBuffer);*/
	//m_pModel->Render();
	return true;
}

Foliage::Foliage(std::string szFullPath, std::vector<InstanceData>* pListInstanceData, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	Character::SetDevice(pDevice, pDeviceContext);
	auto splitedPath = SplitPath(mtw(szFullPath));

	SSB::ObjectScriptIO io;
	auto scriptInfo = io.Read(wtm(splitedPath[2]));

	//I_Model.Load(str, "Idle", &m_pModel);
	// static object
	m_pModel = new SSB::Model;
	m_pModel->SetDevice(pDevice, pDeviceContext);
	int offset = 0;
	m_pModel->Deserialize(scriptInfo.Pointer, scriptInfo.BufferSize, offset);
	m_pModel->Init();

	for (int idx = 0; idx < pListInstanceData->size(); idx++)
		m_ListInstanceData.push_back(pListInstanceData->at(idx));

	m_pInstanceBuffer = DX::CreateVertexBuffer(pDevice, &m_ListInstanceData[0], m_ListInstanceData.size(), sizeof(InstanceData));
	for (auto mesh : m_pModel->_meshes)
	{	

		/*static_cast<SSB::Mesh<SSB::Mesh_Vertex_PCNT_Animatable>*>(mesh.second)->_vertexLayout->Release();*/
		auto pVS = static_cast<SSB::Mesh<SSB::Mesh_Vertex_PCNT_Animatable>*>(mesh.second)->_vs;
		D3D11_INPUT_ELEMENT_DESC layoutPNCTInstance[] =
		{
			//SEMANTIC NAME, SEMANTIC INDEX, FORMAT, INPUT SLOT, ALIGNED BYTE OFFSET, INPUT SLOT CLASS, INSTANCE DATA STEP RATE, 
			{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},		//POSITION0À» ÀÇ¹Ì
			{"Color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"Normal", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"Diffuse", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},

			{"WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			/*{"COLOR", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},*/
		};
		UINT size_layoutPNCTInstance = ARRAYSIZE(layoutPNCTInstance);

		ID3D11InputLayout* pInputLayout = nullptr;
		pDevice->CreateInputLayout(layoutPNCTInstance, size_layoutPNCTInstance, pVS->m_pVSCode->GetBufferPointer(), pVS->m_pVSCode->GetBufferSize(), &pInputLayout);
		static_cast<SSB::Mesh<SSB::Mesh_Vertex_PCNT_Animatable>*>(mesh.second)->_vertexLayout = pInputLayout;
	}
}

Foliage::~Foliage()
{
	if (m_pInstanceBuffer)
	{
		m_pInstanceBuffer->Release();
		m_pInstanceBuffer = nullptr;
	}
}
