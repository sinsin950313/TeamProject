#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
#include "TMath.h"
#include "Shader.h"

namespace SSB
{
	class Light
	{
	protected:
		ID3D11Device* _device;
		ID3D11DeviceContext* _dc;

	public:
		TVector3	m_vPos = TVector3::Zero;
		TVector3	m_vLookAt = TVector3::Zero;
		TVector3	m_vScale = TVector3::One;

	protected:
		struct LightLocationData
		{
			TMatrix WorldMatrix;
			TMatrix ViewMatrix;
			TMatrix ProjMatrix;
		};
	private:
		LightLocationData _lightDataForDepth;

		struct LightDataForRender
		{
			LightLocationData LocationData;
			TVector4 Color;
		};
		LightDataForRender _lightDataForRender;
		TVector4 _lightColor;

	private:
		ID3D11Texture2D* _shadowDepthMap;
		ID3D11ShaderResourceView* _shadowDepthMapShaderResourceView;
		ID3D11DepthStencilView* _shadowDepthMapDepthStencilView;
		ID3D11Buffer* _lightBufferForDepth;
		ID3D11Buffer* _lightBufferForRender;
		ID3D11Texture2D* _renderTargetTexture;
		ID3D11RenderTargetView* _renderTargetView;

	private:
		void CreateDepthMap();
		void CreateLightData();
		bool CreateLightBuffer();
		HRESULT CreateRenderTargetData();

	protected:
		virtual LightLocationData UpdateLightData() = 0;
	private:
		void UpdateLightBuffer();

	protected:
		ID3D11GeometryShader* m_pGS = nullptr;
		ID3DBlob* m_pGSCode = nullptr;

		ID3D11PixelShader* m_pPS = nullptr;
		ID3DBlob* m_pPSCode = nullptr;
		virtual HRESULT CreateLightingShader() = 0;

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

	private:
		struct ToViewSpaceTransformData
		{
			TMatrix matWorld;
			TMatrix matView;
			TMatrix matProj;
		};
		ToViewSpaceTransformData _toViewSpaceTransformData;
		ID3D11Buffer* _toViewSpaceTransformBuffer;

		struct ObjectToWorldTransformData
		{
			TMatrix World;
		};
		ObjectToWorldTransformData _objectToWorldTransformData;
		ID3D11Buffer* _objectToWorldTransformBuffer;

	public:
		TMatrix		m_matWorld = TMatrix::Identity;
		TMatrix		m_matView = TMatrix::Identity;
		TMatrix		m_matProj = TMatrix::Identity;

	private:
		void	CreateCameraBuffer();
		void	UpdateCameraBuffer();

	public:
		void	SetMatrix(TMatrix* matWorld, TMatrix* matView, TMatrix* matProj);
	};
}
