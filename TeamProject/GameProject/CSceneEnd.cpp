#include "CSceneEnd.h"
#include "Input.h"

bool CSceneEnd::Init()
{
	//m_pEndSound = I_Sound.Find(L"swish.wav");

	//m_pOver = new CInterface();
	//m_pOver->Load(m_pd3dDevice, m_pImmediateContext, I_Sprite.GetSpriteData(L"Gameover"));
	//m_pOver->SetAttribute(CVector(g_rcClient.right * 0.5f, g_rcClient.bottom * 0.5f, 0));
	//m_pOver->SetPosition(CVector(g_rcClient.right * 0.5f, g_rcClient.bottom * 0.5f, 0), CVector(0, 0, 0));
	//
	//m_pBlack = new CInterface();
	//m_pBlack->Load(m_pd3dDevice, m_pImmediateContext, I_Sprite.GetSpriteData(L"Black"));
	//m_pBlack->SetAttribute(CVector(g_rcClient.right * 0.5f, g_rcClient.bottom * 0.5f, 0)
	//								, { 0, 0, (float)g_rcClient.right, (float)g_rcClient.bottom });

	m_iPlayed = 0;

	return true;
}

bool CSceneEnd::Frame()
{
	//if (!m_pEndSound->IsPlay() && m_iPlayed == 0)
	//{
	//	m_iPlayed = 1;
	//	m_pEndSound->Play();
	//}
	//m_pOver->Frame();
	return true;
}

bool CSceneEnd::Render()
{
	//m_pBlack->Render();
	//m_pOver->Render();
	return true;
}

bool CSceneEnd::Release()
{
	//m_pBlack->Release();
	//delete m_pBlack;
	//m_pOver->Release();
	//delete m_pOver;
	return true;
}

E_SCENE CSceneEnd::NextScene()
{
	for (int i = 0; i < 256; i++)
	{
		if (I_Input.GetKey(i) == KEY_PUSH)
		{
			return E_SCENE::S_TITLE;
		}
	}
	return E_SCENE::S_END;
}