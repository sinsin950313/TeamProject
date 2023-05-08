#include "Effect.h"

bool	Effect::Init(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext)
{
	m_pd3dDevice = pd3dDevice;
	m_pImmediateContext = pContext;

	m_isDone = false;

	return true;
}

bool	Effect::Frame()
{
	// Emitter는 SpawnCount, DestroyTime등으로 자신의 삭제를 결정
	// 하지만 InitDelay를 지났는지 확인해야함
	// Effect는 리스트의 Emitter가 모두 없어지면 삭제 결정
	TMatrix matRot;
	if(m_vRot != TVector3::Zero)
		D3DXMatrixRotationYawPitchRoll(&matRot, m_vRot.y, m_vRot.x, m_vRot.z);
	if(m_vPos != TVector3::Zero)
		D3DXMatrixTranslation(&m_matTopWorld, m_vPos.x, m_vPos.y, m_vPos.z);
	if (m_vFollowPos)
	{
		D3DXMatrixTranslation(&m_matTopWorld, m_vFollowPos->x, m_vFollowPos->y, m_vFollowPos->z);
	}
	D3DXMatrixMultiply(&m_matTopWorld, &matRot, &m_matTopWorld);

	TMatrix matParentTrans;
	D3DXMatrixTranslation(&matParentTrans, m_vParentPos.x, m_vParentPos.y, m_vParentPos.z);

	auto iter = m_pEmitterList.begin();
	while(iter != m_pEmitterList.end())
	{
		Emitter* pEmitter = (*iter);
		if (pEmitter->m_isDone)
		{
			iter = m_pEmitterList.erase(iter);
			pEmitter->Release();
			delete pEmitter;
		}
		else
		{
			pEmitter->m_matParentTrans = matParentTrans;
			pEmitter->m_matTopWorld = &m_matTopWorld;
			pEmitter->Frame();
			iter++;
		}
	}
	if (m_pEmitterList.size() == 0)
	{
		m_isDone = true;
	}
	return true;
}

bool	Effect::Render()
{
	for (auto pEmitter : m_pEmitterList)
	{
		pEmitter->Render();
	}
	m_pImmediateContext->GSSetShader(NULL, NULL, 0);
	return true;
}

bool	Effect::Release()
{
	for (auto pEmitter : m_pEmitterList)
	{
		pEmitter->Release();
		delete pEmitter;
		pEmitter = nullptr;
	}
	m_pEmitterList.clear();
	return true;
}

void	Effect::SetCamera(Camera* pCamera)
{
	for (auto pEmitter : m_pEmitterList)
	{
		pEmitter->SetCamera(pCamera);
	}
}

void	Effect::Reset()
{
	for (auto pEmitter : m_pEmitterList)
	{
		pEmitter->Reset();
	}
}