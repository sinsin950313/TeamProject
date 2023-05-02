#pragma once

#include "Mesh.h"
#include "ShaderMgr.h"
#include "CommonPath.h"

namespace SSB
{
	template<typename VertexType>
	Mesh<VertexType>::Mesh()
	{
	}
	template<typename VertexType>
	Mesh<VertexType>::~Mesh()
	{
		Release();
	}
	template<typename VertexType>
	inline void Mesh<VertexType>::CheckSize()
	{
		float minX = (std::numeric_limits<float>::max)();
		float minY = (std::numeric_limits<float>::max)();
		float minZ = (std::numeric_limits<float>::max)();
		float maxX = (std::numeric_limits<float>::min)();
		float maxY = (std::numeric_limits<float>::min)();
		float maxZ = (std::numeric_limits<float>::min)();
		for (auto vertex : _vertexList)
		{
			minX = min(minX, vertex.Position.x);
			minY = min(minY, vertex.Position.y);
			minZ = min(minZ, vertex.Position.z);

			maxX = max(maxX, vertex.Position.x);
			maxY = max(maxY, vertex.Position.y);
			maxZ = max(maxZ, vertex.Position.z);
		}

		_minVertex = { minX, minY, minZ };
		_maxVertex = { maxX, maxY, maxZ };
	}
	template<typename VertexType>
	inline void Mesh<VertexType>::SetVertexList(std::vector<VertexType> vertexList)
	{
		_vertexList = vertexList;
	}
	template<typename VertexType>
	inline void Mesh<VertexType>::SetIndexList(std::vector<IndexForMeshVertice> indexList)
	{
		_indexList = indexList;
	}
	template<typename VertexType>
	void Mesh<VertexType>::SetShader(Shader* shader)
	{
		_vs = shader;
	}
	template<typename VertexType>
	bool Mesh<VertexType>::CreateVertexLayout()
	{
		D3D11_INPUT_ELEMENT_DESC* inputElementDesc;
		int inputElementDescCount;
		SetVertexLayoutDesc(&inputElementDesc, inputElementDescCount);

		HRESULT hr = m_pd3dDevice->CreateInputLayout(inputElementDesc, inputElementDescCount, _vs->m_pVSCode->GetBufferPointer(), _vs->m_pVSCode->GetBufferSize(), &_vertexLayout);
		delete inputElementDesc;

		if (FAILED(hr))
		{
			return false;
		}

		return true;
	}
	template<typename VertexType>
	bool Mesh<VertexType>::CreateVertexBuffer()
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = sizeof(VertexType) * _vertexList.size();
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));
		sd.pSysMem = &_vertexList.at(0);
		HRESULT hr = m_pd3dDevice->CreateBuffer(&bd, &sd, &_vertexBuffer);
		if (FAILED(hr))
		{
			assert(SUCCEEDED(hr));
			return false;
		}
		return true;
	}
	template<typename VertexType>
	bool Mesh<VertexType>::CreateIndexBuffer()
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = sizeof(decltype(_indexList[0])) * _indexList.size();
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));
		sd.pSysMem = &_indexList.at(0);
		HRESULT hr = m_pd3dDevice->CreateBuffer(&bd, &sd, &_indexBuffer);
		if (FAILED(hr))
		{
			assert(SUCCEEDED(hr));
			return false;
		}
		return true;
	}
	template<typename VertexType>
	inline void Mesh<VertexType>::Initialize_AddSubMesh(MeshInterface* mesh)
	{
		_subMeshes.push_back(mesh);
	}
	template<typename VertexType>
	TVector3 Mesh<VertexType>::GetMinVertex()
	{
		return _minVertex;
	}
	template<typename VertexType>
	TVector3 Mesh<VertexType>::GetMaxVertex()
	{
		return _maxVertex;
	}
	template<typename VertexType>
	bool Mesh<VertexType>::Init()
	{
		Build();
		CheckSize();
		if(_vs == nullptr)
			InitialShader();
		CreateVertexBuffer();
		CreateIndexBuffer();
		CreateVertexLayout();

		for (auto subMesh : _subMeshes)
		{
			subMesh->Init();
		}

		return true;
	}
	template<typename VertexType>
	bool Mesh<VertexType>::Frame()
	{
		for (auto subMesh : _subMeshes)
		{
			subMesh->Frame();
		}

		return true;
	}
	template<typename VertexType>
	bool Mesh<VertexType>::Render()
	{
		ID3D11DeviceContext* dc = m_pImmediateContext;

		dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		dc->IASetInputLayout(_vertexLayout);
		{
			UINT stride = sizeof(VertexType);
			UINT offset = 0;
			dc->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
		}
		dc->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		dc->VSSetShader(_vs->m_pVS, NULL, 0);

		dc->DrawIndexed(_indexList.size(), 0, 0);

		for (auto subMesh : _subMeshes)
		{
			subMesh->Render();
		}

		return true;
	}
	template<typename VertexType>
	inline bool Mesh<VertexType>::PostRender()
	{
		ID3D11DeviceContext* dc = m_pImmediateContext;

		dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		dc->IASetInputLayout(_vertexLayout);
		{
			UINT stride = sizeof(VertexType);
			UINT offset = 0;
			dc->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
		}
		dc->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		dc->DrawIndexed(_indexList.size(), 0, 0);

		for (auto subMesh : _subMeshes)
		{
			subMesh->PostRender();
		}

		return true;
	}
	template<typename VertexType>
	bool Mesh<VertexType>::RenderInstancing(UINT iNum)
	{
		ID3D11DeviceContext* dc = m_pImmediateContext;

		dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		dc->IASetInputLayout(_vertexLayout);
		{
			UINT stride = sizeof(VertexType);
			UINT offset = 0;
			dc->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
		}
		dc->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		//dc->DrawIndexed(_indexList.size(), 0, 0);
		dc->DrawIndexedInstanced(_indexList.size(), iNum, 0, 0, 0);

		for (auto subMesh : _subMeshes)
		{
			subMesh->PostRender();
		}

		return true;
	}
	template<typename VertexType>
	bool Mesh<VertexType>::Release()
	{
		if (_vertexLayout != nullptr)
		{
			_vertexLayout->Release();
			_vertexLayout = nullptr;
		}

		_vertexList.clear();

		if (_vertexBuffer != nullptr)
		{
			_vertexBuffer->Release();
			_vertexBuffer = nullptr;
		}

		_indexList.clear();

		if (_indexBuffer != nullptr)
		{
			_indexBuffer->Release();
			_indexBuffer = nullptr;
		}

		for (auto subMesh : _subMeshes)
		{
			subMesh->Release();
			delete subMesh;
		}
		_subMeshes.clear();

		return true;
	}
	template<typename VertexType>
	std::string Mesh<VertexType>::Serialize()
	{
		std::string ret;
		return ret;
	}
	template<typename VertexType>
	void Mesh<VertexType>::Deserialize(const char* buffer, int size, int& offset)
	{
		{
			int vertexCount;
			memcpy(&vertexCount, buffer + offset, sizeof(int));
			offset += sizeof(int);

			_vertexList.resize(vertexCount);
			for (int i = 0; i < vertexCount; ++i)
			{
				_vertexList[i] = GetVertex(buffer, size, offset);
			}
		}

		{
			int indexCount;
			memcpy(&indexCount, buffer + offset, sizeof(int));
			offset += sizeof(int);

			_indexList.resize(indexCount);
			for (int i = 0; i < indexCount; ++i)
			{
				int index;
				memcpy(&index, buffer + offset, sizeof(int));
				offset += sizeof(int);
				_indexList[i] = index;
			}
		}

		{
			XMFLOAT3 tmpBuffer;
			memcpy(&tmpBuffer, buffer + offset, sizeof(XMFLOAT3));
			offset += sizeof(XMFLOAT3);
			_minVertex = tmpBuffer;
		}

		{
			XMFLOAT3 tmpBuffer;
			memcpy(&tmpBuffer, buffer + offset, sizeof(XMFLOAT3));
			offset += sizeof(XMFLOAT3);
			_maxVertex = tmpBuffer;
		}

		{
			int subMeshCount;
			memcpy(&subMeshCount, buffer + offset, sizeof(int));
			offset += sizeof(int);

			_subMeshes.resize(subMeshCount);
			for (int i = 0; i < subMeshCount; ++i)
			{
				MeshInterface* mesh = GetMesh();
				mesh->Deserialize(buffer, size, offset);
				_subMeshes[i] = mesh;
			}
		}

		{
			int vertexShaderFileNameSize;
			memcpy(&vertexShaderFileNameSize, buffer + offset, sizeof(int));
			offset += sizeof(int);

			char* fileName = new char[vertexShaderFileNameSize];
			memcpy(fileName, buffer + offset, vertexShaderFileNameSize);
			offset += vertexShaderFileNameSize;
			std::string tmpFileName(fileName, vertexShaderFileNameSize);
			delete fileName;
			I_Shader.VSLoad(kShaderPath + mtw(tmpFileName), L"VS", &_vs);
		}
	}
}
