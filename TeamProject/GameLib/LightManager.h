#pragma once

#include <set>
#include "Light.h"

namespace SSB
{
	class LightManager
	{
	private:
		std::set<SSB::Light*> m_Lights;

	private:
		void Lighting();

	public:
		void RegisterLight(SSB::Light* light);
	};
}
