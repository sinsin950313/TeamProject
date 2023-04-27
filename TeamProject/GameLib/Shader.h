#pragma once
#include "DXDevice.h"
#define		MAXLIGHTNUM 10
class Shader
{
public:
	ID3D11Device* m_pd3dDevice;
	ID3D11DeviceContext* m_pImmediateContext;

public:
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();

	HRESULT Load(
		ID3D11Device* pd3dDevice,
		ID3D11DeviceContext* pContext,
		std::wstring name, std::wstring VSname, std::wstring PSname);

	HRESULT VSLoad(
		ID3D11Device* pd3dDevice,
		ID3D11DeviceContext* pContext,
		std::wstring name, std::wstring VSname);

	HRESULT GSLoad(
		ID3D11Device* pd3dDevice,
		ID3D11DeviceContext* pContext,
		std::wstring name, std::wstring GSname);

	HRESULT PSLoad(
		ID3D11Device* pd3dDevice,
		ID3D11DeviceContext* pContext,
		std::wstring name, std::wstring PSname);

private:
	HRESULT CreateVertexShader(std::wstring name, std::wstring VSname = L"VS");
	HRESULT CreatePixelShader(std::wstring name, std::wstring PSname = L"PS");
	HRESULT CreateGeometryShader(std::wstring name, std::wstring funcName = L"GS");

public:
	ID3D11VertexShader* m_pVS = NULL;
	ID3DBlob* m_pVSCode = nullptr;

	ID3D11PixelShader* m_pPS = NULL;
	ID3DBlob* m_pPSCode = nullptr;

	ID3D11GeometryShader* m_pGS;
	ID3DBlob* m_pGSCode = nullptr;
};

