#include "Player.h"
#include "Input.h"
#include "CollisionMgr.h"

Player::Player()
{

}

Player::~Player()
{
	if (m_pTrail)
	{
		m_pTrail->Release();
		delete m_pTrail;
		m_pTrail = nullptr;
	}
	Release();
}

bool    Player::Init()
{
	Character::Init();

	m_AttackBox.CreateOBBBox(1, 1, 1);

	m_pTrail = new TrailEffect();
	m_pTrail->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/TrailEffect.txt", L"../../data/swoosh.png");
	m_pTrail->Init();

	return true;
}

bool    Player::Frame()
{
	Character::Frame();
	m_pTrail->Frame();

	//
	// 행렬 배열? 큐? 리스트?를 플레이어 전용 쉐이더 파일을 만들어서 넘기고
	// 알파값 및 색상을 가까움 ~ 멂으로 조절 
	// 픽셀 쉐이더에서 픽셀 그리기
	// 
	// 인스턴싱을 사용하는 방법
	// 인스턴싱 데이터를 상수버퍼로 등록한 다음 랜더링하기
	// 플레이어 전용 쉐이더를 컴파일 해야할듯
	// https://m.blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=sorkelf&logNo=40171221589
	// https://www.braynzarsoft.net/viewtutorial/q16390-33-instancing-with-indexed-primitives
	// 
	// float4x4 mTransform : TRANSFORM; // 행렬값
	// float4 mesh : MESHCOLOR; // 컬러값 
	// 위 두개는 상수버퍼로 지정된 개수로 넘길 예정 배열의 인덱스는 밑에 SV_VertexID값을 사용할 예정
	// 
	// 현 시점 문제는 플레이어 밑에 모델 밑에 매쉬의 쉐이더를 변경해야 하는데 
	// 상수버퍼 할당은 5번에 하면 된다고 치고 픽셀 쉐이더는 안바꿔도 될거 같고
	// 버텍스 쉐이더만 변경하면 될텐데 
	// 변경 하는 함수는 SetVertexShader(Shader* shader)를 사용하면 될듯 모델안에 들어있음 사용하면 모든 매쉬를 갈아치움
	// 
	// 
	// uint  iVertex : SV_VertexID; // 시스템으로 들어감 버텍스 인풋 구조체 마지막줄에 추가해주면 됨
	// 
	// m_pSprite->PreDraw(g_pImmediateContext);
	// {
	// 	ID3D11Buffer* vb[2] = { m_pSprite->m_dxobj.g_pVertexBuffer.Get(),	m_pSprite->m_pVBInstance.Get() };
	// 	UINT stride[2] = { sizeof(PNCT_VERTEX), sizeof(TInstatnce) };
	// 	UINT offset[2] = { 0, 0 };
	// 	g_pImmediateContext->IASetVertexBuffers(0, 2, vb, stride, offset);
	// 	g_pImmediateContext->UpdateSubresource(
	// 		m_pSprite->m_dxobj.g_pConstantBuffer.Get(), 0, NULL, &m_pSprite->m_cbData, 0, 0);
	// }
	// 	g_pImmediateContext->DrawIndexedInstanced(m_pSprite->m_dxobj.m_iNumIndex, (UINT)m_pSprite->m_pInstance.size(), 0, 0, 0);
	// (객체의 랜더 인덱스 개수, 인스턴싱 데이터의 개수, 0, 0, 0)
	// 
	//

	return true;
}

bool	Player::Render()
{
	m_pImmediateContext->VSSetConstantBuffers(0, 1, &_toViewSpaceTransformBuffer);
	m_pImmediateContext->VSSetConstantBuffers(1, 1, &_objectToWorldTransformBuffer);
	m_pModel->Render();
	//{
	//	m_pModel->_currentAnimation->Render();
	//
	//	m_pImmediateContext->PSSetShader(m_pModel->_ps->m_pPS, NULL, 0);
	//
	//	for (auto material : m_pModel->_materials)
	//	{
	//		material.second->Render();
	//	}
	//
	//	for (auto mesh : m_pModel->_meshes)
	//	{
	//		mesh.second->Render();
	//	}
	//}

	return true;
}