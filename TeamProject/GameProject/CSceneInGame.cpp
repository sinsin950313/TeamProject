#include "CSceneInGame.h"

E_SCENE CSceneInGame::NextScene()
{
    //if (m_pPlayer->m_iLife < 0)
    //{
    //    return S_END;
    //}
    //if (I_Enemy.m_pBoss)
    //{
    //    if (I_Enemy.m_pBoss->m_iLife == 0)
    //    {
    //        return S_END;
    //    }
    //}
    return S_INGAME;
}

bool    CSceneInGame::Init()
{
    //m_pBgIngame = I_Sound.Find(L"01_bgm0.mp3");
    //m_pItemSound = I_Sound.Find(L"ItemSelect.wav");
    //m_pAlertSound = I_Sound.Find(L"Alert.mp3");
    //
    //m_pWorldSP = new CQuadTree;
    ////m_Game.m_pWorldSP = new COcTree;
    //m_pWorldSP->Create(CVector(-1000, -1000, 0), CVector(2000, 2000, 0));
    //
    //m_pLife = new CInterface();
    //m_pLife->Load(m_pd3dDevice, m_pImmediateContext, I_Sprite.GetSpriteData(L"Player"));
    //CInterface* pInter = nullptr;
    //for (int i = 0; i < 3; i++)
    //{
	//	pInter = new CInterface();
	//	pInter->Load(m_pd3dDevice, m_pImmediateContext, I_Sprite.GetSpriteData(L"Player"));
	//	pInter->SetAttribute(CVector(100, 500, 0));
	//	pInter->SetAnimation(3);
    //    float width = pInter->m_rcActiveImage.right - pInter->m_rcActiveImage.left;
    //    pInter->SetPosition(CVector(171 - width * i, 774, 0), CVector(0, 0, 0), CVector(0, 0, 0));
	//	m_pLife->AddChild(pInter);
    //}
    //
    //m_pScore = new CInterface();
    //m_pScore->Load(m_pd3dDevice, m_pImmediateContext, I_Sprite.GetSpriteData(L"Number"));
    //pInter = nullptr;
    //for (int i = 0; i < 9; i++)
    //{
    //    pInter = new CInterface();
    //    pInter->Load(m_pd3dDevice, m_pImmediateContext, I_Sprite.GetSpriteData(L"Number"));
    //    pInter->SetAttribute(CVector(100, 500, 0));
    //    pInter->SetAnimation(0);
    //    float width = pInter->m_rcActiveImage.right - pInter->m_rcActiveImage.left;
    //    pInter->m_fScale = 1.7f;
    //    pInter->SetPosition(CVector(180 - width * i * pInter->m_fScale, 25, 0), CVector(0, 0, 0), CVector(0, 0, 0));
    //    m_pScore->AddChild(pInter);
    //}
    //
    //float width = g_rcClient.right - g_rcClient.left;
    //float height = g_rcClient.bottom - g_rcClient.top;
    //m_isAlert = false;
    //m_pAlert = new CInterface();
    //m_pAlert->Load(m_pd3dDevice, m_pImmediateContext, I_Sprite.GetSpriteData(L"AlertCenter"));
    //m_pAlert->SetAnimation(0);
    //m_pAlert->m_fScale = 2.0f;
    //m_pAlert->m_fAngle = 180.0f;
    //m_pAlert->m_isUsing = false;
    //m_pAlert->SetCameraSize(CVector(1024, 1024, 0));
    //m_pAlert->m_vPos = CVector(width * 0.5f, height * 0.5f + 75, 0);
    //m_pAlert->SetPosition(m_pAlert->m_vPos, CVector(0, 0, 0), CVector(0, 0, 0));
    //m_pAlert->m_pWorkList.push_back(new InterfaceLoopFade(1.5f));
    //m_pAlert->m_pWorkList.push_back(new InterfaceLifeTime(2.0f));
    //
    //pInter = nullptr;
    //pInter = new CInterface();
    //pInter->Load(m_pd3dDevice, m_pImmediateContext, I_Sprite.GetSpriteData(L"AlertSide"));
    //pInter->SetAnimation(0);
    //pInter->m_fScale = 2.0f;
    //pInter->SetCameraSize(CVector(1024, 1024, 0));
    //pInter->SetPosition(CVector(width * 0.5f, 1024 * 0.1, 0), CVector(0, 0, 0), CVector(0, 0, 0));
    //pInter->m_pWorkList.push_back(new InterfaceLoopFade(1.5f));
    //m_pAlert->AddChild(pInter);
    //
    //pInter = new CInterface();
    //pInter->Load(m_pd3dDevice, m_pImmediateContext, I_Sprite.GetSpriteData(L"AlertSide"));
    //pInter->SetAnimation(0);
    //pInter->m_fScale = 2.0f;
    //pInter->m_fAngle = 180.0f;
    //pInter->SetCameraSize(CVector(1024, 1024, 0));
    //pInter->SetPosition(CVector(width * 0.5f, 1024 * 0.9, 0), CVector(0, 0, 0), CVector(0, 0, 0));
    //pInter->m_pWorkList.push_back(new InterfaceLoopFade(1.5f));
    //m_pAlert->AddChild(pInter);
    //
    //m_fOnGameTime = 0.0f;
    //
    //ObjectInit();
    return true;
}

