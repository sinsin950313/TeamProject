#pragma once
#include "Shader.h"
class ShaderMgr : public Singleton<ShaderMgr>
{
public:
	ID3D11Device* m_pd3dDevice = nullptr;
	ID3D11DeviceContext* m_pImmediateContext = nullptr;
	void	SetDevice(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext);

private:
	friend class Singleton<ShaderMgr>;
	std::map<std::wstring, Shader*>	m_List;

public:
	HRESULT Load(std::wstring name, std::wstring VSname, std::wstring PSname, Shader** retShader);
	HRESULT Load(std::wstring name, Shader** retShader);
	HRESULT VSLoad(std::wstring name, std::wstring VSfunc, Shader** retShader);
	HRESULT PSLoad(std::wstring name, std::wstring PSfunc, Shader** retShader);

private:
	Shader* Find(std::wstring name);

public:
	bool	Release();
private:
	ShaderMgr();
public:
	~ShaderMgr();
};

#define I_Shader ShaderMgr::GetInstance()

