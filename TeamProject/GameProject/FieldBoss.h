#pragma once

#include "Character.h"

namespace SSB
{
	class FieldBoss : public Character
	{
	private:
		class Arrow : public Character
		{
		private:
			const float _kLiveTime = 3.0f;
			float _startTimeStamp;
			const float _damage = 30;
			bool _isDead = false;

		public:
			Arrow(MeshMap* map, float timeStamp);

		private:
			void Move();
			void CollisionCheck();

		public:
			bool IsDead();

		public:
			bool Frame() override;
		};

	private:
        float m_BattleRange = 20;
        float m_SpotRange = 30;
		std::set<Arrow*> _arrows;

    public:
        float GetBattleRange();
        float GetSpotRange();

    public:
		virtual ~FieldBoss();

	public:
		void Attack();
		void	SetMatrix(TMatrix* matWorld, TMatrix* matView, TMatrix* matProj) override;

	public:
		bool Frame() override;
		bool Release() override;
	};
}
