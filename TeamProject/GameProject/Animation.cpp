#include "Animation.h"
namespace SSB
{
	Animation::Animation()
	{
		_currentFrameInfo = new AnimationFrameInfo;
	}
	Animation::~Animation()
	{
		Release();
		if (_currentFrameInfo != nullptr)
		{
			delete _currentFrameInfo;
			_currentFrameInfo = nullptr;
		}
	}
	bool Animation::CreateAnimatedFrameInfoBuffer()
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = sizeof(FrameMatrixInfo);
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));
		sd.pSysMem = &_frameMatrixInfo;
		HRESULT hr = m_pd3dDevice->CreateBuffer(&bd, &sd, &_animatedFrameBuffer);
		if (FAILED(hr))
		{
			assert(SUCCEEDED(hr));
			return false;
		}
        return true;
	}
	TMatrix Animation::GetInterpolate(AnimationUnitInfo& beforeInfo, AnimationUnitInfo& afterInfo, float t)
	{
		TMatrix ret;

		TVector3 pos;
		D3DXVec3Lerp(&pos, &beforeInfo.Translate, &afterInfo.Translate, t);
		TVector3 scale;
		D3DXVec3Lerp(&scale, &beforeInfo.Scale, &afterInfo.Scale, t);
		TQuaternion rotate;
		D3DXQuaternionSlerp(&rotate, &beforeInfo.Rotate, &afterInfo.Rotate, t);

		//ret = TMatrix::Scale(scale) * TMatrix(rotate.GetRotateMatrix(), { 0, 0, 0 }) * TMatrix::Translate(pos);
		D3DXMatrixAffineTransformation(&ret, &scale, NULL, &rotate, &pos);

		return ret;
	}
	void Animation::UpdateFrameInfo()
	{
		asdf;
		float animationElapseTime = (float)(g_fSecondPerFrame / 1000.0f);
		int beforeIndex = animationElapseTime * _framePerSecond;
		int afterIndex = beforeIndex + 1;
		if (afterIndex == _data.size())
		{
			afterIndex = beforeIndex;
		}
		else if (_data.size() <= beforeIndex)
		{
			beforeIndex = beforeIndex % _data.size();
			afterIndex = afterIndex % _data.size();
		}

		float beforeTime = beforeIndex / _framePerSecond;
		float afterTime = afterIndex / _framePerSecond;
		float t = (animationElapseTime - beforeTime) / (afterTime - beforeTime);
		if (afterTime - beforeTime < 0.001f)
		{
			t = 0;
		}

		for (int i = 0; i < _boneAnimationUnitMaxCount; ++i)
		{
			_frameMatrixInfo.BoneMatrix[i] = GetInterpolate(_data[beforeIndex]->BoneAnimationUnit[i], _data[afterIndex]->BoneAnimationUnit[i], t);
			_frameMatrixInfo.BoneMatrix[i] = _frameMatrixInfo.BoneMatrix[i].Transpose();
		}
		for (int i = 0; i < _meshAnimationUnitMaxCount; ++i)
		{
			_frameMatrixInfo.MeshMatrix[i] = GetInterpolate(_data[beforeIndex]->MeshAnimationUnit[i], _data[afterIndex]->MeshAnimationUnit[i], t);
			_frameMatrixInfo.MeshMatrix[i] = _frameMatrixInfo.MeshMatrix[i].Transpose();
		}
	}
	bool Animation::Init()
	{
		const AnimationFrameInfo* ptr = kAnimationInitializer.GetAnimationFrameInfo();
		memcpy(_currentFrameInfo, ptr, sizeof(AnimationFrameInfo));
		CreateAnimatedFrameInfoBuffer();

		return true;
	}
	void Animation::Initialize_SetAnimationFrameData(std::vector<AnimationFrameInfo*> data)
	{
		_data = data;
	}
	void Animation::Initialize_SetFrameInterval(FrameIndex start, FrameIndex end)
	{
		_startFrame = start;
		_endFrame = end;
	}
	void Animation::Initialize_SetAnimationUnitMaximumCount(int boneCount, int meshCount)
	{
		_boneAnimationUnitMaxCount = boneCount;
		_meshAnimationUnitMaxCount = meshCount;
	}
	bool Animation::Frame()
	{
		if (!_data.empty())
		{
			UpdateFrameInfo();
		}

		m_pImmediateContext->UpdateSubresource(_animatedFrameBuffer, 0, nullptr, &_frameMatrixInfo, 0, 0);

		return true;
	}
	bool Animation::Render()
	{
		m_pImmediateContext->VSSetConstantBuffers(3, 1, &_animatedFrameBuffer);

		return true;
	}
	bool Animation::Release()
	{
		for (auto pFrameInfo : _data)
		{
			delete pFrameInfo;
		}
		_data.clear();

		if (_animatedFrameBuffer)
		{
			_animatedFrameBuffer->Release();
			_animatedFrameBuffer = nullptr;
		}

		return true;
	}
	std::string Animation::Serialize(int tabCount)
	{
		std::string ret;
		return ret;
	}
	void Animation::Deserialize(std::string& serialedString)
	{
		auto data = GetUnitObject(serialedString, 0);
		serialedString = data.str;

		int offset = 0;
		{
			offset = serialedString.find(_framePerSecondStr, offset);
			auto data = GetUnitElement(serialedString, offset);
			std::string atomic = data.str;
			offset = data.offset;
			Serializeable::Deserialize(atomic, _framePerSecond);
		}

		{
			offset = serialedString.find(_boneAnimationUnitMaxCountStr, offset);
			auto data = GetUnitElement(serialedString, offset);
			std::string atomic = data.str;
			offset = data.offset;
			Serializeable::Deserialize(atomic, _boneAnimationUnitMaxCount);
		}

		{
			offset = serialedString.find(_meshAnimationUnitMaxCountStr, offset);
			auto data = GetUnitElement(serialedString, offset);
			std::string atomic = data.str;
			offset = data.offset;
			Serializeable::Deserialize(atomic, _meshAnimationUnitMaxCount);
		}

		{
			offset = serialedString.find(_startFrameStr, offset);
			auto data = GetUnitElement(serialedString, offset);
			std::string atomic = data.str;
			offset = data.offset;
			Serializeable::Deserialize(atomic, _startFrame);
		}

		{
			offset = serialedString.find(_endFrameStr, offset);
			auto data = GetUnitElement(serialedString, offset);
			std::string atomic = data.str;
			offset = data.offset;
			Serializeable::Deserialize(atomic, _endFrame);
		}

		int maxFrameCount;
		offset = serialedString.find(_frameInfoPerSecondStr, offset);
		{
			offset = serialedString.find(_maxFrameCountStr, offset);
			auto data = GetUnitElement(serialedString, offset);
			std::string atomic = data.str;
			offset = data.offset;
			Serializeable::Deserialize(atomic, maxFrameCount);
		}

		_data.resize(maxFrameCount);

		{
			for (int i = 0; i < maxFrameCount; ++i)
			{
				{
					offset = serialedString.find(_frameStr, offset);
					auto elemData = GetUnitElement(serialedString, offset);
					std::string elem = elemData.str;
					offset = elemData.offset;
				}

				auto data = GetUnitElement(serialedString, offset);
				offset = data.offset;
				AnimationFrameInfo* frameInfo = new AnimationFrameInfo;
				Serializeable::Deserialize(data.str, *frameInfo);
				_data[i] = frameInfo;
			}
		}
	}
	DefaultAnimation::DefaultAnimation()
	{
		_frameInfo = new AnimationFrameInfo;
		for (int i = 0; i < kAnimationUnitMaxIndex; ++i)
		{
			_frameInfo->BoneAnimationUnit[i].Matrix = TMatrix();
			_frameInfo->BoneAnimationUnit[i].Rotate = TQuaternion();
			_frameInfo->BoneAnimationUnit[i].Scale = TVector3::One;
			_frameInfo->BoneAnimationUnit[i].Translate = TVector3();

			_frameInfo->MeshAnimationUnit[i].Matrix = TMatrix();
			_frameInfo->MeshAnimationUnit[i].Rotate = TQuaternion();
			_frameInfo->MeshAnimationUnit[i].Scale = TVector3::One;
			_frameInfo->MeshAnimationUnit[i].Translate = TVector3();
		}
	}
	DefaultAnimation::~DefaultAnimation()
	{
		delete _frameInfo;
		_frameInfo = nullptr;
	}
	const AnimationFrameInfo* DefaultAnimation::GetAnimationFrameInfo() const
	{
		return _frameInfo;
	}
}