bool    CSceneInGame::Frame()
{
    //m_pBgIngame->Play(true);
    //m_fOnGameTime += g_fSecondPerFrame;
    //ObjectFrame();
    //int score = m_iScore;
    //for (auto pInter : m_pScore->m_pChildList)
    //{
    //    pInter->SetAnimation(score % 10);
    //    score /= 10;
    //    pInter->Frame();
    //}
    //if (m_pAlert->m_isUsing)
    //{
    //    m_pAlert->Frame();
    //    m_pAlert->m_fAngle += g_fSecondPerFrame * 240.0f;
    //}
    //if (!m_pAlert->m_isUsing && m_isAlert && 
    //    !I_Enemy.m_pBoss)
    //{
    //    I_Enemy.CreateBoss();
    //}
    return true;
}

bool    CSceneInGame::Render()
{
    //m_Writer.Draw(0, 0, I_Timer.m_szTimer);
    //ObjectRender();
    //for (int i = 0 ; i < m_pPlayer->m_iLife; i++)
    //{
    //    CInterface* pInter = m_pLife->m_pChildList[i];
    //    pInter->Render();
    //}
    //for (auto pInter : m_pScore->m_pChildList)
    //{
    //    pInter->Render();
    //}
    //if (m_pAlert->m_isUsing)
    //{
    //    for (auto pInter : m_pAlert->m_pChildList)
    //    {
    //        pInter->Render();
    //    }
    //    m_pAlert->Render();
    //}
    return true;
}

bool    CSceneInGame::Release()
{
    //if (m_pBgIngame)
    //    m_pBgIngame->Stop();
    //
    //m_pLife->Release();
    //delete m_pLife;
    //
    //m_pScore->Release();
    //delete m_pScore;
    //
    //m_pAlert->Release();
    //delete m_pAlert;
    //
    //ObjectRelease();
    //if (m_pWorldSP)
    //{
    //    delete m_pWorldSP;
    //    m_pWorldSP = nullptr;
    //}
    return true;
}


