#pragma once
#include "Scene.h"
#include "Interface.h"

class SceneMgr : public Singleton<SceneMgr>
{
public:
	ID3D11Device* m_pd3dDevice = nullptr;
	ID3D11DeviceContext* m_pImmediateContext = nullptr;
	void	SetDevice(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext);

private:
	friend class Singleton<SceneMgr>;

public:
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();

	void	SceneChange(E_SCENE s_type);
private:
	SceneMgr();
public:
	~SceneMgr();

private:
	Scene** m_pCurrentScene;
	Scene* m_pSceneArray[MAX_SCENE];
	E_SCENE m_CurScene;
	E_SCENE m_NextScene;
	bool    m_isChangingScene;

	Interface* m_pFade;
};

#define I_Scene SceneMgr::GetInstance()

