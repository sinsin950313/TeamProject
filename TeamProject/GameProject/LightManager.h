#pragma once

#include <set>
#include "Light.h"
#include "std.h"

namespace SSB
{
	// 이번 프로젝트에서는 1개의 Light 만 사용한다 가정
	class LightManager : public Singleton<LightManager>
	{
	private:
		//std::set<SSB::Light*> m_Lights;
		SSB::Light* m_MainLight = nullptr;

	public:
		~LightManager();

	public:
		//void RegisterLight(SSB::Light* light);
		void SetLight(SSB::Light* light);

	public:
		std::vector<Light*> GetLightList();

	public:
		void Init();
		void Frame();
		//void PreRender();
		void Render();
		void Release();
	};
}
#define I_Light LightManager::GetInstance()