bool		CSceneInGame::ObjectInit()
{
    //I_Enemy.SetDevice(m_pd3dDevice, m_pImmediateContext, m_pWorldSP);
    //I_Bullet.SetDevice(m_pd3dDevice, m_pImmediateContext, m_pWorldSP);
    //I_Item.SetDevice(m_pd3dDevice, m_pImmediateContext, m_pWorldSP);
    //I_Effect.SetDevice(m_pd3dDevice, m_pImmediateContext);
    //
    //CMapObject* pMap = new CMapObject();
    //pMap->Load(m_pd3dDevice, m_pImmediateContext, I_Sprite.GetSpriteData(L"Map1"));
    //pMap->m_fScale = 2.0f;
    //pMap->SetPosition(CVector(g_rcClient.right * 0.5, g_rcClient.bottom - 4607, 0), CVector(g_rcWindow.right, g_rcWindow.bottom, 0));
    //m_pMapList.push_back(pMap);
    //
    //pMap = new CMapObject();
    //pMap->Load(m_pd3dDevice, m_pImmediateContext, I_Sprite.GetSpriteData(L"Map2"));
    //pMap->m_fScale = 2.0f;
    //pMap->SetPosition(CVector(g_rcClient.right * 0.5, g_rcClient.bottom - 4607, 0), CVector(g_rcWindow.right, g_rcWindow.bottom, 0));
    //m_pMapList.push_back(pMap);
    //
    //CSprite* pCover = new CSprite();
    //pCover->Load(m_pd3dDevice, m_pImmediateContext, I_Sprite.GetSpriteData(L"Black"));
    //CVector size = CVector((g_rcClient.right - pMap->m_Box.m_vSize.x) * 0.5, 
    //                       g_rcClient.bottom, 0);
    //pCover->SetPosition(size * 0.5f, CVector(0, 0, 0));
    //pCover->SetUVRect({ 0, 0, size.x, (float)g_rcClient.bottom });
    //m_pBorder.push_back(pCover);
    //
    //pCover = new CSprite();
    //pCover->Load(m_pd3dDevice, m_pImmediateContext, I_Sprite.GetSpriteData(L"Black"));
    //CVector v = size * 0.5f;
    //v.x = g_rcClient.right - v.x;
    //pCover->SetPosition(v, CVector(0, 0, 0));
    //pCover->SetUVRect({ 0, 0, size.x, (float)g_rcClient.bottom });
    //m_pBorder.push_back(pCover);
    //
    ////I_Effect.UsingEffect(CVector(g_rcClient.right * 0.5, g_rcClient.bottom * 0.5, 0), CVector(0, 1, 0), L"../../data/1945/Map1.bmp", L"rtMap1");
    ////I_Effect.UsingEffect(CVector(g_rcClient.right * 0.5, g_rcClient.bottom * 0.5, 0), CVector(0, 1, 0), L"../../data/1945/Map1.bmp", L"rtMap2");
    //
    //m_pPlayer = dynamic_cast<CPlayer*>(m_pWorldSP->CreatePlayer(CVector(30, 30, 30), CVector(30, 30, 30)));
    //m_pPlayer->Load(m_pd3dDevice, m_pImmediateContext, I_Sprite.GetSpriteData(L"Player"));
    //m_pPlayer->Init();
    //m_pPlayer->m_fScale = 1.5f;
    //m_pPlayer->SetPosition(CVector(g_rcClient.right * 0.5f, g_rcClient.bottom * 1.1f, 0), m_pPlayer->m_Box.m_vSize, CVector(0, 0, 0));
    //
    //
    ////for (int iObj = 0; iObj < 10; iObj++)
    ////{
    ////    //CBaseObject* pObj = new CBaseObject();
    ////    CObject2D* pObj = dynamic_cast<CObject2D*>(m_pWorldSP->CreateObject());
    ////    pObj->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/DefaultShapeMask.txt");
    ////    pObj->Init();
    ////    pObj->SetColor(CVector4(0.0f, 1.0f, 0.0f, 0.0f));
    ////    pObj->LoadTexture(L"../../data/bitmap1.bmp");
    ////    pObj->SetMask(pMask);
    ////    pObj->SetUVRect({ 1, 1, 263, 60 });
    ////
    ////    m_StaticList.insert(std::make_pair(iObj, pObj));
    ////    m_AllObjectList.insert(std::make_pair(iObj, pObj));
    ////    m_pWorldSP->AddStatiCBaseObject(pObj);
    ////}
    //
    //I_Enemy.m_vPlayerPos = &m_pPlayer->m_vPos;
    return true;
}

