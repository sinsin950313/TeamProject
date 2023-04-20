#include "Effect.h"

bool	Effect::Init()
{
	return true;
}

bool	Effect::Frame()
{
	// Emitter는 SpawnCount, DestroyTime등으로 자신의 삭제를 결정
	// 하지만 InitDelay를 지났는지 확인해야함
	// Effect는 리스트의 Emitter가 모두 없어지면 삭제 결정
	TMatrix world;
	D3DXMatrixTranslation(&world, m_vPos.x, m_vPos.y, m_vPos.z);
	for (auto pEmitter : m_pEmitterList)
	{
		pEmitter->m_matParent = world;
		pEmitter->Frame();
	}
	return true;
}

bool	Effect::Render()
{
	for (auto pEmitter : m_pEmitterList)
	{
		pEmitter->Render();
	}
	return true;
}

bool	Effect::Release()
{
	for (auto pEmitter : m_pEmitterList)
	{
		pEmitter->Release();
		delete pEmitter;
	}
	return true;
}

void	Effect::Reset()
{
	for (auto pEmitter : m_pEmitterList)
	{
		pEmitter->Reset();
	}
}