#pragma once

#include "CameraTPS.h"
#include "FQuadTree.h"
#include "TSelect.h"

namespace SSB
{
	class SpringArmCamera : public CameraTPS
	{
	private:
		float _kMaxDistance = 10.0f;
		TSelect m_Select;
		FQuadTree* _map;
		T_BOX _collideCheckBox;

	private:
		bool CheckIntersectionWithMap();
		float ShotRay();

	public:
		void Initialize_SetMap(FQuadTree* map);

	public:
		virtual bool Frame() override;
	};
}
