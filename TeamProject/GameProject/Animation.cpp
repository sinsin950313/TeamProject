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
		m_fAnimTime += g_fSecondPerFrame * _framePerSecond;

		int prevIndex;
		int nextIndex;
		float t;
		if (_isLoop)
		{
			while (m_fAnimTime > _data.size() - 1)
			{
				m_fAnimTime -= _data.size() - 1;
			}

			prevIndex = m_fAnimTime;
			nextIndex = prevIndex + 1;
		}
		else
		{
			if (m_fAnimTime > _data.size() - 1)
			{
				m_fAnimTime = _data.size() - 1;
			}

			prevIndex = m_fAnimTime;
			nextIndex = prevIndex;
		}
		if (nextIndex == _data.size())
		{
			nextIndex -= 1;
		}
		t = m_fAnimTime - prevIndex;

		for (int i = 0; i < _boneAnimationUnitMaxCount; ++i)
		{
			_frameMatrixInfo.BoneMatrix[i] = GetInterpolate(_data[prevIndex]->BoneAnimationUnit[i], _data[nextIndex]->BoneAnimationUnit[i], t);
			_frameMatrixInfo.BoneMatrix[i] = _frameMatrixInfo.BoneMatrix[i].Transpose();
		}
		for (int i = 0; i < _meshAnimationUnitMaxCount; ++i)
		{
			_frameMatrixInfo.MeshMatrix[i] = GetInterpolate(_data[prevIndex]->MeshAnimationUnit[i], _data[nextIndex]->MeshAnimationUnit[i], t);
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
	void Animation::Initialize_SetLoop(bool loop)
	{
		_isLoop = loop;
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
	bool Animation::PreRender()
	{
		m_pImmediateContext->VSSetConstantBuffers(3, 1, &_animatedFrameBuffer);

		return true;
	}
	bool Animation::Render()
	{
		m_pImmediateContext->VSSetConstantBuffers(3, 1, &_animatedFrameBuffer);

		return true;
	}
	bool Animation::PostRender()
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

		if (_currentFrameInfo != nullptr)
		{
			delete _currentFrameInfo;
			_currentFrameInfo = nullptr;
		}

		if (_animatedFrameBuffer)
		{
			_animatedFrameBuffer->Release();
			_animatedFrameBuffer = nullptr;
		}

		return true;
	}
	std::string Animation::Serialize()
	{
		std::string ret;
		return ret;
	}
	void Animation::Deserialize(const char* buffer, int size, int& offset)
	{
		// FramePerSecond
		{
			memcpy(&_framePerSecond, buffer + offset, sizeof(float));
			offset += sizeof(float);
		}

		// BoneAnimationUnitMaxCount
		{
			memcpy(&_boneAnimationUnitMaxCount, buffer + offset, sizeof(int));
			offset += sizeof(int);
		}

		// MeshAnimationUnitMaxCount;
		{
			memcpy(&_meshAnimationUnitMaxCount, buffer + offset, sizeof(int));
			offset += sizeof(int);
		}

		// Frame Data
		{
			int frameDataCount;
			memcpy(&frameDataCount, buffer + offset, sizeof(int));
			offset += sizeof(int);

			_data.resize(frameDataCount);
			for (int i = 0; i < frameDataCount; ++i)
			{
				_data[i] = new AnimationFrameInfo;
				for (int j = 0; j < _boneAnimationUnitMaxCount; ++j)
				{
					{
						XMFLOAT4X4 matrix;
						memcpy(&matrix, buffer + offset, sizeof(XMFLOAT4X4));
						offset += sizeof(XMFLOAT4X4);

						_data[i]->BoneAnimationUnit[j].Matrix = matrix;
					}
					{
						XMFLOAT3 translate;
						memcpy(&translate, buffer + offset, sizeof(XMFLOAT3));
						offset += sizeof(XMFLOAT3);
						_data[i]->BoneAnimationUnit[j].Translate = translate;
					}
					{
						XMFLOAT3 scale;
						memcpy(&scale, buffer + offset, sizeof(XMFLOAT3));
						offset += sizeof(XMFLOAT3);
						_data[i]->BoneAnimationUnit[j].Scale = scale;
					}
					{
						XMFLOAT4 rotate;
						memcpy(&rotate, buffer + offset, sizeof(XMFLOAT4));
						offset += sizeof(XMFLOAT4);
						_data[i]->BoneAnimationUnit[j].Rotate = {
							rotate.x, rotate.y, rotate.z, rotate.w
						};
					}
				}

				for (int j = 0; j < _meshAnimationUnitMaxCount; ++j)
				{
					{
						XMFLOAT4X4 matrix;
						memcpy(&matrix, buffer + offset, sizeof(XMFLOAT4X4));
						offset += sizeof(XMFLOAT4X4);

						_data[i]->MeshAnimationUnit[j].Matrix = matrix;
					}
					{
						XMFLOAT3 translate;
						memcpy(&translate, buffer + offset, sizeof(XMFLOAT3));
						offset += sizeof(XMFLOAT3);
						_data[i]->MeshAnimationUnit[j].Translate = translate;
					}
					{
						XMFLOAT3 scale;
						memcpy(&scale, buffer + offset, sizeof(XMFLOAT3));
						offset += sizeof(XMFLOAT3);
						_data[i]->MeshAnimationUnit[j].Scale = scale;
					}
					{
						XMFLOAT4 rotate;
						memcpy(&rotate, buffer + offset, sizeof(XMFLOAT4));
						offset += sizeof(XMFLOAT4);
						_data[i]->MeshAnimationUnit[j].Rotate = {
							rotate.x, rotate.y, rotate.z, rotate.w
						};
					}
				}
			}
		}

		// Start Frame
		{
			memcpy(&_startFrame, buffer + offset, sizeof(int));
			offset += sizeof(int);
		}

		// End Frame
		{
			memcpy(&_endFrame, buffer + offset, sizeof(int));
			offset += sizeof(int);
		}

		// Loop
		{
			int tmpLoop;
			memcpy(&tmpLoop, buffer + offset, sizeof(int));
			offset += sizeof(int);

			_isLoop = tmpLoop;
		}
	}
	TMatrix Animation::GetCurrentBoneMatrix(BoneIndex index)
	{
		if (_data.empty())
		{
			return XMFLOAT4X4();
		}

		return _frameMatrixInfo.BoneMatrix[index].Transpose();
	}
	void Animation::ResetAnimationTimer()
	{
		m_fAnimTime = 0;
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
