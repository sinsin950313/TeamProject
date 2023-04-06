#include "Light.h"
#include "TMath.h"
#include <vector>
#include "DXState.h"

namespace SSB
{
	void Light::CreateDepthMap()
	{
		{
			D3D11_TEXTURE2D_DESC desc;
			ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
			desc.Width = 2048;
			desc.Height = 2048;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
			desc.SampleDesc.Count = 1;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;

			_device->CreateTexture2D(&desc, NULL, &_shadowDepthMap);
		}

		{
			D3D11_DEPTH_STENCIL_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
			desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipSlice = 0;

			_device->CreateDepthStencilView(_shadowDepthMap, &desc, &_shadowDepthMapDepthStencilView);
		}

		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
			desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipLevels = 1;

			_device->CreateShaderResourceView(_shadowDepthMap, &desc, &_shadowDepthMapShaderResourceView);
		}
	}
	void Light::CreateLightData()
	{
		{
			_lightDataForDepth.WorldMatrix
				= XMFLOAT4X4(
					1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0,
					0, 0, 0, 1
				);
		}

		{
			TVector3 lightPos(0, 0, 0);
			TVector3 lightLookAt(0, 0, 1);
			TVector3 lightUp(0, 1, 0);
			D3DXMatrixLookAtLH(&_lightDataForDepth.ViewMatrix, &lightPos, &lightLookAt, &lightUp);
		}

		{
			float fMaxViewDistance = 10000.0f;
			D3DXMatrixOrthoOffCenterLH(&_lightDataForDepth.ProjMatrix, -fMaxViewDistance / 2, fMaxViewDistance / 2, -fMaxViewDistance / 2, fMaxViewDistance / 2, 0.0f, 100.0f);
		}

		_lightDataForRender.Color = TVector4(1, 1, 1, 1);
	}
	bool Light::CreateLightBuffer()
	{
		{
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.ByteWidth = sizeof(LightLocationData);
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));
			sd.pSysMem = &_lightDataForDepth;
			HRESULT hr = _device->CreateBuffer(&bd, &sd, &_lightBufferForDepth);
			if (FAILED(hr))
			{
				assert(SUCCEEDED(hr));
				return false;
			}
		}

		{
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.ByteWidth = sizeof(LightDataForRender);
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));
			sd.pSysMem = &_lightDataForRender;
			HRESULT hr = _device->CreateBuffer(&bd, &sd, &_lightBufferForRender);
			if (FAILED(hr))
			{
				assert(SUCCEEDED(hr));
				return false;
			}
		}
		return true;
	}
	HRESULT Light::CreateRenderTargetData()
	{
		HRESULT hr;

		D3D11_TEXTURE2D_DESC desc;
		desc.Width = (UINT)2048;
		desc.Height = (UINT)2048;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.ArraySize = 1;

		if (FAILED(hr = _device->CreateTexture2D(&desc, NULL, &_renderTargetTexture)))
		{
			return hr;
		}

		if (FAILED(hr = _device->CreateRenderTargetView(_renderTargetTexture, NULL, &_renderTargetView)))
		{
			return hr;
		}
	}
	void Light::UpdateLightBuffer()
	{
		TVector3 up(0, 1, 0);
		D3DXMatrixLookAtLH(&_lightDataForDepth.WorldMatrix, &m_vPos, &m_vLookAt, &up);

		LightLocationData locationData;
		D3DXMatrixTranspose(&locationData.WorldMatrix, &_lightDataForDepth.WorldMatrix);
		D3DXMatrixTranspose(&locationData.ViewMatrix, &_lightDataForDepth.ViewMatrix);
		D3DXMatrixTranspose(&locationData.ProjMatrix, &_lightDataForDepth.ProjMatrix);

		{
			_dc->UpdateSubresource(_lightBufferForDepth, 0, nullptr, &locationData, 0, 0);
		}

		{
			LightDataForRender tmp;
			tmp.LocationData = locationData;
			tmp.Color = _lightDataForRender.Color;

			_dc->UpdateSubresource(_lightBufferForRender, 0, nullptr, &tmp, 0, 0);
		}
	}
	void Light::Initialize_SetDevice(ID3D11Device* device, ID3D11DeviceContext* dc)
	{
		_device = device;
		_dc = dc;
	}
	void Light::SetLightDirection(TVector3 direction)
	{
		m_vLookAt = direction;
		UpdateLightBuffer();
	}
	void Light::SetLightPosition(TVector3 position)
	{
		m_vPos = position;
		UpdateLightBuffer();
	}
	void Light::SetLightColor(TVector4 color)
	{
		_lightDataForRender.Color = color;
		UpdateLightBuffer();
	}
	void Light::Init()
	{
		CreateRenderTargetData();
		CreateCameraBuffer();
		CreateDepthMap();
		CreateLightData();
		CreateLightBuffer();
		CreateLightingShader();
	}
	void Light::Frame()
	{
	}
	void Light::PreRender()
	{
		_dc->PSSetShader(m_pPS, NULL, 0);
		_dc->GSSetShader(m_pGS, NULL, 0);

		_dc->GSSetConstantBuffers(0, 1, &_lightBufferForDepth);

		_dc->OMSetBlendState(DXState::g_pAlphaBlend, 0, -1);
		_dc->OMSetRenderTargets(1, &_renderTargetView, _shadowDepthMapDepthStencilView);

		const FLOAT color[] = { 0, 0, 0, 0 };
		_dc->ClearRenderTargetView(_renderTargetView, color);
		_dc->ClearDepthStencilView(_shadowDepthMapDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
	void Light::Render()
	{
		//_dc->PSSetShader(_lightingShader->m_pPS, NULL, 0);

		_dc->PSSetConstantBuffers(1, 1, &_objectToWorldTransformBuffer);
		_dc->PSSetConstantBuffers(10, 1, &_lightBufferForRender);
		_dc->PSSetShaderResources(4, 1, &_shadowDepthMapShaderResourceView);
	}
	void Light::Release()
	{
		if (_shadowDepthMap != nullptr)
		{
			_shadowDepthMap->Release();
			_shadowDepthMap = nullptr;
		}

		if (_shadowDepthMapShaderResourceView != nullptr)
		{
			_shadowDepthMapShaderResourceView->Release();
			_shadowDepthMapShaderResourceView = nullptr;
		}

		if (_shadowDepthMapDepthStencilView != nullptr)
		{
			_shadowDepthMapDepthStencilView->Release();
			_shadowDepthMapDepthStencilView = nullptr;
		}

		if (_lightBufferForDepth != nullptr)
		{
			_lightBufferForDepth->Release();
			_lightBufferForDepth = nullptr;
		}

		if (_lightBufferForRender != nullptr)
		{
			_lightBufferForRender->Release();
			_lightBufferForRender = nullptr;
		}

		if (_toViewSpaceTransformBuffer != nullptr)
		{
			_toViewSpaceTransformBuffer->Release();
			_toViewSpaceTransformBuffer = nullptr;
		}

		if (_objectToWorldTransformBuffer != nullptr)
		{
			_objectToWorldTransformBuffer->Release();
			_objectToWorldTransformBuffer = nullptr;
		}

		if (m_pGS != nullptr)
		{
			m_pGS->Release();
			m_pGS = nullptr;
		}

		if (m_pGSCode != nullptr)
		{
			m_pGSCode->Release();
			m_pGSCode = nullptr;
		}

		if (m_pPS != nullptr)
		{
			m_pPS->Release();
			m_pPS = nullptr;
		}

		if (m_pPSCode != nullptr)
		{
			m_pPSCode->Release();
			m_pPSCode = nullptr;
		}

		if (_renderTargetTexture != nullptr)
		{
			_renderTargetTexture->Release();
			_renderTargetTexture = nullptr;
		}

		if (_renderTargetView != nullptr)
		{
			_renderTargetView->Release();
			_renderTargetView = nullptr;
		}
	}
	void Light::CreateCameraBuffer()
	{
		{
			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
			desc.ByteWidth = sizeof(ToViewSpaceTransformData);
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));
			sd.pSysMem = &_toViewSpaceTransformData;
			HRESULT hr = _device->CreateBuffer(&desc, &sd, &_toViewSpaceTransformBuffer);
			if (FAILED(hr))
			{
				assert(SUCCEEDED(hr));
			}
		}
		{
			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
			desc.ByteWidth = sizeof(ObjectToWorldTransformData);
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));
			sd.pSysMem = &_objectToWorldTransformData;
			HRESULT hr = _device->CreateBuffer(&desc, &sd, &_objectToWorldTransformBuffer);
			if (FAILED(hr))
			{
				assert(SUCCEEDED(hr));
			}
		}
	}
	void Light::UpdateCameraBuffer()
	{
		D3DXMatrixTranspose(&_objectToWorldTransformData.World, &m_matWorld);
		_dc->UpdateSubresource(_objectToWorldTransformBuffer, 0, nullptr, &m_matWorld, 0, 0);

		D3DXMatrixTranspose(&_toViewSpaceTransformData.matView, &m_matView);
		D3DXMatrixTranspose(&_toViewSpaceTransformData.matProj, &m_matProj);
		_dc->UpdateSubresource(_toViewSpaceTransformBuffer, 0, nullptr, &_toViewSpaceTransformData, 0, 0);
	}
	void Light::SetMatrix(TMatrix* matWorld, TMatrix* matView, TMatrix* matProj)
	{
		if (matWorld)
		{
			m_matWorld = *matWorld;
		}
		if (matView)
		{
			m_matView = *matView;
		}
		if (matProj)
		{
			m_matProj = *matProj;
		}
		UpdateCameraBuffer();
	}
}