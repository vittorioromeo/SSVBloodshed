// Copyright (c) 2013-2015 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_HEALTH_INL
#define SSVOB_COMPONENTS_HEALTH_INL

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBGame.hpp"
#include "SSVBloodshed/Components/OBCPlayer.hpp"

namespace ob
{
inline bool OBCHealth::damage(
sses::EntityStat mAttackerStat, OBCActorND* mAttacker, float mAmount) noexcept
{
    if(tckCooldown.isRunning() || isDead()) return false;
    health = ssvu::getClampedMin(health - mAmount, 0.f);

    if(isDead() && mAttacker != nullptr && getManager().isAlive(mAttackerStat))
    {
        if(mAttacker->getEntity().hasComponent<OBCPlayer>()) {
            mAttacker->getEntity().getComponent<OBCPlayer>().onKill(
            this->getEntity());
        }
    }

    tckCooldown.restart();
    if(getManager().isAlive(mAttackerStat)) onDamage(mAttacker);
    return true;
}
}

#endif
