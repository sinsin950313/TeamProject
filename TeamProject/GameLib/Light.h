#pragma once

#include <DirectXMath.h>
#include <d3d11.h>

namespace SSB
{
	class Light
	{
	private:
		ID3D11Device* _device;
		ID3D11DeviceContext* _dc;

	private:
		DirectX::XMFLOAT4X4 _worldMatrix;
		DirectX::XMFLOAT4X4 _viewMatrix;
		DirectX::XMFLOAT4X4 _projMatrix;
		DirectX::XMFLOAT4 _color;

	private:
		ID3D11Texture2D* _depthMap;
		ID3D11ShaderResourceView* _depthMapShaderResourceView;
		ID3D11DepthStencilView* _depthMapDepthStencilView;
		ID3D11Buffer* _lightBuffer;
		ID3D11PixelShader* _depthMapTexturePixelShader;
		ID3D11PixelShader* _lightingPixelShader;

	private:
		void UpdateDepthMap();
		void UpdateLightBuffer();
		void RegisterLightingPixelShader();

	protected:
		virtual ID3D11PixelShader* GetLightingPixelShader() = 0;

	public:
		void Initialize_SetDevice(ID3D11Device* device, ID3D11DeviceContext* dc);
		void SetLightDirection(DirectX::XMFLOAT3 direction);
		void SetLightPosition(DirectX::XMFLOAT3 position);
		void SetLightColor(DirectX::XMFLOAT3 color);

	public:
		void Init();
		void Frame();
		void PreRender();
		void Render();
		void Release();
	};
}
