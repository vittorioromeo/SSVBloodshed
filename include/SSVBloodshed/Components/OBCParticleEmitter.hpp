// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_PARTICLEEMITTER
#define SSVOB_COMPONENTS_PARTICLEEMITTER

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBGame.hpp"
#include "SSVBloodshed/Components/OBCActorBase.hpp"
#include "SSVBloodshed/Components/OBCParticleSystem.hpp"

namespace ob
{
	class OBCParticleEmitter : public OBCActorNoDrawBase
	{
		public:
			using GameParticleMemFn = void(OBGame::*)(unsigned int mCount, const Vec2f& mPos);

		private:
			Vec2f offset;
			GameParticleMemFn particleMemFn;
			unsigned int count;

		public:
			OBCParticleEmitter(OBCPhys& mCPhys, GameParticleMemFn mParticleMemFn, unsigned int mCount = 1) : OBCActorNoDrawBase{mCPhys}, particleMemFn{mParticleMemFn}, count{mCount} { }

			inline void update(float) override { (game.*particleMemFn)(count, cPhys.getPosPx() + offset); }

			inline void setOffset(const Vec2f& mOffset) noexcept	{ offset = mOffset; }
			inline const Vec2f& getOffset() const noexcept			{ return offset; }
	};
}

#endif
