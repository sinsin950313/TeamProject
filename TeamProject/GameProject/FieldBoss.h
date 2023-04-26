#pragma once

#include "Character.h"
#include <queue>

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
			bool _isHit = false;

		public:
			Arrow();
			~Arrow();

		private:
			void Move();
			void CollisionCheck();

		public:
			void Active(MeshMap* map, float timeStamp, TVector3 pos, TVector3 target);
			void UnActive();
			bool IsDead();

		public:
			bool Init() override;
			bool Render() override;
			bool Frame() override;
			bool Release() override;
		};

	private:
        float m_BattleRange = 20;
        float m_SpotRange = 40;
		std::set<Arrow*> _activeArrows;
		std::queue<Arrow*> _unActiveArrows;

    public:
        float GetBattleRange();
        float GetSpotRange();

    public:
		FieldBoss();
		virtual ~FieldBoss();

	public:
		void Attack();
		void	SetMatrix(TMatrix* matWorld, TMatrix* matView, TMatrix* matProj) override;

	public:
		bool Frame() override;
		bool Render() override;
		bool Release() override;
	};
}
