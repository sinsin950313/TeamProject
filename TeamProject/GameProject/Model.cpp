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
	bool Model::PreRender()
	{
		_currentAnimation->PreRender();

		for (auto material : _materials)
		{
			material.second->PreRender();
		}

		for (auto mesh : _meshes)
		{
			mesh.second->PreRender();
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
	void Model::Deserialize(const char* buffer, int size, int& offset)
	{
		// Min Vertex
		{
			XMFLOAT3 tmp;
			memcpy(&tmp, buffer + offset, sizeof(XMFLOAT3));
			_minVertex = tmp;
			offset += sizeof(XMFLOAT3);
		}

		// Max Vertex
		{
			XMFLOAT3 tmp;
			memcpy(&tmp, buffer + offset, sizeof(XMFLOAT3));
			_maxVertex = tmp;
			offset += sizeof(XMFLOAT3);
		}

		// Material
		{
			int materialCount;
			memcpy(&materialCount, buffer + offset, sizeof(int));
			offset += sizeof(int);

			for (int i = 0; i < materialCount; ++i)
			{
				int materialIndex;
				memcpy(&materialIndex, buffer + offset, sizeof(int));
				offset += sizeof(int);

				Material* tmp = new Material;
				tmp->Deserialize(buffer, size, offset);
				tmp->SetDevice(m_pd3dDevice, m_pImmediateContext);
				tmp->Init();
				_materials.insert(std::make_pair(materialIndex, tmp));
			}
		}

		// Mesh
		{
			int meshCount;
			memcpy(&meshCount, buffer + offset, sizeof(int));
			offset += sizeof(int);

			for (int i = 0; i < meshCount; ++i)
			{
				int meshIndex;
				{
					memcpy(&meshIndex, buffer + offset, sizeof(int));
					offset += sizeof(int);
				}

				std::string vertexType;

				int vertexTypeSize;
				memcpy(&vertexTypeSize, buffer + offset, sizeof(int));
				offset += sizeof(int);
				{
					char* meshTypeStr = new char[vertexTypeSize];
					memcpy(meshTypeStr, buffer + offset, vertexTypeSize);
					offset += vertexTypeSize;

					vertexType = std::string(meshTypeStr, vertexTypeSize);
					delete meshTypeStr;
				}

				MeshInterface* tmp;
				if (vertexType == Vertex_PCNT_Keyword)
				{
					tmp = new Mesh_Vertex_PCNT;
				}
				else if (vertexType == Vertex_PCNT_Animatable_Keyword)
				{
					tmp = new Mesh_Vertex_PCNT_Animatable;
				}
				else if (vertexType == Vertex_PCNT_Skinning_Keyword)
				{
					tmp = new Mesh_Vertex_PCNT_Skinning;
				}
				else if (vertexType == Vertex_PCNTs_Keyword)
				{
					tmp = new Mesh_Vertex_PCNTs;
				}
				else if (vertexType == Vertex_PCNTs_Animatable_Keyword)
				{
					tmp = new Mesh_Vertex_PCNTs_Animatable;
				}
				else if (vertexType == Vertex_PCNTs_Skinning_Keyword)
				{
					tmp = new Mesh_Vertex_PCNTs_Skinning;
				}
				//if (vertexType == Vertex_PC_Keyword)
				else
				{
					tmp = new Mesh_Vertex_PC;
				}

				tmp->Deserialize(buffer, size, offset);
				tmp->SetDevice(m_pd3dDevice, m_pImmediateContext);
				tmp->Init();
				_meshes.insert(std::make_pair(meshIndex, tmp));
			}
		}

		// Animation
		{
			int animationCount;
			memcpy(&animationCount, buffer + offset, sizeof(int));
			offset += sizeof(int);

			for (int i = 0; i < animationCount; ++i)
			{
				Animation* tmp = new Animation;
				std::string animationName;
				{
					int animationNameSize;
					memcpy(&animationNameSize, buffer + offset, sizeof(int));
					offset += sizeof(int);

					char* animationNameBuffer = new char[animationNameSize];
					memcpy(animationNameBuffer, buffer + offset, animationNameSize);
					offset += animationNameSize;
					animationName = std::string(animationNameBuffer, animationNameSize);
					delete animationNameBuffer;
				}
				tmp->Deserialize(buffer, size, offset);
				tmp->SetDevice(m_pd3dDevice, m_pImmediateContext);
				tmp->Init();
				_animations.insert(std::make_pair(animationName, tmp));
			}
		}

		// Pixel Shader
		{
			int pixelShaderFileNameCount;
			memcpy(&pixelShaderFileNameCount, buffer + offset, sizeof(int));
			offset += sizeof(int);

			char* fileNameBuffer = new char[pixelShaderFileNameCount];
			memcpy(fileNameBuffer, buffer + offset, pixelShaderFileNameCount);
			offset += pixelShaderFileNameCount;
			std::string tmpFileName(fileNameBuffer, pixelShaderFileNameCount);
			std::wstring fileName = mtw(tmpFileName);
			I_Shader.PSLoad(kShaderPath + fileName, L"PS", &_ps);
			delete fileNameBuffer;
		}

		// Socket
		{
			int socketCount;
			memcpy(&socketCount, buffer + offset, sizeof(int));
			offset += sizeof(int);

			for (int i = 0; i < socketCount; ++i)
			{
				int socketNameCount;
				memcpy(&socketNameCount, buffer + offset, sizeof(int));
				offset += sizeof(int);

				char* socketNameBuffer = new char[socketNameCount];
				memcpy(socketNameBuffer, buffer + offset, socketNameCount);
				std::string socketName(socketNameBuffer, socketNameCount);
				offset += socketNameCount;

				int socketIndex;
				memcpy(&socketIndex, buffer + offset, sizeof(int));
				offset += sizeof(int);

				_sockets.insert(std::make_pair(socketName, socketIndex));
				delete socketNameBuffer;
			}
		}

		// Bounding Volume
		{
			// Position
			{
				XMFLOAT3 tmp;
				memcpy(&tmp, buffer + offset, sizeof(XMFLOAT3));
				offset += sizeof(XMFLOAT3);
				_boundingVolume.Position = tmp;
			}
			// Rotation
			{
				XMFLOAT3X3 tmp;
				memcpy(&tmp, buffer + offset, sizeof(XMFLOAT3X3));
				offset += sizeof(XMFLOAT3X3);
				_boundingVolume.Rotation = tmp;
			}
			// Scale
			{
				XMFLOAT3 tmp;
				memcpy(&tmp, buffer + offset, sizeof(XMFLOAT3));
				offset += sizeof(XMFLOAT3);
				_boundingVolume.Scale = tmp;
			}
			// Width, Height, Depth
			{
				XMFLOAT3 tmp;
				memcpy(&tmp, buffer + offset, sizeof(XMFLOAT3));
				offset += sizeof(XMFLOAT3);
				_boundingVolume.Width = tmp.x;
				_boundingVolume.Height = tmp.y;
				_boundingVolume.Depth = tmp.z;
			}
		}
	}
}