bool		CSceneInGame::ObjectFrame()
{

    //for (CObject2D* map : m_pMapList)
    //{
    //    map->Frame();
    //    if (!m_isAlert && map->m_fSpeed < 1.0f)
    //    {
    //        m_isAlert = true;
    //        m_pAlert->m_isUsing = true;
    //        m_pAlertSound->PlayEffect();
    //    }
    //}
    //m_pWorldSP->ObjectListReset(m_pWorldSP->m_pRoot);
    //m_pPlayer->Frame();
    //
    //I_Enemy.Frame();
    //I_Bullet.Frame();
    //I_Effect.Frame();
    //I_Item.Frame();
    //
    //for (CBaseObject* pObj : I_Bullet.m_pPlayerBulletList)
    //{
    //    CRect colRect;
    //    CBaseObject* colObj = nullptr;
    //    m_pWorldSP->CollisionPlayerBullet(pObj, colRect, colObj);
    //    if (colObj != nullptr)
    //    {
    //        CEnemy* pEnemy = dynamic_cast<CEnemy*>(colObj);
	//		CBullet* pBullet = (dynamic_cast<CBullet*>(pObj));
	//		//colObj->SetPosition(CVector(300, 300, 0), pEnemy->m_Box.m_vSize);
	//		//enemy->Hit();
	//		//pBullet->Hit();
    //        float x = (colRect.right + colRect.left) * 0.5f;
    //        float y = (colRect.bottom + colRect.top) * 0.5f;
    //        y += pBullet->m_vDir.y;
    //        //y -= min(abs(colRect.top - y), abs(colRect.bottom - y));
    //        I_Effect.UsingEffect(CVector(x, y, 0), CVector(0, 0, 0), 2.0f, L"BulletEffect");
    //        
	//		pBullet->m_isUsing = false;
    //        //pEnemy->m_isUsing = false;
    //        pEnemy->Hit();
    //        m_iScore += 120;
    //    }
    //    if (I_Enemy.m_pBoss)
    //    {
    //        CEnemyBoss* pBoss = I_Enemy.m_pBoss;
    //        CBox box;
    //        BOXCOLLISIONTYPE ct = pObj->m_Box.BoxCollision(pBoss->m_Box, box);
    //        if ((ct == BOXCOLLISIONTYPE::BOX_IN || ct == BOXCOLLISIONTYPE::BOX_OVERLAP) &&
    //            pBoss->m_isUsing)
    //        {
    //            pBoss->Hit();
    //            I_Effect.UsingEffect(box.m_vCenter, CVector(0, 0, 0), 2.0f, L"BulletEffect");
    //            (dynamic_cast<CBullet*>(pObj))->m_isUsing = false;
    //        }
    //
    //    }
    //}
    //
    //CRect colRect;
    //CBaseObject* colObj = nullptr;
    //m_pWorldSP->CollisionEnemyBullet(m_pPlayer, colRect, colObj);
    //if (colObj != nullptr)
    //{
    //    float x = (colRect.right + colRect.left) * 0.5f;
    //    float y = (colRect.bottom + colRect.top) * 0.5f;
    //    y += colObj->m_vDir.y * colObj->m_fSpeed * g_fSecondPerFrame;
    //    CVector cen = CVector(x, y, 0);
    //    I_Effect.UsingEffect(cen, CVector(0, 0, 0), 2.0f, L"BulletEffect");
    //    dynamic_cast<CBullet*>(colObj)->m_isUsing = false;
    //    m_pPlayer->Hit();
    //}
    //
    //colObj = nullptr;
    //m_pWorldSP->CollisionEnemy(m_pPlayer, colRect, colObj);
    //if (colObj != nullptr)
    //{
    //    dynamic_cast<CEnemy*>(colObj)->m_isUsing = false;
    //    I_Effect.UsingEffect(colObj->m_vPos, CVector(0, 0, 0), 1.0f, L"LargeExplosion");
    //    m_pPlayer->Hit();
    //}
    //
    //colObj = nullptr;
    //m_pWorldSP->CollisionItem(m_pPlayer, colRect, colObj);
    //if (colObj != nullptr)
    //{
    //    m_pItemSound->PlayEffect();
    //    m_iScore += 50;
    //    dynamic_cast<CItem*>(colObj)->m_isUsing = false;
    //    if(m_pPlayer->m_iAttackLevel < 3)
    //        m_pPlayer->m_iAttackLevel++;
    //}
    return true;
}

bool		CSceneInGame::ObjectRender()
{
    //for (CObject2D* map : m_pMapList)
    //{
    //    map->Render();
    //}
    //I_Bullet.Render();
    //I_Item.Render();
    //I_Enemy.Render();
    //m_pPlayer->Render();
    //I_Effect.Render();
    //for (CSprite* sprite : m_pBorder)
    //{
    //    sprite->Render();
    //}
    return true;
}

bool		CSceneInGame::ObjectRelease()
{
    //for (CSprite* sprite : m_pBorder)
    //{
    //    sprite->Release();
    //}
    //for (CObject2D* map : m_pMapList)
    //{
    //    map->Release();
    //    delete map;
    //    map = nullptr;
    //}
    //
    //I_Item.Release();
    //I_Bullet.Release();
    //I_Enemy.Release();
    //I_Effect.Release();
    //
    //if (m_pPlayer)
    //{
    //    m_pPlayer->Release();
    //    delete m_pPlayer;
    //    m_pPlayer = nullptr;
    //}

    return true;
}