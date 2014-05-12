// Copyright (c) 2013-2014 Vittorio Romeo
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
	class OBCParticleEmitter : public OBCActorND
	{
		public:
			using GameParticleMemFn = void(OBGame::*)(std::size_t mCount, const Vec2f& mPos);

		private:
			Vec2f offset;
			GameParticleMemFn particleMemFn;
			std::size_t count;

		public:
			OBCParticleEmitter(OBCPhys& mCPhys, GameParticleMemFn mParticleMemFn, std::size_t mCount = 1) : OBCActorND{mCPhys}, particleMemFn{mParticleMemFn}, count{mCount} { }

			inline void update(FT) override { (game.*particleMemFn)(count, cPhys.getPosPx() + offset); }

			inline void setOffset(const Vec2f& mOffset) noexcept	{ offset = mOffset; }
			inline const Vec2f& getOffset() const noexcept			{ return offset; }
	};
}

#endif
