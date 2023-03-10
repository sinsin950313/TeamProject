#pragma once

#include "TMath.h"

namespace SSB
{
	struct Vertex_PC
	{
		XMFLOAT4 Position;
		XMFLOAT4 Color;
	};

	struct Vertex_PCNT : public Vertex_PC
	{
		XMFLOAT4 Normal;
		XMFLOAT2 TextureUV;
	};

	struct MeshData
	{
		int MeshIndex;
		int Padding[3];
	};

	static const int kAffectedBoneCount = 4;
	struct Vertex_PCNT_Skinning : public Vertex_PCNT
	{
		int AffectedBoneIndex[kAffectedBoneCount];
		float Weight[kAffectedBoneCount];
	};

	struct Vertex_PCNTs : public Vertex_PCNT
	{
		unsigned int MaterialIndex;
	};

	struct Vertex_PCNTs_Skinning : public Vertex_PCNT
	{
		int AffectedBoneIndex[kAffectedBoneCount];
		float Weight[kAffectedBoneCount];
		unsigned int MaterialIndex;
	};

	static const int kMaximumBoneCount = 255;
	struct MeshToBoneSpaceTransformData
	{
		TMatrix BoneSpaceTransformBuffer[kMaximumBoneCount];
		int MeshIndex;
		float MeshWeight;
		int Padding[2];
	};

	static const int kAnimationUnitMaxIndex = 255;
	struct AnimationUnitInfo
	{
		TMatrix Matrix;
		TVector3 Translate;
		float Padding1 = 0;
		TVector3 Scale{ 1, 1, 1 };
		float Padding2 = 0;
		TQuaternion Rotate;
	};

	struct AnimationFrameInfo
	{
		AnimationUnitInfo BoneAnimationUnit[kAnimationUnitMaxIndex];
		AnimationUnitInfo MeshAnimationUnit[kAnimationUnitMaxIndex];
	};
}