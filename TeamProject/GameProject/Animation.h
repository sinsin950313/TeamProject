#pragma once

#include "Common.h"
#include "TMath.h"
#include "Timer.h"
#include <vector>
#include <d3d11.h>
#include "Serializeable.h"
#include "SerializeableDataType.h"

namespace SSB
{
	typedef unsigned int FrameIndex;

	class DefaultAnimation
	{
	private:
		AnimationFrameInfo* _frameInfo;

	public:
		DefaultAnimation();
		~DefaultAnimation();

	public:
		const AnimationFrameInfo* GetAnimationFrameInfo() const;
	};

	static const DefaultAnimation kAnimationInitializer;

	class Animation : public Common, public Serializeable
	{
	private:
		struct FrameMatrixInfo
		{
			TMatrix BoneMatrix[255];
			TMatrix MeshMatrix[255];
		};
	private:
		float	m_fAnimTime = 0.0f;
		float _framePerSecond = 30;

		int _boneAnimationUnitMaxCount = 0;
		int _meshAnimationUnitMaxCount = 0;
		std::vector<AnimationFrameInfo*> _data;

		FrameIndex _startFrame;
		FrameIndex _endFrame;

		AnimationFrameInfo* _currentFrameInfo;
		FrameMatrixInfo _frameMatrixInfo;
		ID3D11Buffer* _animatedFrameBuffer;

	public:
		Animation();
		~Animation();

	private:
		bool CreateAnimatedFrameInfoBuffer();
		TMatrix GetInterpolate(AnimationUnitInfo& beforeInfo, AnimationUnitInfo& afterInfo, float t);
		void UpdateFrameInfo();

	public:
		void Initialize_SetAnimationFrameData(std::vector<AnimationFrameInfo*> data);
		void Initialize_SetFrameInterval(FrameIndex start, FrameIndex end);
		void Initialize_SetAnimationUnitMaximumCount(int boneCount, int meshCount);

	public:
		bool Init() override;
		bool Frame() override;
		bool Render() override;
		bool Release() override;
		std::string Serialize(int tabCount) override;
		void Deserialize(std::string& serialedString) override;
	};
}
