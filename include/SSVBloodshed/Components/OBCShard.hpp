// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_SHARD
#define SSVOB_COMPONENTS_SHARD

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBGame.hpp"
#include "SSVBloodshed/Components/OBCActorBase.hpp"

namespace ob
{
	class OBCShard : public OBCActorBase
	{
		public:
			OBCShard(OBCPhys& mCPhys, OBCDraw& mCDraw) noexcept : OBCActorBase{mCPhys, mCDraw} { }

			inline void init()
			{
				getEntity().addGroups(OBGroup::GShard);
				body.addGroups(OBGroup::GShard);
				body.addGroupsToCheck(OBGroup::GSolidGround, OBGroup::GFriendly);
				body.addGroupsNoResolve(OBGroup::GOrganic, OBGroup::GPit);
				body.setRestitutionX(0.8f);
				body.setRestitutionY(0.8f);
				body.onPreUpdate += [this]{ body.setVelocity(ssvs::getCClampedMax(body.getVelocity() * 0.99f, 500.f)); };
				body.onDetection += [this](const DetectionInfo& mDI)
				{
					if(!mDI.body.hasGroup(OBGroup::GPlayer)) return;

					getComponentFromBody<OBCPlayer>(mDI.body).shardGrabbed();
					getEntity().destroy(); game.createPShard(20, cPhys.getPosPx());
				};

				body.setVelocity(ssvs::getVecFromRad(ssvu::getRndR<float>(0.f, ssvu::tau), ssvu::getRndR<float>(100.f, 370.f)));
				cDraw.setBlendMode(sf::BlendMode::BlendAdd);
				cDraw.setGlobalScale(0.65f);
				cDraw.setRotation(ssvu::getRnd(0, 360));
			}

			inline void update(FT) override { cDraw[0].rotate(ssvs::getMag(body.getVelocity()) * 0.01f); }
	};
}

#endif

