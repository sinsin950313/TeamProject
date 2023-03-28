#include "Mesh.h"
#include "Mesh.hpp"
#include "ShaderMgr.h"
#include "CommonPath.h"

namespace SSB
{
	std::string Mesh_Vertex_PC::GetVertexType()
	{
		return Vertex_PC_Keyword;
	}
	void Mesh_Vertex_PC::SetVertexLayoutDesc(D3D11_INPUT_ELEMENT_DESC** desc, int& count)
	{
		count = 2;

		*desc = new D3D11_INPUT_ELEMENT_DESC[count];
		(*desc)[0] = { "Position", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		(*desc)[1] = { "Color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	}

	void Mesh_Vertex_PC::InitialShader()
	{
		Shader* pShader;
		I_Shader.VSLoad(kShaderPath + L"Default3DVertexShader_PC.hlsl", L"VS", &pShader);
		SetShader(pShader);
	}

	Vertex_PC Mesh_Vertex_PC::GetVertex(const char* buffer, int size, int& offset)
	{
		Vertex_PC ret;

		{
			XMFLOAT4 tmp;
			memcpy(&tmp, buffer + offset, sizeof(XMFLOAT4));
			offset += sizeof(XMFLOAT4);
			ret.Position = tmp;
		}

		{
			XMFLOAT4 tmp;
			memcpy(&tmp, buffer + offset, sizeof(XMFLOAT4));
			offset += sizeof(XMFLOAT4);
			ret.Color = tmp;
		}

		return ret;
	}

	Mesh<Vertex_PC>* Mesh_Vertex_PC::GetMesh()
	{
		return new Mesh_Vertex_PC;
	}

	std::string Mesh_Vertex_PCNT::GetVertexType()
	{
		return Vertex_PCNT_Keyword;
	}
	void Mesh_Vertex_PCNT::SetVertexLayoutDesc(D3D11_INPUT_ELEMENT_DESC** desc, int& count)
	{
		count = 4;

		*desc = new D3D11_INPUT_ELEMENT_DESC[count];
		(*desc)[0] = { "Position", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		(*desc)[1] = { "Color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		(*desc)[2] = { "Normal", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		(*desc)[3] = { "Diffuse", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	}

	void Mesh_Vertex_PCNT::InitialShader()
	{
		Shader* pShader;
		I_Shader.VSLoad(kShaderPath + L"Default3DVertexShader_PCNT.hlsl", L"VS", &pShader);
		SetShader(pShader);
	}

	Vertex_PCNT Mesh_Vertex_PCNT::GetVertex(const char* buffer, int size, int& offset)
	{
		Vertex_PCNT ret;

		{
			XMFLOAT4 tmp;
			memcpy(&tmp, buffer + offset, sizeof(XMFLOAT4));
			offset += sizeof(XMFLOAT4);
			ret.Position = tmp;
		}

		{
			XMFLOAT4 tmp;
			memcpy(&tmp, buffer + offset, sizeof(XMFLOAT4));
			offset += sizeof(XMFLOAT4);
			ret.Color = tmp;
		}

		{
			XMFLOAT4 tmp;
			memcpy(&tmp, buffer + offset, sizeof(XMFLOAT4));
			offset += sizeof(XMFLOAT4);
			ret.Normal = tmp;
		}

		{
			XMFLOAT2 tmp;
			memcpy(&tmp, buffer + offset, sizeof(XMFLOAT2));
			offset += sizeof(XMFLOAT2);
			ret.TextureUV = tmp;
		}

		return ret;
	}

	Mesh<Vertex_PCNT>* Mesh_Vertex_PCNT::GetMesh()
	{
		return new Mesh_Vertex_PCNT;
	}

	bool Mesh_Vertex_PCNT_Animatable::CreateMeshBuffer()
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
		bd.ByteWidth = sizeof(MeshData);
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));
		sd.pSysMem = &_meshData;
		HRESULT hr = m_pd3dDevice->CreateBuffer(&bd, &sd, &_meshBuffer);
		if (FAILED(hr))
		{
			assert(SUCCEEDED(hr));
			return false;
		}
		return true;
	}

	std::string Mesh_Vertex_PCNT_Animatable::GetVertexType()
	{
		return Vertex_PCNT_Animatable_Keyword;
	}
	void Mesh_Vertex_PCNT_Animatable::SetVertexLayoutDesc(D3D11_INPUT_ELEMENT_DESC** desc, int& count)
	{
		count = 4;

		*desc = new D3D11_INPUT_ELEMENT_DESC[count];
		(*desc)[0] = { "Position", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		(*desc)[1] = { "Color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		(*desc)[2] = { "Normal", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		(*desc)[3] = { "Diffuse", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	}

	void Mesh_Vertex_PCNT_Animatable::Initialize_SetMeshData(MeshData meshData)
	{
		_meshData = meshData;
	}

	bool Mesh_Vertex_PCNT_Animatable::Init()
	{
		Mesh<Vertex_PCNT>::Init();
		CreateMeshBuffer();

		return true;
	}

	bool Mesh_Vertex_PCNT_Animatable::Render()
	{
		m_pImmediateContext->VSSetConstantBuffers(4, 1, &_meshBuffer);
		Mesh<Vertex_PCNT>::Render();
		return true;
	}

	bool Mesh_Vertex_PCNT_Animatable::Release()
	{
		if (_meshBuffer != nullptr)
		{
			_meshBuffer->Release();
			_meshBuffer = nullptr;
		}

		Mesh<Vertex_PCNT>::Release();
		return true;
	}
	std::string Mesh_Vertex_PCNT_Animatable::Serialize()
	{
		std::string ret;

		return ret;
	}

	void Mesh_Vertex_PCNT_Animatable::Deserialize(const char* buffer, int size, int& offset)
	{
		Mesh<Vertex_PCNT>::Deserialize(buffer, size, offset);

		memcpy(&_meshData.MeshIndex, buffer + offset, sizeof(int));
		offset += sizeof(int);
	}

	void Mesh_Vertex_PCNT_Animatable::InitialShader()
	{
		Shader* pShader;
		I_Shader.VSLoad(kShaderPath + L"Default3DVertexShader_PCNT_Animatable.hlsl", L"VS", &pShader);
		SetShader(pShader);
	}

	Vertex_PCNT Mesh_Vertex_PCNT_Animatable::GetVertex(const char* buffer, int size, int& offset)
	{
		Vertex_PCNT ret;

		{
			XMFLOAT4 tmp;
			memcpy(&tmp, buffer + offset, sizeof(XMFLOAT4));
			offset += sizeof(XMFLOAT4);
			ret.Position = tmp;
		}

		{
			XMFLOAT4 tmp;
			memcpy(&tmp, buffer + offset, sizeof(XMFLOAT4));
			offset += sizeof(XMFLOAT4);
			ret.Color = tmp;
		}

		{
			XMFLOAT4 tmp;
			memcpy(&tmp, buffer + offset, sizeof(XMFLOAT4));
			offset += sizeof(XMFLOAT4);
			ret.Normal = tmp;
		}

		{
			XMFLOAT2 tmp;
			memcpy(&tmp, buffer + offset, sizeof(XMFLOAT2));
			offset += sizeof(XMFLOAT2);
			ret.TextureUV = tmp;
		}

		return ret;
	}

	Mesh<Vertex_PCNT>* Mesh_Vertex_PCNT_Animatable::GetMesh()
	{
		return new Mesh_Vertex_PCNT_Animatable;
	}

	void Mesh_Vertex_PCNT_Skinning::SetVertexLayoutDesc(D3D11_INPUT_ELEMENT_DESC** desc, int& count)
	{
		count = 6;
	
		*desc = new D3D11_INPUT_ELEMENT_DESC[count];
		(*desc)[0] = { "Position", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		(*desc)[1] = { "Color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		(*desc)[2] = { "Normal", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		(*desc)[3] = { "Diffuse", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		(*desc)[4] = { "AffectingBoneIndex", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, 56, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		(*desc)[5] = { "Weight", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 72, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	}

	void Mesh_Vertex_PCNT_Skinning::InitialShader()
	{
		Shader* pShader;
		I_Shader.VSLoad(kShaderPath + L"Default3DVertexShader_PCNT_Skinning.hlsl", L"VS", &pShader);
		SetShader(pShader);
	}

	Vertex_PCNT_Skinning Mesh_Vertex_PCNT_Skinning::GetVertex(const char* buffer, int size, int& offset)
	{
		Vertex_PCNT_Skinning ret;

		{
			XMFLOAT4 tmp;
			memcpy(&tmp, buffer + offset, sizeof(XMFLOAT4));
			offset += sizeof(XMFLOAT4);
			ret.Position = tmp;
		}

		{
			XMFLOAT4 tmp;
			memcpy(&tmp, buffer + offset, sizeof(XMFLOAT4));
			offset += sizeof(XMFLOAT4);
			ret.Color = tmp;
		}

		{
			XMFLOAT4 tmp;
			memcpy(&tmp, buffer + offset, sizeof(XMFLOAT4));
			offset += sizeof(XMFLOAT4);
			ret.Normal = tmp;
		}

		{
			XMFLOAT2 tmp;
			memcpy(&tmp, buffer + offset, sizeof(XMFLOAT2));
			offset += sizeof(XMFLOAT2);
			ret.TextureUV = tmp;
		}

		{
			for (int i = 0; i < 4; ++i)
			{
				memcpy(&ret.AffectedBoneIndex[i], buffer + offset, sizeof(int));
				offset += sizeof(int);

				memcpy(&ret.Weight[i], buffer + offset, sizeof(float));
				offset += sizeof(float);
			}
		}

		return ret;
	}

	Mesh<Vertex_PCNT_Skinning>* Mesh_Vertex_PCNT_Skinning::GetMesh()
	{
		return new Mesh_Vertex_PCNT_Skinning;
	}

	std::string Mesh_Vertex_PCNT_Skinning::GetVertexType()
	{
		return Vertex_PCNT_Skinning_Keyword;
	}

	bool Mesh_Vertex_PCNT_Skinning::CreateBoneSpaceTransformBuffer()
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = sizeof(MeshToBoneSpaceTransformData);
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));
		sd.pSysMem = &_boneSpaceTransformData;
		HRESULT hr = m_pd3dDevice->CreateBuffer(&bd, &sd, &_boneSpaceTransformBuffer);
		if (FAILED(hr))
		{
			assert(SUCCEEDED(hr));
			return false;
		}
		return true;
	}

	void Mesh_Vertex_PCNT_Skinning::SetMeshData(MeshToBoneSpaceTransformData data)
	{
		for (int i = 0; i < kMaximumBoneCount; ++i)
		{
			_boneSpaceTransformData.BoneSpaceTransformBuffer[i] = data.BoneSpaceTransformBuffer[i].Transpose();
		}
	}

	void Mesh_Vertex_PCNT_Skinning::SetMeshAnimationWeight(float weight)
	{
		_boneSpaceTransformData.MeshWeight = weight;
	}

	void Mesh_Vertex_PCNT_Skinning::Initialize_SetMaxBoneCount(int maxBoneCount)
	{
		_maxBoneCount = maxBoneCount;
	}

	void Mesh_Vertex_PCNT_Skinning::Initialize_SetMeshData(int index)
	{
		_boneSpaceTransformData.MeshIndex = index;
	}

	bool Mesh_Vertex_PCNT_Skinning::Init()
	{
		Mesh<Vertex_PCNT_Skinning>::Init();

		CreateBoneSpaceTransformBuffer();

		return true;
	}

	bool Mesh_Vertex_PCNT_Skinning::Render()
	{
		m_pImmediateContext->VSSetConstantBuffers(4, 1, &_boneSpaceTransformBuffer);

		Mesh<Vertex_PCNT_Skinning>::Render();

		return true;
	}

	std::string Mesh_Vertex_PCNT_Skinning::Serialize()
	{
		std::string ret;

		return ret;
	}
	void Mesh_Vertex_PCNT_Skinning::Deserialize(const char* buffer, int size, int& offset)
	{
		Mesh<Vertex_PCNT_Skinning>::Deserialize(buffer, size, offset);

		{
			memcpy(&_maxBoneCount, buffer + offset, sizeof(int));
			offset += sizeof(int);
		}

		for (int i = 0; i < _maxBoneCount; ++i)
		{
			XMFLOAT4X4 tmpBuffer;
			memcpy(&tmpBuffer, buffer + offset, sizeof(XMFLOAT4X4));
			offset += sizeof(XMFLOAT4X4);
			_boneSpaceTransformData.BoneSpaceTransformBuffer[i] = tmpBuffer;
		}

		memcpy(&_boneSpaceTransformData.MeshIndex, buffer + offset, sizeof(int));
		offset += sizeof(int);

		memcpy(&_boneSpaceTransformData.MeshWeight, buffer + offset, sizeof(float));
		offset += sizeof(float);
	}
	bool Mesh_Vertex_PCNT_Skinning::Release()
	{
		if (_boneSpaceTransformBuffer != nullptr)
		{
			_boneSpaceTransformBuffer->Release();
			_boneSpaceTransformBuffer = nullptr;
		}

		Mesh<Vertex_PCNT_Skinning>::Release();

		return true;
	}

	void Mesh_Vertex_PCNTs::SetVertexLayoutDesc(D3D11_INPUT_ELEMENT_DESC** desc, int& count)
	{
		count = 5;

		*desc = new D3D11_INPUT_ELEMENT_DESC[count];
		(*desc)[0] = { "Position", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		(*desc)[1] = { "Color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		(*desc)[2] = { "Normal", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		(*desc)[3] = { "Diffuse", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		(*desc)[4] = { "MaterialIndex", 0, DXGI_FORMAT_R32_UINT, 0, 56, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	}

	void Mesh_Vertex_PCNTs::InitialShader()
	{
		Shader* pShader;
		I_Shader.VSLoad(kShaderPath + L"Default3DVertexShader_PCNTs.hlsl", L"VS", &pShader);
		SetShader(pShader);
	}

	Vertex_PCNTs Mesh_Vertex_PCNTs::GetVertex(const char* buffer, int size, int& offset)
	{
		Vertex_PCNTs ret;

		{
			XMFLOAT4 tmp;
			memcpy(&tmp, buffer + offset, sizeof(XMFLOAT4));
			offset += sizeof(XMFLOAT4);
			ret.Position = tmp;
		}

		{
			XMFLOAT4 tmp;
			memcpy(&tmp, buffer + offset, sizeof(XMFLOAT4));
			offset += sizeof(XMFLOAT4);
			ret.Color = tmp;
		}

		{
			XMFLOAT4 tmp;
			memcpy(&tmp, buffer + offset, sizeof(XMFLOAT4));
			offset += sizeof(XMFLOAT4);
			ret.Normal = tmp;
		}

		{
			XMFLOAT2 tmp;
			memcpy(&tmp, buffer + offset, sizeof(XMFLOAT2));
			offset += sizeof(XMFLOAT2);
			ret.TextureUV = tmp;
		}

		{
			memcpy(&ret.MaterialIndex, buffer + offset, sizeof(unsigned int));
			offset += sizeof(unsigned int);
		}

		return ret;
	}

	Mesh<Vertex_PCNTs>* Mesh_Vertex_PCNTs::GetMesh()
	{
		return new Mesh_Vertex_PCNTs;
	}

	std::string Mesh_Vertex_PCNTs::GetVertexType()
	{
		return Vertex_PCNTs_Keyword;
	}

	bool Mesh_Vertex_PCNTs_Animatable::CreateMeshBuffer()
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = sizeof(MeshData);
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));
		sd.pSysMem = &_meshData;
		HRESULT hr = m_pd3dDevice->CreateBuffer(&bd, &sd, &_meshBuffer);
		if (FAILED(hr))
		{
			assert(SUCCEEDED(hr));
			return false;
		}
		return true;
	}

	void Mesh_Vertex_PCNTs_Animatable::SetVertexLayoutDesc(D3D11_INPUT_ELEMENT_DESC** desc, int& count)
	{
		count = 5;

		*desc = new D3D11_INPUT_ELEMENT_DESC[count];
		(*desc)[0] = { "Position", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		(*desc)[1] = { "Color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		(*desc)[2] = { "Normal", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		(*desc)[3] = { "Diffuse", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		(*desc)[4] = { "MaterialIndex", 0, DXGI_FORMAT_R32_UINT, 0, 56, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	}

	void Mesh_Vertex_PCNTs_Animatable::InitialShader()
	{
		Shader* pShader;
		I_Shader.VSLoad(kShaderPath + L"Default3DVertexShader_PCNTs_Animatable.hlsl", L"VS", &pShader);
		SetShader(pShader);
	}

	Vertex_PCNTs Mesh_Vertex_PCNTs_Animatable::GetVertex(const char* buffer, int size, int& offset)
	{
		Vertex_PCNTs ret;

		{
			XMFLOAT4 tmp;
			memcpy(&tmp, buffer + offset, sizeof(XMFLOAT4));
			offset += sizeof(XMFLOAT4);
			ret.Position = tmp;
		}

		{
			XMFLOAT4 tmp;
			memcpy(&tmp, buffer + offset, sizeof(XMFLOAT4));
			offset += sizeof(XMFLOAT4);
			ret.Color = tmp;
		}

		{
			XMFLOAT4 tmp;
			memcpy(&tmp, buffer + offset, sizeof(XMFLOAT4));
			offset += sizeof(XMFLOAT4);
			ret.Normal = tmp;
		}

		{
			XMFLOAT2 tmp;
			memcpy(&tmp, buffer + offset, sizeof(XMFLOAT2));
			offset += sizeof(XMFLOAT2);
			ret.TextureUV = tmp;
		}

		{
			memcpy(&ret.MaterialIndex, buffer + offset, sizeof(unsigned int));
			offset += sizeof(unsigned int);
		}

		return ret;
	}

	Mesh<Vertex_PCNTs>* Mesh_Vertex_PCNTs_Animatable::GetMesh()
	{
		return new Mesh_Vertex_PCNTs_Animatable;
	}

	std::string Mesh_Vertex_PCNTs_Animatable::GetVertexType()
	{
		return Vertex_PCNTs_Animatable_Keyword;
	}

	void Mesh_Vertex_PCNTs_Animatable::Initialize_SetMeshData(MeshData meshData)
	{
		_meshData = meshData;
	}

	bool Mesh_Vertex_PCNTs_Animatable::Init()
	{
		Mesh<Vertex_PCNTs>::Init();
		CreateMeshBuffer();
		return true;
	}

	bool Mesh_Vertex_PCNTs_Animatable::Render()
	{
		m_pImmediateContext->VSSetConstantBuffers(4, 1, &_meshBuffer);

		Mesh<Vertex_PCNTs>::Render();
		return true;
	}

	bool Mesh_Vertex_PCNTs_Animatable::Release()
	{
		if (_meshBuffer != nullptr)
		{
			_meshBuffer->Release();
			_meshBuffer = nullptr;
		}

		Mesh<Vertex_PCNTs>::Release();
		return true;
	}
	std::string Mesh_Vertex_PCNTs_Animatable::Serialize()
	{
		std::string ret;

		return ret;
	}

	void Mesh_Vertex_PCNTs_Animatable::Deserialize(const char* buffer, int size, int& offset)
	{
		Mesh<Vertex_PCNTs>::Deserialize(buffer, size, offset);

		memcpy(&_meshData.MeshIndex, buffer + offset, sizeof(int));
		offset += sizeof(int);
	}

	std::string Mesh_Vertex_PCNTs_Skinning::GetVertexType()
	{
		return Vertex_PCNTs_Skinning_Keyword;
	}
	void Mesh_Vertex_PCNTs_Skinning::SetVertexLayoutDesc(D3D11_INPUT_ELEMENT_DESC** desc, int& count)
	{
		count = 7;

		*desc = new D3D11_INPUT_ELEMENT_DESC[count];
		(*desc)[0] = { "Position", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		(*desc)[1] = { "Color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		(*desc)[2] = { "Normal", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		(*desc)[3] = { "Diffuse", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		(*desc)[4] = { "AffectingBoneIndex", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, 56, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		(*desc)[5] = { "Weight", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 72, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		(*desc)[6] = { "MaterialIndex", 0, DXGI_FORMAT_R32_UINT, 0, 88, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	}

	void Mesh_Vertex_PCNTs_Skinning::InitialShader()
	{
		Shader* pShader;
		I_Shader.VSLoad(kShaderPath + L"Default3DVertexShader_PCNTs_Skinning.hlsl", L"VS", &pShader);
		SetShader(pShader);
	}

	Vertex_PCNTs_Skinning Mesh_Vertex_PCNTs_Skinning::GetVertex(const char* buffer, int size, int& offset)
	{
		Vertex_PCNTs_Skinning ret;

		{
			XMFLOAT4 tmp;
			memcpy(&tmp, buffer + offset, sizeof(XMFLOAT4));
			offset += sizeof(XMFLOAT4);
			ret.Position = tmp;
		}

		{
			XMFLOAT4 tmp;
			memcpy(&tmp, buffer + offset, sizeof(XMFLOAT4));
			offset += sizeof(XMFLOAT4);
			ret.Color = tmp;
		}

		{
			XMFLOAT4 tmp;
			memcpy(&tmp, buffer + offset, sizeof(XMFLOAT4));
			offset += sizeof(XMFLOAT4);
			ret.Normal = tmp;
		}

		{
			XMFLOAT2 tmp;
			memcpy(&tmp, buffer + offset, sizeof(XMFLOAT2));
			offset += sizeof(XMFLOAT2);
			ret.TextureUV = tmp;
		}

		{
			memcpy(&ret.MaterialIndex, buffer + offset, sizeof(unsigned int));
			offset += sizeof(unsigned int);
		}

		{
			for (int i = 0; i < 4; ++i)
			{
				memcpy(&ret.AffectedBoneIndex[i], buffer + offset, sizeof(int));
				offset += sizeof(int);

				memcpy(&ret.Weight[i], buffer + offset, sizeof(float));
				offset += sizeof(float);
			}
		}

		return ret;
	}

	Mesh<Vertex_PCNTs_Skinning>* Mesh_Vertex_PCNTs_Skinning::GetMesh()
	{
		return new Mesh_Vertex_PCNTs_Skinning;
	}

	bool Mesh_Vertex_PCNTs_Skinning::CreateBoneSpaceTransformBuffer()
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = sizeof(MeshToBoneSpaceTransformData);
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));
		sd.pSysMem = &_boneSpaceTransformData;
		HRESULT hr = m_pd3dDevice->CreateBuffer(&bd, &sd, &_boneSpaceTransformBuffer);
		if (FAILED(hr))
		{
			assert(SUCCEEDED(hr));
			return false;
		}
		return true;
	}

	void Mesh_Vertex_PCNTs_Skinning::SetMeshData(MeshToBoneSpaceTransformData data)
	{
		for (int i = 0; i < kMaximumBoneCount; ++i)
		{
			_boneSpaceTransformData.BoneSpaceTransformBuffer[i] = data.BoneSpaceTransformBuffer[i].Transpose();
		}
	}

	void Mesh_Vertex_PCNTs_Skinning::Initialize_SetMaxBoneCount(int maxBoneCount)
	{
		_maxBoneCount = maxBoneCount;
	}

	void Mesh_Vertex_PCNTs_Skinning::Initialize_SetMeshData(int index)
	{
		_boneSpaceTransformData.MeshIndex = index;
	}

	void Mesh_Vertex_PCNTs_Skinning::SetMeshAnimationWeight(float weight)
	{
		_boneSpaceTransformData.MeshWeight = weight;
	}

	bool Mesh_Vertex_PCNTs_Skinning::Init()
	{
		Mesh<Vertex_PCNTs_Skinning>::Init();

		CreateBoneSpaceTransformBuffer();

		return true;
	}

	bool Mesh_Vertex_PCNTs_Skinning::Render()
	{
		m_pImmediateContext->VSSetConstantBuffers(4, 1, &_boneSpaceTransformBuffer);

		Mesh<Vertex_PCNTs_Skinning>::Render();

		return true;
	}

	bool Mesh_Vertex_PCNTs_Skinning::Release()
	{
		Mesh<Vertex_PCNTs_Skinning>::Release();

		if (_boneSpaceTransformBuffer != nullptr)
		{
			_boneSpaceTransformBuffer->Release();
			_boneSpaceTransformBuffer = nullptr;
		}

		Mesh<Vertex_PCNTs_Skinning>::Release();

		return true;
	}

	std::string Mesh_Vertex_PCNTs_Skinning::Serialize()
	{
		std::string ret;

		return ret;
	}

	void Mesh_Vertex_PCNTs_Skinning::Deserialize(const char* buffer, int size, int& offset)
	{
		Mesh<Vertex_PCNTs_Skinning>::Deserialize(buffer, size, offset);

		{
			memcpy(&_maxBoneCount, buffer + offset, sizeof(int));
			offset += sizeof(int);
		}

		for (int i = 0; i < _maxBoneCount; ++i)
		{
			XMFLOAT4X4 tmpBuffer;
			memcpy(&tmpBuffer, buffer + offset, sizeof(XMFLOAT4X4));
			offset += sizeof(XMFLOAT4X4);
			_boneSpaceTransformData.BoneSpaceTransformBuffer[i] = tmpBuffer;
		}

		memcpy(&_boneSpaceTransformData.MeshIndex, buffer + offset, sizeof(int));
		offset += sizeof(int);

		memcpy(&_boneSpaceTransformData.MeshWeight, buffer + offset, sizeof(float));
		offset += sizeof(float);
	}

	/*
	Box::Box(float width, float height, float depth)
		: _width(width), _height(height), _depth(depth)
	{
	}
	void Box::Build()
	{
		std::vector<Vertex_PCNT> vertexList;
		vertexList.resize(24);
		// ¾Õ¸é
		vertexList[0] = Vertex_PCNT{ XMFLOAT4{-0.5f * _width, 0.5f * _height, -0.5f * _depth, 1.0f}, XMFLOAT4{0.5f, 0.0f, 0.0f, 1.0f}, XMFLOAT4{0.0f, 0.0f, -1.0f, 1.0f}, XMFLOAT2{0.0f, 0.0f} };
		vertexList[1] = Vertex_PCNT{ XMFLOAT4{0.5f * _width, 0.5f * _height, -0.5f * _depth, 1.0f}, XMFLOAT4{0.5f, 0.0f, 0.0f, 1.0f}, XMFLOAT4{0.0f, 0.0f, -1.0f, 1.0f}, XMFLOAT2{0.5f, 0.0f} };
		vertexList[2] = Vertex_PCNT{ XMFLOAT4{0.5f * _width, -0.5f * _height, -0.5f * _depth, 1.0f}, XMFLOAT4{0.5f, 0.0f, 0.0f, 1.0f}, XMFLOAT4{0.0f, 0.0f, -1.0f, 1.0f}, XMFLOAT2{0.5f, 0.5f} };
		vertexList[3] = Vertex_PCNT{ XMFLOAT4{-0.5f * _width, -0.5f * _height, -0.5f * _depth, 1.0f}, XMFLOAT4{0.5f, 0.0f, 0.0f, 1.0f}, XMFLOAT4{0.0f, 0.0f, -1.0f, 1.0f}, XMFLOAT2{0.0f, 0.5f} };
		// µÞ¸é
		vertexList[4] = Vertex_PCNT{ XMFLOAT4{0.5f * _width, 0.5f * _height, 0.5f * _depth, 1.0f}, XMFLOAT4{0.0f, 0.0f, 0.0f, 1.0f}, XMFLOAT4{0.0f, 0.0f, 1.0f, 1.0f}, XMFLOAT2{0.0f, 0.0f} };
		vertexList[5] = Vertex_PCNT{ XMFLOAT4{-0.5f * _width, 0.5f * _height, 0.5f * _depth, 1.0f}, XMFLOAT4{0.0f, 0.5f, 0.0f, 1.0f}, XMFLOAT4{0.0f, 0.0f, 1.0f, 1.0f}, XMFLOAT2{0.5f, 0.0f} };
		vertexList[6] = Vertex_PCNT{ XMFLOAT4{-0.5f * _width, -0.5f * _height, 0.5f * _depth, 1.0f}, XMFLOAT4{0.0f, 0.5f, 0.0f, 1.0f}, XMFLOAT4{0.0f, 0.0f, 1.0f, 1.0f}, XMFLOAT2{0.5f, 0.5f} };
		vertexList[7] = Vertex_PCNT{ XMFLOAT4{0.5f * _width, -0.5f * _height, 0.5f * _depth, 1.0f}, XMFLOAT4{0.0f, 0.5f, 0.0f, 1.0f}, XMFLOAT4{0.0f, 0.0f, 1.0f, 1.0f}, XMFLOAT2{0.0f, 0.5f} };

		// ¿À¸¥ÂÊ
		vertexList[8] = Vertex_PCNT{ XMFLOAT4{0.5f * _width, 0.5f * _height, -0.5f * _depth, 1.0f}, XMFLOAT4{0.0f, 0.0f, 0.5f, 1.0f}, XMFLOAT4{1.0f, 0.0f, 0.0f, 1.0f}, XMFLOAT2{0.0f, 0.0f} };
		vertexList[9] = Vertex_PCNT{ XMFLOAT4{0.5f * _width, 0.5f * _height, 0.5f * _depth, 1.0f}, XMFLOAT4{0.0f, 0.0f, 0.5f, 1.0f}, XMFLOAT4{1.0f, 0.0f, 0.0f, 1.0f}, XMFLOAT2{0.5f, 0.0f} };
		vertexList[10] = Vertex_PCNT{ XMFLOAT4{0.5f * _width, -0.5f * _height, 0.5f * _depth, 1.0f}, XMFLOAT4{0.0f, 0.0f, 0.5f, 1.0f}, XMFLOAT4{1.0f, 0.0f, 0.0f, 1.0f}, XMFLOAT2{0.5f, 0.5f} };
		vertexList[11] = Vertex_PCNT{ XMFLOAT4{0.5f * _width, -0.5f * _height, -0.5f * _depth, 1.0f}, XMFLOAT4{0.0f, 0.0f, 0.5f, 1.0f}, XMFLOAT4{1.0f, 0.0f, 0.0f, 1.0f}, XMFLOAT2{0.0f, 0.5f} };

		// ¿ÞÂÊ
		vertexList[12] = Vertex_PCNT{ XMFLOAT4{-0.5f * _width, 0.5f * _height, 0.5f * _depth, 1.0f}, XMFLOAT4{0.5f, 0.5f, 0.0f, 1.0f}, XMFLOAT4{-1.0f, 0.0f, 0.0f, 1.0f}, XMFLOAT2{0.0f, 0.0f} };
		vertexList[13] = Vertex_PCNT{ XMFLOAT4{-0.5f * _width, 0.5f * _height, -0.5f * _depth, 1.0f}, XMFLOAT4{0.5f, 0.5f, 0.0f, 1.0f}, XMFLOAT4{-1.0f, 0.0f, 0.0f, 1.0f}, XMFLOAT2{0.5f, 0.0f} };
		vertexList[14] = Vertex_PCNT{ XMFLOAT4{-0.5f * _width, -0.5f * _height, -0.5f * _depth, 1.0f}, XMFLOAT4{0.5f, 0.5f, 0.0f, 1.0f}, XMFLOAT4{-1.0f, 0.0f, 0.0f, 1.0f}, XMFLOAT2{0.5f, 0.5f} };
		vertexList[15] = Vertex_PCNT{ XMFLOAT4{-0.5f * _width, -0.5f * _height, 0.5f * _depth, 1.0f}, XMFLOAT4{0.5f, 0.5f, 0.0f, 1.0f}, XMFLOAT4{-1.0f, 0.0f, 0.0f, 1.0f}, XMFLOAT2{0.0f, 0.5f} };

		// À­¸é
		vertexList[16] = Vertex_PCNT{ XMFLOAT4{-0.5f * _width, 0.5f * _height, 0.5f * _depth, 1.0f}, XMFLOAT4{0.5f, 0.5f, 0.5f, 1.0f}, XMFLOAT4{0.0f, 1.0f, 0.0f, 1.0f}, XMFLOAT2{0.0f, 0.0f} };
		vertexList[17] = Vertex_PCNT{ XMFLOAT4{0.5f * _width, 0.5f * _height, 0.5f * _depth, 1.0f}, XMFLOAT4{0.5f, 0.5f, 0.5f, 1.0f}, XMFLOAT4{0.0f, 1.0f, 0.0f, 1.0f}, XMFLOAT2{0.5f, 0.0f} };
		vertexList[18] = Vertex_PCNT{ XMFLOAT4{0.5f * _width, 0.5f * _height, -0.5f * _depth, 1.0f}, XMFLOAT4{0.5f, 0.5f, 0.5f, 1.0f}, XMFLOAT4{0.0f, 1.0f, 0.0f, 1.0f}, XMFLOAT2{0.5f, 0.5f} };
		vertexList[19] = Vertex_PCNT{ XMFLOAT4{-0.5f * _width, 0.5f * _height, -0.5f * _depth, 1.0f}, XMFLOAT4{0.5f, 0.5f, 0.5f, 1.0f}, XMFLOAT4{0.0f, 1.0f, 0.0f, 1.0f}, XMFLOAT2{0.0f, 0.5f} };

		// ¾Æ·§¸é
		vertexList[20] = Vertex_PCNT{ XMFLOAT4{-0.5f * _width, -0.5f * _height, -0.5f * _depth, 1.0f}, XMFLOAT4{0.0f, 0.5f, 0.5f, 1.0f}, XMFLOAT4{0.0f, -1.0f, 0.0f, 1.0f}, XMFLOAT2{0.0f, 0.0f} };
		vertexList[21] = Vertex_PCNT{ XMFLOAT4{0.5f * _width, -0.5f * _height, -0.5f * _depth, 1.0f}, XMFLOAT4{0.0f, 0.5f, 0.5f, 1.0f}, XMFLOAT4{0.0f, -1.0f, 0.0f, 1.0f}, XMFLOAT2{0.5f, 0.0f} };
		vertexList[22] = Vertex_PCNT{ XMFLOAT4{0.5f * _width, -0.5f * _height, 0.5f * _depth, 1.0f}, XMFLOAT4{0.0f, 0.5f, 0.5f, 1.0f}, XMFLOAT4{0.0f, -1.0f, 0.0f, 1.0f}, XMFLOAT2{0.5f, 0.5f} };
		vertexList[23] = Vertex_PCNT{ XMFLOAT4{-0.5f * _width, -0.5f * _height, 0.5f * _depth, 1.0f}, XMFLOAT4{0.0f, 0.5f, 0.5f, 1.0f}, XMFLOAT4{0.0f, -1.0f, 0.0f, 1.0f}, XMFLOAT2{0.0f, 0.5f} };

		SetVertexList(vertexList);

		std::vector<DWORD> indexList;
		indexList.resize(36);
		int iIndex = 0;
		indexList[iIndex++] = 0; 	indexList[iIndex++] = 1; 	indexList[iIndex++] = 2; 	indexList[iIndex++] = 0;	indexList[iIndex++] = 2; 	indexList[iIndex++] = 3;
		indexList[iIndex++] = 4; 	indexList[iIndex++] = 5; 	indexList[iIndex++] = 6; 	indexList[iIndex++] = 4;	indexList[iIndex++] = 6; 	indexList[iIndex++] = 7;
		indexList[iIndex++] = 8; 	indexList[iIndex++] = 9; 	indexList[iIndex++] = 10; indexList[iIndex++] = 8;	indexList[iIndex++] = 10; indexList[iIndex++] = 11;
		indexList[iIndex++] = 12; indexList[iIndex++] = 13; indexList[iIndex++] = 14; indexList[iIndex++] = 12;	indexList[iIndex++] = 14; indexList[iIndex++] = 15;
		indexList[iIndex++] = 16; indexList[iIndex++] = 17; indexList[iIndex++] = 18; indexList[iIndex++] = 16;	indexList[iIndex++] = 18; indexList[iIndex++] = 19;
		indexList[iIndex++] = 20; indexList[iIndex++] = 21; indexList[iIndex++] = 22; indexList[iIndex++] = 20;	indexList[iIndex++] = 22; indexList[iIndex++] = 23;

		SetIndexList(indexList);
	}
	Triangle::Triangle()
	{
	}
	void Triangle::Build()
	{
		std::vector<Vertex_PCNT> vertexList;
		vertexList.resize(3);
		vertexList[0] = Vertex_PCNT{ XMFLOAT4{+0.0f, +0.5f, 0.5f, 1.0f}, XMFLOAT4{1.0f, 0.0f, 0.0f, 0.0f}, XMFLOAT4{0.0f, 0.0f, -1.0f, 1.0f}, XMFLOAT2{0, 0} };
		vertexList[1] = Vertex_PCNT{ XMFLOAT4{+0.5f, -0.5f, 0.5f, 1.0f}, XMFLOAT4{0.0f, 1.0f, 0.0f, 0.0f}, XMFLOAT4{0.0f, 0.0f, -1.0f, 1.0f}, XMFLOAT2{0, 0} };
		vertexList[2] = Vertex_PCNT{ XMFLOAT4{-0.5f, -0.5f, 0.5f, 1.0f}, XMFLOAT4{0.0f, 0.0f, 1.0f, 0.0f}, XMFLOAT4{0.0f, 0.0f, -1.0f, 1.0f}, XMFLOAT4{0, 0} };

		SetVertexList(vertexList);

		std::vector<DWORD> indexList;
		indexList.resize(3);
		int iIndex = 0;
		indexList[iIndex++] = 0;
		indexList[iIndex++] = 1;
		indexList[iIndex++] = 2;

		SetIndexList(indexList);
	}
	*/
}