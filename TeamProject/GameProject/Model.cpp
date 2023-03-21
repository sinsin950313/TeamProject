#include "Model.h"
#include "ShaderMgr.h"
#include "Mesh.hpp"
#include "CommonPath.h"

namespace SSB
{
	Animation Model::DefaultAnimation;
	bool Model::DefaultAnimationInitialized = false;

	Model::Model()
	{
	}
	Model::~Model()
	{
		Release();
	}
	void Model::SizeCheck()
	{
		float minX = (std::numeric_limits<float>::max)();
		float minY = (std::numeric_limits<float>::max)();
		float minZ = (std::numeric_limits<float>::max)();
		float maxX = (std::numeric_limits<float>::min)();
		float maxY = (std::numeric_limits<float>::min)();
		float maxZ = (std::numeric_limits<float>::min)();
		for (auto vertex : _meshElementMinMaxVertexList)
		{
			minX = min(minX, vertex.x);
			minY = min(minY, vertex.y);
			minZ = min(minZ, vertex.z);

			maxX = max(maxX, vertex.x);
			maxY = max(maxY, vertex.y);
			maxZ = max(maxZ, vertex.z);
		}

		_minVertex = { minX, minY, minZ };
		_maxVertex = { maxX, maxY, maxZ };
	}
	void Model::Initialize_RegisterMesh(MeshIndex index, MeshInterface* mesh)
	{
		_meshes.insert(std::make_pair(index, mesh));
	}
	void Model::Initialize_RegisterMaterial(MaterialIndex index, Material* material)
	{
		_materials.insert(std::make_pair(index, material));
	}
	void Model::Initialize_RegisterAnimation(AnimationName name, Animation* animation)
	{
		_animations.insert(std::make_pair(name, animation));
	}
	void Model::Initialize_SetBoundingVolume(OBBData data)
	{
		_boundingVolume = data;
	}
	void Model::Initialize_RegisterSocket(SocketName name, BoneIndex index)
	{
		_sockets.insert(std::make_pair(name, index));
	}
	void Model::SetCurrentAnimation(AnimationName name)
	{
		if (_animations.find(name) != _animations.end())
		{
			if (_currentAnimation != _animations.find(name)->second)
			{
				_currentAnimation = _animations.find(name)->second;
				_currentAnimation->ResetAnimationTimer();
			}
		}
		else
		{
			_currentAnimation = &DefaultAnimation;
		}
	}
	void Model::SetVertexShader(Shader* shader)
	{
		for (auto mesh : _meshes)
		{
			mesh.second->SetShader(shader);
		}
	}
	void Model::SetPixelShader(Shader* shader)
	{
		_ps = shader;
	}
	TMatrix Model::GetSocketCurrentMatrix(SocketName name)
	{
		if (_sockets.find(name) != _sockets.end())
		{
			return _currentAnimation->GetCurrentBoneMatrix(_sockets.find(name)->second);
		}
		return TMatrix();
	}
	OBBData Model::GetBoundingVolume()
	{
		return _boundingVolume;
	}
	bool Model::Init()
	{
		if (!DefaultAnimationInitialized)
		{
			DefaultAnimation.SetDevice(m_pd3dDevice, m_pImmediateContext);
			DefaultAnimation.Init();
			DefaultAnimationInitialized = true;
		}
		_currentAnimation = &DefaultAnimation;

		for (auto mesh : _meshes)
		{
			_meshElementMinMaxVertexList.push_back(mesh.second->GetMaxVertex());
			_meshElementMinMaxVertexList.push_back(mesh.second->GetMinVertex());
		}
		SizeCheck();

		return false;
	}
	bool Model::Frame()
	{
		_currentAnimation->Frame();

		for (auto material : _materials)
		{
			material.second->Frame();
		}

		for (auto mesh : _meshes)
		{
			mesh.second->Frame();
		}

		return false;
	}
	bool Model::Render()
	{
		_currentAnimation->Render();

		m_pImmediateContext->PSSetShader(_ps->m_pPS, NULL, 0);

		for (auto material : _materials)
		{
			material.second->Render();
		}

		for (auto mesh : _meshes)
		{
			mesh.second->Render();
		}

		return false;
	}
	bool Model::Release()
	{
		for (auto material : _materials)
		{
			material.second->Release();
			delete material.second;
		}
		_materials.clear();

		_currentAnimation = &DefaultAnimation;
		for (auto animation : _animations)
		{
			if (animation.second != &DefaultAnimation)
			{
				animation.second->Release();
				delete animation.second;
			}
		}
		_animations.clear();

		for (auto mesh : _meshes)
		{
			mesh.second->Release();
			delete mesh.second;
		}
		_meshes.clear();
		_meshElementMinMaxVertexList.clear();

		return false;
	}
	void Model::Deserialize(std::string& serialedString)
	{
		serialedString = GetUnitObject(serialedString, 0).str;

		int offset = 0;
		{
			offset = serialedString.find(_minVertexStr);
			auto data = GetUnitElement(serialedString, offset);
			offset = data.offset;
			XMFLOAT3 tmp;
			Serializeable::Deserialize(data.str, tmp);
			_minVertex = tmp;
		}

		{
			offset = serialedString.find(_maxVertexStr);
			auto data = GetUnitElement(serialedString, offset);
			offset = data.offset;
			XMFLOAT3 tmp;
			Serializeable::Deserialize(data.str, tmp);
			_maxVertex = tmp;
		}

		{
			offset = serialedString.find(_socketDataStr);
			auto data = GetUnitElement(serialedString, offset);
			offset = data.offset;
			int socketCount;
			Serializeable::Deserialize(data.str, socketCount);
			for (int i = 0; i < socketCount; ++i)
			{
				auto socketNameData = GetUnitAtomic(serialedString, offset);
				offset = socketNameData.offset;
				auto boneIndexData = GetUnitAtomic(serialedString, offset);
				offset = boneIndexData.offset;

				_sockets.insert(std::make_pair(socketNameData.str, std::stoi(boneIndexData.str)));
			}
		}

		offset = serialedString.find(_boundingVolumeStr);
		{
			auto data = GetUnitElement(serialedString, offset);
			offset = data.offset;
			XMFLOAT3 pos;
			Serializeable::Deserialize(data.str, pos);

			_boundingVolume.Position = pos;
		}

		{
			auto data = GetUnitElement(serialedString, offset);
			offset = data.offset;
			XMFLOAT3X3 rot;
			Serializeable::Deserialize(data.str, rot);

			_boundingVolume.Rotation = rot;
		}

		{
			auto data = GetUnitElement(serialedString, offset);
			offset = data.offset;
			XMFLOAT3 scale;
			Serializeable::Deserialize(data.str, scale);

			_boundingVolume.Scale = scale;
		}

		{
			auto data = GetUnitElement(serialedString, offset);
			offset = data.offset;
			XMFLOAT3 tmp;
			Serializeable::Deserialize(data.str, tmp);

			_boundingVolume.Width = tmp.x;
			_boundingVolume.Height = tmp.y;
			_boundingVolume.Depth = tmp.z;
		}

		while (serialedString.find(_materialIndexStr, offset) != std::string::npos)
		{
			int materialIndex;
			{
				offset = serialedString.find(_materialIndexStr, offset);
				auto data = GetUnitElement(serialedString, offset);
				offset = data.offset;
				Serializeable::Deserialize(data.str, materialIndex);
			}

			auto objectData = GetUnitObject(serialedString, offset);
			std::string objectStr = objectData.str;
			offset = objectData.offset;
			Material* mat = new Material;
			mat->SetDevice(m_pd3dDevice, m_pImmediateContext);
			mat->Deserialize(objectStr);
			mat->Init();
			_materials.insert(std::make_pair(materialIndex, mat));
		}

		while (serialedString.find(_meshIndexStr, offset) != std::string::npos)
		{
			int meshIndex;
			{
				offset = serialedString.find(_meshIndexStr, offset);
				auto data = GetUnitElement(serialedString, offset);
				offset = data.offset;
				Serializeable::Deserialize(data.str, meshIndex);
			}

			auto objectData = GetUnitObject(serialedString, offset);
			std::string objectStr = objectData.str;

			std::string vertexType;
			{
				int tmpOffset = serialedString.find(_vertexTypeStr, offset);
				std::string tmp = GetUnitElement(serialedString, tmpOffset).str;
				vertexType = GetUnitAtomic(tmp, 0).str;
			}
			offset = objectData.offset;

			MeshInterface* mesh;
			if (vertexType == Vertex_PCNT_Keyword)
			{
				mesh = new Mesh_Vertex_PCNT;
			}
			else if (vertexType == Vertex_PCNT_Animatable_Keyword)
			{
				mesh = new Mesh_Vertex_PCNT_Animatable;
			}
			else if (vertexType == Vertex_PCNT_Skinning_Keyword)
			{
				mesh = new Mesh_Vertex_PCNT_Skinning;
			}
			else if (vertexType == Vertex_PCNTs_Keyword)
			{
				mesh = new Mesh_Vertex_PCNTs;
			}
			else if (vertexType == Vertex_PCNTs_Animatable_Keyword)
			{
				mesh = new Mesh_Vertex_PCNTs_Animatable;
			}
			else if (vertexType == Vertex_PCNTs_Skinning_Keyword)
			{
				mesh = new Mesh_Vertex_PCNTs_Skinning;
			}
			//if (vertexType == Vertex_PC_Keyword)
			else
			{
				mesh = new Mesh_Vertex_PC;
			}

			mesh->SetDevice(m_pd3dDevice, m_pImmediateContext);
			mesh->Deserialize(objectStr);
			mesh->Init();
			
			_meshes.insert(std::make_pair(meshIndex, mesh));
		}

		while (serialedString.find(_animationNameStr, offset) != std::string::npos)
		{
			AnimationName animationName;
			{
				offset = serialedString.find(_animationNameStr, offset);
				auto atomic = GetUnitElement(serialedString, offset);
				offset = atomic.offset;
				animationName = std::string(GetUnitAtomic(atomic.str, 0).str);
			}

			auto objectData = GetUnitObject(serialedString, offset);
			std::string objectStr = objectData.str;
			offset = objectData.offset;
			Animation* anim = new Animation;
			anim->SetDevice(m_pd3dDevice, m_pImmediateContext);
			anim->Deserialize(objectStr);
			anim->Init();
			_animations.insert(std::make_pair(animationName, anim));
		}

		{
			offset = serialedString.find(_pixelShaderStr, offset);
			auto atomicData = GetUnitElement(serialedString, offset);
			std::string atomic = atomicData.str;
			I_Shader.PSLoad(kShaderPath + mtw(GetUnitAtomic(atomic, 0).str), L"PS", &_ps);
		}
	}
}
