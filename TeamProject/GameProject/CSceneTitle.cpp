#include "CSceneTitle.h"

bool    CSceneTitle::Init()
{
	//I_Sprite.GameDataLoad(L"../../data/1945/GameData.txt");
	//
	//m_pTitle = new CInterface();
	//m_pTitle->Load(m_pd3dDevice, m_pImmediateContext, I_Sprite.GetSpriteData(L"Title_bg"));
	//m_pTitle->m_fScale = 3.0f;
	//m_pTitle->SetCameraSize(CVector(1024, 768, 0));
	//m_pTitle->SetPosition({ g_rcClient.right * 0.5f, g_rcClient.bottom * 0.5f, 0 }, CVector(0, 0, 0));
	//m_pTitle->SetPosition({ g_rcClient.right * 0.5f
	//								, (g_rcClient.bottom - g_rcClient.top) - m_pTitle->m_Box.m_vSize.y * 0.5f + 80, 0 }, CVector(0, 0, 0));
	//m_pTitle->m_pWorkList.push_back(new InterfaceAnim(0.02f));
	//
	//CInterface* pInter = new CInterface();
	//pInter->Load(m_pd3dDevice, m_pImmediateContext, I_Sprite.GetSpriteData(L"Title_bg2"));
	//pInter->m_fScale = 3.0f;
	//pInter->SetCameraSize(CVector(1024, 768, 0));
	//pInter->SetPosition({ g_rcClient.right * 0.5f, g_rcClient.bottom * 0.5f, 0 }, CVector(0, 0, 0));
	//pInter->SetPosition({ g_rcClient.right * 0.5f
	//							, m_pTitle->m_vPos.y - pInter->m_Box.m_vSize.y * 0.5f - m_pTitle->m_Box.m_vSize.y * 0.5f, 0 }, CVector(0, 0, 0));
	//m_pTitle->AddChild(pInter);
	//
	//pInter = new CInterface();
	//pInter->Load(m_pd3dDevice, m_pImmediateContext, I_Sprite.GetSpriteData(L"Title_num"));
	//pInter->m_fScale = 3.0f;
	//pInter->SetCameraSize(CVector(1024, 768, 0));
	//pInter->SetPosition({ g_rcClient.right * 0.5f, g_rcClient.bottom * 0.5f, 0 }, CVector(0, 0, 0));
	//pInter->SetPosition({ g_rcClient.right * 0.5f, pInter->m_vPos.y - pInter->m_Box.m_vSize.y * 0.15f, 0 }, CVector(0, 0, 0));
	//m_pTitle->AddChild(pInter);
	//
	//pInter = new CInterface();
	//pInter->Load(m_pd3dDevice, m_pImmediateContext, I_Sprite.GetSpriteData(L"Title_title"));
	//pInter->m_fScale = 3.0f;
	//pInter->SetCameraSize(CVector(1024, 768, 0));
	//pInter->SetPosition({ g_rcClient.right * 0.5f, g_rcClient.bottom * 0.5f, 0 }, CVector(0, 0, 0));
	//pInter->SetPosition({ g_rcClient.right * 0.5f, pInter->m_vPos.y - pInter->m_Box.m_vSize.y * 0.15f, 0 }, CVector(0, 0, 0));
	//m_pTitle->AddChild(pInter);
	//
	//CVector pos = pInter->m_vPos;
	//m_pStartButton = new CInterface();
	//m_pStartButton->Load(m_pd3dDevice, m_pImmediateContext, I_Sprite.GetSpriteData(L"Title_btn"));
	//m_pStartButton->SetAttribute({ 0, 0, 0 });
	//m_pStartButton->SetPosition({ g_rcClient.right * 0.5f, pos.y + 350, 0 }, CVector(0, 0, 0));
	//m_pStartButton->SetCameraSize(CVector(1024, 768, 0));
	//m_pStartButton->SetAnimation(0);
	//m_pStartButton->m_pWorkList.push_back(new InterfaceClick(3.0f));
	//m_pStartButton->m_pWorkList.push_back(new InterfaceAnim(0.2f));
	//m_pTitle->AddChild(m_pStartButton);
	//
	//CInterface* pCover = new CInterface();
	//pCover->Load(m_pd3dDevice, m_pImmediateContext, I_Sprite.GetSpriteData(L"Black"));
	//float width = g_rcClient.right - g_rcClient.left;
	//float height = g_rcClient.bottom - g_rcClient.top;
	//CVector size = CVector((width - m_pTitle->m_Box.m_vSize.x) * 0.5,
	//	height, 0);
	//pCover->SetAttribute(size * 0.5f, { 0, 0, size.x, size.y });
	//m_pTitle->AddChild(pCover);
	//
	//
	//pCover = new CInterface();
	//pCover->Load(m_pd3dDevice, m_pImmediateContext, I_Sprite.GetSpriteData(L"Black"));
	//CVector v = { width - size.x * 0.5f, height * 0.5f, 0 };
	//pCover->SetAttribute(v, { 0, 0, size.x, size.y });
	//m_pTitle->AddChild(pCover);
	//
	//m_pBgTitle = I_Sound.Find(L"00_sel.mp3");
	//m_pBtn = I_Sound.Find(L"MenuEnter_Stereo.WAV");

	return true;
}

E_SCENE     CSceneTitle::NextScene()
{
	//if (m_pStartButton->m_CurrentState == UI_SELECT)
	//{
	//	m_pBtn->Play();
	//	return S_INGAME;
	//}
	return S_TITLE;
}

bool    CSceneTitle::Frame()
{
	//m_pBgTitle->Play(true);
	//m_pTitle->Frame();

	return true;
}

bool    CSceneTitle::Render()
{
	//m_pTitle->Render();
	return true;
}

bool    CSceneTitle::Release()
{
	//if(m_pBgTitle)
	//	m_pBgTitle->Stop();
	//if(m_pTitle)
	//	m_pTitle->Release();
	return true;
}