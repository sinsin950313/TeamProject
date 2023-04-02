#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
#include "TMath.h"
#include "Shader.h"

namespace SSB
{
	class Light
	{
	private:
		ID3D11Device* _device;
		ID3D11DeviceContext* _dc;

	private:
		TVector3	m_vPos = TVector3::Zero;
		TVector3	m_vLookAt = TVector3::Zero;
		TVector3	m_vScale = TVector3::One;

		struct LightDataForDepth
		{
			TMatrix WorldMatrix;
			TMatrix ViewMatrix;
			TMatrix ProjMatrix;
		};
		LightDataForDepth _lightDataForDepth;

		struct LightDataForRender
		{
			TVector4 Color;
		};
		LightDataForRender _lightDataForRender;

	private:
		ID3D11Texture2D* _shadowDepthMap;
		ID3D11ShaderResourceView* _shadowDepthMapShaderResourceView;
		ID3D11DepthStencilView* _shadowDepthMapDepthStencilView;
		ID3D11Buffer* _lightBufferForDepth;
		ID3D11Buffer* _lightBufferForRender;
		Shader* _lightingShader = nullptr;

	private:
		void CreateDepthMap();
		void CreateLightData();
		bool CreateLightBuffer();

	private:
		void UpdateLightBuffer();
		void RegisterLightingVertexShader();

	protected:
		virtual Shader* GetLightingShader() = 0;

	public:
		void Initialize_SetDevice(ID3D11Device* device, ID3D11DeviceContext* dc);
		void SetLightDirection(TVector3 direction);
		void SetLightPosition(TVector3 position);
		void SetLightColor(TVector4 color);

	public:
		void Init();
		void Frame();
		void PreRender();
		void Render();
		void Release();
	};
}
