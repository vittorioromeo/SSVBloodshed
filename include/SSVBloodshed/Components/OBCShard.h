// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_SHARD
#define SSVOB_COMPONENTS_SHARD

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCActorBase.h"

namespace ob
{
	class OBCShard : public OBCActorBase
	{
		public:
			OBCShard(OBCPhys& mCPhys, OBCDraw& mCDraw) noexcept : OBCActorBase{mCPhys, mCDraw} { }

			inline void init() override
			{
				getEntity().addGroup(OBGroup::GShard);
				body.addGroup(OBGroup::GShard);
				body.addGroupsToCheck(OBGroup::GSolidGround, OBGroup::GFriendly);
				body.addGroupNoResolve(OBGroup::GOrganic);
				body.setRestitutionX(0.8f);
				body.setRestitutionY(0.8f);
				body.onPreUpdate += [this]{ body.setVelocity(ssvs::getCClampedMax(body.getVelocity() * 0.99f, 600.f)); };
				body.onDetection += [this](const DetectionInfo& mDI)
				{
					if(!mDI.body.hasGroup(OBGroup::GPlayer)) return;

					getEntityFromBody(mDI.body).getComponent<OBCPlayer>().shardGrabbed();
					getEntity().destroy(); game.createPShard(20, cPhys.getPosPx());
				};

				body.setVelocity(ssvs::getVecFromDeg(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(100.f, 370.f)));
				cDraw.setBlendMode(sf::BlendMode::BlendAdd);
				cDraw.setGlobalScale({0.65f + ssvu::getRndR(-0.10f, 0.10f), 0.65f + ssvu::getRndR(-0.10f, 0.10f)});
				cDraw.setRotation(ssvu::getRnd(0, 360));
			}

			inline void update(float) override { cDraw[0].rotate(ssvs::getMagnitude(body.getVelocity()) * 0.01f); }
	};
}

#endif

