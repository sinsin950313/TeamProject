#include "LightManager.h"

namespace SSB
{
	void LightManager::SetLight(SSB::Light* light)
	{
		if (m_MainLight != nullptr)
		{
			m_MainLight->Release();
			delete m_MainLight;
		}
		m_MainLight = light;
	}
	std::vector<Light*> LightManager::GetLightList()
	{
		return { m_MainLight };
	}
	void LightManager::Init()
	{
	}
	void LightManager::Frame()
	{
		if (m_MainLight != nullptr)
		{
			m_MainLight->Frame();
		}
	}
	void LightManager::PreRender()
	{
		m_MainLight->PreRender();
	}
	void LightManager::Render()
	{
		m_MainLight->Render();
	}
	void LightManager::Release()
	{
		if (m_MainLight != nullptr)
		{
			m_MainLight->Release();
			delete m_MainLight;
			m_MainLight = nullptr;
		}
	}
}
