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
		Animation* _currentAnimation;

		Shader* _ps;

		OBBData _boundingVolume;

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

	public:
		void SetCurrentAnimation(AnimationName name);
		void SetVertexShader(Shader* shader);
		void SetPixelShader(Shader* shader);
		OBBData GetBoundingVolume();

	public:
		bool Init() override;
		bool Frame() override;
		bool Render() override;
		bool Release() override;
		std::string Serialize(int tabCount) { return ""; }
		void Deserialize(std::string& serialedString) override;
	};
}
