#pragma once

#include "Common.h"
#include "Mesh.h"
#include "Animation.h"
#include "Material.h"
#include <map>
#include "Shader.h"
#include "Serializeable.h"
#include "Animation.h"
#include "TMath.h"

namespace SSB
{
	typedef std::string SocketName;

	struct OBBData
	{
		TVector3 Position;
		TMatrix Rotation;
		TVector3 Scale;

		float Width;
		float Height;
		float Depth;
	};

	class Model : public Common, public Serializeable
	{
	private:
		static bool DefaultAnimationInitialized;
		static Animation DefaultAnimation;

	private:
		std::vector<TVector3> _meshElementMinMaxVertexList;
	public:
		TVector3 _minVertex;
		TVector3 _maxVertex;

	private:
		std::map<MaterialIndex, Material*> _materials;
		std::map<MeshIndex, MeshInterface*> _meshes;

		std::map<AnimationName, Animation*> _animations;
	public:
		Animation* _currentAnimation;
	private:

		Shader* _ps;

		OBBData _boundingVolume;

		std::map<SocketName, BoneIndex> _sockets;

	public:
		Model();
		virtual ~Model();

	public:
		void SizeCheck();

	public:
		void Initialize_RegisterMaterial(MaterialIndex index, Material* material);
		void Initialize_RegisterMesh(MeshIndex index, MeshInterface* mesh);
		void Initialize_RegisterAnimation(AnimationName name, Animation* animation);
		void Initialize_SetBoundingVolume(OBBData data);
		void Initialize_RegisterSocket(SocketName name, BoneIndex index);

	public:
		void SetCurrentAnimation(AnimationName name);
		void SetVertexShader(Shader* shader);
		void SetPixelShader(Shader* shader);
		TMatrix GetSocketCurrentMatrix(SocketName name);
		OBBData GetBoundingVolume();

	public:
		bool Init() override;
		bool Frame() override;
		bool PreRender();
		bool Render() override;
		bool RenderInstancing(UINT iNum);
		bool Release() override;
		std::string Serialize() override { return ""; }
		void Deserialize(const char* buffer, int size, int& offset) override;
	};
}
