// Copyright (c) 2013-2015 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include "SSVBloodshed/OBFactory.hpp"
#include "SSVBloodshed/Components/OBCPhys.hpp"
#include "SSVBloodshed/Components/OBCDraw.hpp"
#include "SSVBloodshed/Components/OBCPlayer.hpp"
#include "SSVBloodshed/Components/OBCEnemy.hpp"
#include "SSVBloodshed/Components/OBCProjectile.hpp"
#include "SSVBloodshed/Components/OBCParticleEmitter.hpp"
#include "SSVBloodshed/Components/OBCParticleSystem.hpp"
#include "SSVBloodshed/Components/OBCFloor.hpp"
#include "SSVBloodshed/Components/OBCHealth.hpp"
#include "SSVBloodshed/Components/OBCEnemyTypes.hpp"
#include "SSVBloodshed/Components/OBCKillable.hpp"
#include "SSVBloodshed/Components/OBCTurret.hpp"
#include "SSVBloodshed/Components/OBCWpnController.hpp"
#include "SSVBloodshed/Components/OBCIdReceiver.hpp"
#include "SSVBloodshed/Components/OBCDoor.hpp"
#include "SSVBloodshed/Components/OBCPPlate.hpp"
#include "SSVBloodshed/Components/OBCTrapdoor.hpp"
#include "SSVBloodshed/Components/OBCShard.hpp"
#include "SSVBloodshed/Components/OBCSpawner.hpp"
#include "SSVBloodshed/Components/OBCDamageOnTouch.hpp"
#include "SSVBloodshed/Components/OBCUsable.hpp"
#include "SSVBloodshed/Components/OBCVMachine.hpp"

using namespace std;
using namespace sf;
using namespace ssvu;
using namespace ssvu::FileSystem;
using namespace ssvs;
using namespace ssvsc;
using namespace ssvsc::Utils;
using namespace sses;

namespace ob
{
Sprite OBFactory::getSpriteByTile(
const std::string& mTextureId, const IntRect& mRect) const
{
    return {assets.get<Texture>(mTextureId), mRect};
}
void OBFactory::emplaceSpriteByTile(
OBCDraw& mCDraw, sf::Texture* mTexture, const sf::IntRect& mRect) const
{
    mCDraw.emplaceSprite(*mTexture, mRect);
}



auto OBFactory::createActorBase(
const Vec2i& mPos, const Vec2i& mSize, int mDrawPriority, bool mStatic)
{
    auto& result(createEntity(mDrawPriority));
    auto& cPhys(result.createComponent<OBCPhys>(game, mStatic, mPos, mSize));
    auto& cDraw(result.createComponent<OBCDraw>(game, cPhys.getBody()));
    return ssvu::fwdAsTpl(result, cPhys, cDraw);
}
auto OBFactory::createKillableBase(
const Vec2i& mPos, const Vec2i& mSize, int mDrawPriority, float mHealth)
{
    auto tpl(createActorBase(mPos, mSize, mDrawPriority));
    auto& cHealth(gt<Entity>(tpl).createComponent<OBCHealth>(mHealth));
    auto& cKillable(gt<Entity>(tpl).createComponent<OBCKillable>(
    gt<OBCPhys>(tpl), cHealth, OBCKillable::Type::Organic));
    return ssvu::fwdAsTpl(
    gt<Entity>(tpl), gt<OBCPhys>(tpl), gt<OBCDraw>(tpl), cHealth, cKillable);
}
auto OBFactory::createEnemyBase(
const Vec2i& mPos, const Vec2i& mSize, int mHealth)
{
    auto tpl(createKillableBase(mPos, mSize, OBLayer::LEnemy, mHealth));
    auto& cTargeter(gt<Entity>(tpl).createComponent<OBCTargeter>(
    gt<OBCPhys>(tpl), OBGroup::GFriendly));
    auto& cBoid(gt<Entity>(tpl).createComponent<OBCBoid>(gt<OBCPhys>(tpl)));
    auto& cEnemy(gt<Entity>(tpl).createComponent<OBCEnemy>(gt<OBCPhys>(tpl),
    gt<OBCDraw>(tpl), gt<OBCKillable>(tpl), cTargeter, cBoid));
    gt<Entity>(tpl).createComponent<OBCDamageOnTouch>(
    gt<OBCPhys>(tpl), 1.f, OBGroup::GFriendlyKillable);
    return ssvu::fwdAsTpl(gt<Entity>(tpl), gt<OBCPhys>(tpl), gt<OBCDraw>(tpl),
    gt<OBCHealth>(tpl), gt<OBCKillable>(tpl), cEnemy);
}
auto OBFactory::createProjectileBase(OBCActorND* mShooter, const Vec2i& mPos,
const Vec2i& mSize, float mSpeed, float mDeg, const IntRect& mIntRect)
{
    auto tpl(createActorBase(mPos, mSize, OBLayer::LProjectile));
    emplaceSpriteByTile(gt<OBCDraw>(tpl), assets.txSmall, mIntRect);
    auto& cProjectile(gt<Entity>(tpl).createComponent<OBCProjectile>(
    mShooter, gt<OBCPhys>(tpl), gt<OBCDraw>(tpl), mSpeed, mDeg));
    return ssvu::fwdAsTpl(
    gt<Entity>(tpl), gt<OBCPhys>(tpl), gt<OBCDraw>(tpl), cProjectile);
}
Entity& OBFactory::createETurretBase(const Vec2i& mPos, Dir8 mDir,
const sf::IntRect& mIntRect, const OBWpnType& mWpn, float mShootDelay,
float mPJDelay, int mShootCount)
{
    auto tpl(createKillableBase(mPos, {1000, 1000}, OBLayer::LEnemy, 18));
    emplaceSpriteByTile(gt<OBCDraw>(tpl), assets.txSmall, mIntRect);
    gt<OBCKillable>(tpl).setType(OBCKillable::Type::Robotic);
    gt<OBCKillable>(tpl).setParticleMult(0.35f);
    gt<Entity>(tpl).createComponent<OBCTurret>(gt<OBCPhys>(tpl),
    gt<OBCDraw>(tpl), gt<OBCKillable>(tpl), mDir, mWpn, mShootDelay, mPJDelay,
    mShootCount);
    return gt<Entity>(tpl);
}

Entity& OBFactory::createParticleSystem(RenderTexture& mRenderTexture,
bool mClearOnDraw, unsigned char mOpacity, int mDrawPriority,
sf::BlendMode mBlendMode)
{
    auto& result(createEntity(mDrawPriority));
    result.createComponent<OBCParticleSystem>(
    mRenderTexture, game.getGameWindow(), mClearOnDraw, mOpacity, mBlendMode);
    return result;
}
Entity& OBFactory::createTrail(
const Vec2i& mA, const Vec2i& mB, const Color& mColor)
{
    auto& result(manager.createEntity());
    result.createComponent<OBCTrail>(game, mA, mB, mColor);
    return result;
}

Entity& OBFactory::createFloor(const Vec2i& mPos, bool mGrate)
{
    auto tpl(createActorBase(mPos, {1000, 1000}, OBLayer::LFloor));
    emplaceSpriteByTile(
    gt<OBCDraw>(tpl), assets.txSmall, assets.getFloorVariant());
    gt<Entity>(tpl).createComponent<OBCFloor>(
    gt<OBCPhys>(tpl), gt<OBCDraw>(tpl), mGrate);
    return gt<Entity>(tpl);
}
Entity& OBFactory::createPit(const Vec2i& mPos)
{
    auto tpl(createActorBase(mPos, {1000, 1000}, OBLayer::LPit, true));
    emplaceSpriteByTile(gt<OBCDraw>(tpl), assets.txSmall, assets.pit);
    gt<OBCPhys>(tpl).getBody().addGroups(OBGroup::GSolidGround, OBGroup::GPit);
    return gt<Entity>(tpl);
}
Entity& OBFactory::createTrapdoor(const Vec2i& mPos, bool mPlayerOnly)
{
    auto tpl(createActorBase(mPos, {1000, 1000}, OBLayer::LTrapdoor));
    emplaceSpriteByTile(gt<OBCDraw>(tpl), assets.txSmall,
    mPlayerOnly ? assets.trapdoorPOnly : assets.trapdoor);
    gt<Entity>(tpl).createComponent<OBCTrapdoor>(
    gt<OBCPhys>(tpl), gt<OBCDraw>(tpl), mPlayerOnly);
    return gt<Entity>(tpl);
}
Entity& OBFactory::createWall(const Vec2i& mPos, const sf::IntRect& mIntRect)
{
    auto tpl(createActorBase(mPos, {1000, 1000}, OBLayer::LWall, true));
    emplaceSpriteByTile(gt<OBCDraw>(tpl), assets.txSmall, mIntRect);
    gt<OBCPhys>(tpl).getBody().addGroups(
    OBGroup::GSolidGround, OBGroup::GSolidAir);
    return gt<Entity>(tpl);
}
Entity& OBFactory::createWallDestructible(
const Vec2i& mPos, const sf::IntRect& mIntRect)
{
    auto tpl(createKillableBase(mPos, {1000, 1000}, OBLayer::LWall, 20));
    emplaceSpriteByTile(gt<OBCDraw>(tpl), assets.txSmall, mIntRect);
    gt<OBCPhys>(tpl).getBody().addGroups(OBGroup::GSolidGround,
    OBGroup::GSolidAir, OBGroup::GKillable, OBGroup::GFriendlyKillable,
    OBGroup::GEnemyKillable, OBGroup::GEnvDestructible);
    gt<OBCPhys>(tpl).getBody().setStatic(true);
    gt<OBCKillable>(tpl).setType(OBCKillable::Type::Wall);
    return gt<Entity>(tpl);
}
Entity& OBFactory::createDoor(
const Vec2i& mPos, const sf::IntRect& mIntRect, int mId, bool mOpen)
{
    auto tpl(createActorBase(mPos, {1000, 1000}, OBLayer::LWall, true));
    emplaceSpriteByTile(gt<OBCDraw>(tpl), assets.txSmall, mIntRect);
    auto& cIdReceiver(gt<Entity>(tpl).createComponent<OBCIdReceiver>(mId));
    gt<Entity>(tpl).createComponent<OBCDoor>(
    gt<OBCPhys>(tpl), gt<OBCDraw>(tpl), cIdReceiver, mOpen);
    return gt<Entity>(tpl);
}
Entity& OBFactory::createDoorG(
const Vec2i& mPos, const sf::IntRect& mIntRect, bool mOpen)
{
    auto tpl(createActorBase(mPos, {1000, 1000}, OBLayer::LWall, true));
    emplaceSpriteByTile(gt<OBCDraw>(tpl), assets.txSmall, mIntRect);
    gt<Entity>(tpl).createComponent<OBCDoorG>(
    gt<OBCPhys>(tpl), gt<OBCDraw>(tpl), mOpen);
    return gt<Entity>(tpl);
}
Entity& OBFactory::createDoorR(
const Vec2i& mPos, const sf::IntRect& mIntRect, bool mOpen)
{
    auto tpl(createActorBase(mPos, {1000, 1000}, OBLayer::LWall, true));
    emplaceSpriteByTile(gt<OBCDraw>(tpl), assets.txSmall, mIntRect);
    gt<Entity>(tpl).createComponent<OBCDoorR>(
    gt<OBCPhys>(tpl), gt<OBCDraw>(tpl), mOpen);
    return gt<Entity>(tpl);
}
Entity& OBFactory::createPPlate(const Vec2i& mPos, int mId, PPlateType mType,
IdAction mIdAction, bool mPlayerOnly)
{
    auto tpl(createActorBase(mPos, {1000, 1000}, OBLayer::LFloor));
    const auto& intRect(
    mType == PPlateType::Single
    ? assets.pPlateSingle
    : (mType == PPlateType::Multi ? assets.pPlateMulti : assets.pPlateOnOff));
    emplaceSpriteByTile(gt<OBCDraw>(tpl), assets.txSmall, intRect);
    gt<Entity>(tpl).createComponent<OBCPPlate>(
    gt<OBCPhys>(tpl), gt<OBCDraw>(tpl), mId, mType, mIdAction, mPlayerOnly);
    return gt<Entity>(tpl);
}
Entity& OBFactory::createPlayer(const Vec2i& mPos)
{
    auto tpl(createKillableBase(mPos, {650, 650}, OBLayer::LPlayer, 10));
    emplaceSpriteByTile(gt<OBCDraw>(tpl), assets.txSmall, assets.p1Stand);
    emplaceSpriteByTile(gt<OBCDraw>(tpl), assets.txSmall, assets.p1Gun);
    auto& cDir8(gt<Entity>(tpl).createComponent<OBCDir8>());
    auto& cWielder(gt<Entity>(tpl).createComponent<OBCWielder>(
    gt<OBCPhys>(tpl), gt<OBCDraw>(tpl), cDir8, assets.p1Stand, assets.p1Shoot));
    auto& cWpnController(gt<Entity>(tpl).createComponent<OBCWpnController>(
    gt<OBCPhys>(tpl), OBGroup::GEnemyKillable));
    gt<Entity>(tpl).createComponent<OBCPlayer>(gt<OBCPhys>(tpl),
    gt<OBCDraw>(tpl), gt<OBCKillable>(tpl), cWielder, cWpnController);
    return gt<Entity>(tpl);
}
Entity& OBFactory::createExplosiveCrate(const Vec2i& mPos, int mId)
{
    auto tpl(createKillableBase(mPos, {1000, 1000}, OBLayer::LWall, 10));
    emplaceSpriteByTile(
    gt<OBCDraw>(tpl), assets.txSmall, assets.explosiveCrate);
    auto& cIdReceiver(gt<Entity>(tpl).createComponent<OBCIdReceiver>(mId));
    gt<OBCPhys>(tpl).getBody().addGroups(OBGroup::GSolidGround,
    OBGroup::GSolidAir, OBGroup::GKillable, OBGroup::GFriendlyKillable,
    OBGroup::GEnemyKillable, OBGroup::GEnvDestructible);
    gt<OBCPhys>(tpl).getBody().setStatic(true);
    gt<OBCKillable>(tpl).setType(OBCKillable::Type::ExplosiveCrate);

    cIdReceiver.onActivate += [tpl](IdAction)
    {
        gt<OBCKillable>(tpl).kill();
    };
    gt<OBCKillable>(tpl).onDeath += [this, tpl]
    {
        deathExplode(nullptr, tpl, 16);
    };

    return gt<Entity>(tpl);
}
Entity& OBFactory::createShard(const Vec2i& mPos)
{
    auto tpl(createActorBase(mPos, {400, 400}, OBLayer::LShard));
    emplaceSpriteByTile(gt<OBCDraw>(tpl), assets.txSmall, assets.shard);
    gt<Entity>(tpl).createComponent<OBCShard>(
    gt<OBCPhys>(tpl), gt<OBCDraw>(tpl));
    return gt<Entity>(tpl);
}
Entity& OBFactory::createSpawner(const Vec2i& mPos, SpawnerItem mType, int mId,
float mDelayStart, float mDelaySpawn, int mSpawnCount)
{
    auto tpl(createActorBase(mPos, {400, 400}, OBLayer::LShard));
    emplaceSpriteByTile(gt<OBCDraw>(tpl), assets.txSmall, assets.spawner);
    auto& cIdReceiver(gt<Entity>(tpl).createComponent<OBCIdReceiver>(mId));
    auto& cSpawner(gt<Entity>(tpl).createComponent<OBCSpawner>(gt<OBCPhys>(tpl),
    gt<OBCDraw>(tpl), cIdReceiver, mType, mDelayStart, mDelaySpawn,
    mSpawnCount));

    if(mId != -1) cSpawner.setActive(false);
    return gt<Entity>(tpl);
}
Entity& OBFactory::createForceField(const Vec2i& mPos, int mId, Dir8 mDir,
bool mBlockFriendly, bool mBlockEnemy, float mForceMult)
{
    auto tpl(createActorBase(mPos, {1000, 1000}, OBLayer::LWall, false));
    emplaceSpriteByTile(gt<OBCDraw>(tpl), assets.txSmall, assets.ff0);
    auto& cIdReceiver(gt<Entity>(tpl).createComponent<OBCIdReceiver>(mId));
    gt<Entity>(tpl).createComponent<OBCForceField>(gt<OBCPhys>(tpl),
    gt<OBCDraw>(tpl), cIdReceiver, mDir, mBlockFriendly, mBlockEnemy,
    mForceMult);
    gt<OBCDraw>(tpl).setBlendMode(sf::BlendAdd);
    sf::Color color{225, 0, 0, 255};
    color.g = 255 * ssvu::toInt(mBlockFriendly);
    color.b = 255 * ssvu::toInt(mBlockEnemy);

    if(!mBlockFriendly && !mBlockEnemy) {
        color = {255, 255, 255, 245};
        gt<OBCDraw>(tpl)[0].setTextureRect(
        mForceMult > 0 ? assets.bulletBooster : assets.bulletChanger);
    }

    gt<OBCDraw>(tpl)[0].setColor(color);
    gt<OBCDraw>(tpl).setRotation(getDegFromDir8(mDir));
    return gt<Entity>(tpl);
}
Entity& OBFactory::createBulletForceField(
const Vec2i& mPos, int mId, Dir8 mDir, bool mBlockFriendly, bool mBlockEnemy)
{
    auto tpl(createActorBase(mPos, {1000, 1000}, OBLayer::LWall, false));
    emplaceSpriteByTile(
    gt<OBCDraw>(tpl), assets.txSmall, assets.forceArrowMark);
    auto& cIdReceiver(gt<Entity>(tpl).createComponent<OBCIdReceiver>(mId));
    gt<Entity>(tpl).createComponent<OBCBulletForceField>(gt<OBCPhys>(tpl),
    gt<OBCDraw>(tpl), cIdReceiver, mDir, mBlockFriendly, mBlockEnemy);
    gt<OBCDraw>(tpl).setBlendMode(sf::BlendAdd);

    sf::Color color{255, 0, 0, 255};
    color.g = 255 * ssvu::toInt(mBlockFriendly);
    color.b = 255 * ssvu::toInt(mBlockEnemy);

    gt<OBCDraw>(tpl)[0].setColor(color);
    gt<OBCDraw>(tpl).setRotation(getDegFromDir8(mDir));
    return gt<Entity>(tpl);
}
Entity& OBFactory::createBooster(
const Vec2i& mPos, int mId, Dir8 mDir, float mForceMult)
{
    auto tpl(createActorBase(mPos, {1000, 1000}, OBLayer::LWall, false));
    emplaceSpriteByTile(gt<OBCDraw>(tpl), assets.txSmall, assets.ff0);
    auto& cIdReceiver(gt<Entity>(tpl).createComponent<OBCIdReceiver>(mId));
    gt<Entity>(tpl).createComponent<OBCBooster>(
    gt<OBCPhys>(tpl), gt<OBCDraw>(tpl), cIdReceiver, mDir, mForceMult);
    gt<OBCDraw>(tpl).setBlendMode(sf::BlendAdd);

    gt<OBCDraw>(tpl)[0].setTextureRect(
    mForceMult > 0 ? assets.bulletBooster : assets.bulletChanger);
    gt<OBCDraw>(tpl)[0].setColor(sf::Color{255, 255, 255, 245});
    gt<OBCDraw>(tpl).setRotation(getDegFromDir8(mDir));
    return gt<Entity>(tpl);
}

Entity& OBFactory::createEBall(const Vec2i& mPos, BallType mType, bool mSmall)
{
    auto tpl(createEnemyBase(mPos, {750, 750}, mSmall ? 2 : 5));
    emplaceSpriteByTile(gt<OBCDraw>(tpl), assets.txSmall,
    mType == BallType::Flying ? assets.eBallFlying : assets.eBall);
    if(!mSmall && mType != BallType::Flying)
        gt<Entity>(tpl)
        .createComponent<OBCFloorSmasher>(gt<OBCPhys>(tpl))
        .setActive(true);
    gt<Entity>(tpl).createComponent<OBCEBall>(gt<OBCEnemy>(tpl), mType, mSmall);
    if(mSmall) gt<OBCDraw>(tpl).setGlobalScale(0.7f);
    return gt<Entity>(tpl);
}
Entity& OBFactory::createERunner(const Vec2i& mPos, RunnerType mType)
{
    auto tpl(createEnemyBase(mPos, {600, 600}, 4));
    emplaceSpriteByTile(gt<OBCDraw>(tpl), assets.txSmall, assets.e1Shoot);
    emplaceSpriteByTile(gt<OBCDraw>(tpl), assets.txSmall, assets.e1Gun);
    auto& cDir8(gt<Entity>(tpl).createComponent<OBCDir8>());
    auto& cWielder(gt<Entity>(tpl).createComponent<OBCWielder>(
    gt<OBCPhys>(tpl), gt<OBCDraw>(tpl), cDir8, assets.e1Stand, assets.e1Shoot));
    auto& cWpnController(gt<Entity>(tpl).createComponent<OBCWpnController>(
    gt<OBCPhys>(tpl), OBGroup::GFriendlyKillable));
    gt<Entity>(tpl).createComponent<OBCERunner>(
    gt<OBCEnemy>(tpl), cWielder, cWpnController, mType);
    return gt<Entity>(tpl);
}
Entity& OBFactory::createECharger(const Vec2i& mPos, ChargerType mType)
{
    auto tpl(createEnemyBase(mPos, {1100, 1100}, 18));
    emplaceSpriteByTile(gt<OBCDraw>(tpl), assets.txMedium, assets.e2Stand);
    emplaceSpriteByTile(gt<OBCDraw>(tpl), assets.txMedium, assets.e2Gun);
    auto& cFloorSmasher(
    gt<Entity>(tpl).createComponent<OBCFloorSmasher>(gt<OBCPhys>(tpl)));
    auto& cDir8(gt<Entity>(tpl).createComponent<OBCDir8>());
    auto& cWielder(gt<Entity>(tpl).createComponent<OBCWielder>(
    gt<OBCPhys>(tpl), gt<OBCDraw>(tpl), cDir8, assets.e2Stand, assets.e2Shoot));
    auto& cWpnController(gt<Entity>(tpl).createComponent<OBCWpnController>(
    gt<OBCPhys>(tpl), OBGroup::GFriendlyKillable));
    gt<Entity>(tpl).createComponent<OBCECharger>(
    gt<OBCEnemy>(tpl), cFloorSmasher, cWielder, cWpnController, mType);
    return gt<Entity>(tpl);
}
Entity& OBFactory::createEJuggernaut(const Vec2i& mPos, JuggernautType mType)
{
    auto tpl(createEnemyBase(mPos, {1900, 1900}, 36));
    emplaceSpriteByTile(gt<OBCDraw>(tpl), assets.txBig, assets.e3Stand);
    emplaceSpriteByTile(gt<OBCDraw>(tpl), assets.txBig, assets.e3Gun);
    auto& cDir8(gt<Entity>(tpl).createComponent<OBCDir8>());
    auto& cWielder(gt<Entity>(tpl).createComponent<OBCWielder>(
    gt<OBCPhys>(tpl), gt<OBCDraw>(tpl), cDir8, assets.e3Stand, assets.e3Shoot));
    auto& cWpnController(gt<Entity>(tpl).createComponent<OBCWpnController>(
    gt<OBCPhys>(tpl), OBGroup::GFriendlyKillable));
    gt<Entity>(tpl).createComponent<OBCEJuggernaut>(
    gt<OBCEnemy>(tpl), cWielder, cWpnController, mType);
    return gt<Entity>(tpl);
}
Entity& OBFactory::createEGiant(const Vec2i& mPos)
{
    auto tpl(createEnemyBase(mPos, {2400, 2400}, 100));
    emplaceSpriteByTile(gt<OBCDraw>(tpl), assets.txGiant, assets.e4Stand);
    gt<Entity>(tpl).createComponent<OBCFloorSmasher>(gt<OBCPhys>(tpl), true);
    gt<Entity>(tpl).createComponent<OBCEGiant>(gt<OBCEnemy>(tpl));
    return gt<Entity>(tpl);
}
Entity& OBFactory::createEEnforcer(const Vec2i& mPos)
{
    auto tpl(createEnemyBase(mPos, {1200, 1200}, 30));
    emplaceSpriteByTile(gt<OBCDraw>(tpl), assets.txMedium, assets.e5Stand);
    gt<Entity>(tpl).createComponent<OBCEEnforcer>(gt<OBCEnemy>(tpl));
    return gt<Entity>(tpl);
}
Entity& OBFactory::createETurretStarPlasma(const Vec2i& mPos, Dir8 mDir)
{
    return createETurretBase(mPos, mDir, assets.eTurret0,
    OBWpnTypes::createEPlasmaStarGun(), 125.f, 5.f, 3);
}
Entity& OBFactory::createETurretCannonPlasma(const Vec2i& mPos, Dir8 mDir)
{
    return createETurretBase(mPos, mDir, assets.eTurret1,
    OBWpnTypes::createPlasmaCannon(), 125.f, 5.f, 1);
}
Entity& OBFactory::createETurretBulletPlasma(const Vec2i& mPos, Dir8 mDir)
{
    return createETurretBase(mPos, mDir, assets.eTurret2,
    OBWpnTypes::createEPlasmaBulletGun(1, 5.f), 125.f, 2.f, 4);
}
Entity& OBFactory::createETurretRocket(const Vec2i& mPos, Dir8 mDir)
{
    return createETurretBase(mPos, mDir, assets.eTurret3,
    OBWpnTypes::createRocketLauncher(), 250.f, 0.f, 1);
}



Entity& OBFactory::createPJBullet(
OBCActorND* mShooter, const Vec2i& mPos, float mDeg)
{
    auto tpl(createProjectileBase(
    mShooter, mPos, {150, 150}, 420.f, mDeg, assets.pjBullet));
    return gt<Entity>(tpl);
}
Entity& OBFactory::createPJBulletPlasma(
OBCActorND* mShooter, const Vec2i& mPos, float mDeg)
{
    auto tpl(createProjectileBase(
    mShooter, mPos, {150, 150}, 360.f, mDeg, assets.pjBulletPlasma));
    gt<Entity>(tpl).createComponent<OBCParticleEmitter>(
    gt<OBCPhys>(tpl), &OBGame::createPPlasma);
    gt<OBCDraw>(tpl).setBlendMode(sf::BlendAdd);
    return gt<Entity>(tpl);
}
Entity& OBFactory::createPJBoltPlasma(
OBCActorND* mShooter, const Vec2i& mPos, float mDeg)
{
    auto tpl(createProjectileBase(
    mShooter, mPos, {150, 150}, 260.f, mDeg, assets.pjPlasma));
    gt<Entity>(tpl).createComponent<OBCParticleEmitter>(
    gt<OBCPhys>(tpl), &OBGame::createPPlasma);
    gt<OBCProjectile>(tpl).setPierceOrganic(4);
    gt<OBCDraw>(tpl).setBlendMode(sf::BlendAdd);
    return gt<Entity>(tpl);
}
Entity& OBFactory::createPJStar(
OBCActorND* mShooter, const Vec2i& mPos, float mDeg)
{
    auto tpl(createProjectileBase(
    mShooter, mPos, {150, 150}, 320.f, mDeg, assets.pjStar));
    return gt<Entity>(tpl);
}
Entity& OBFactory::createPJStarPlasma(
OBCActorND* mShooter, const Vec2i& mPos, float mDeg)
{
    auto tpl(createProjectileBase(
    mShooter, mPos, {150, 150}, 270.f, mDeg, assets.pjStarPlasma));
    gt<Entity>(tpl).createComponent<OBCParticleEmitter>(
    gt<OBCPhys>(tpl), &OBGame::createPPlasma);
    gt<OBCDraw>(tpl).setBlendMode(sf::BlendAdd);
    return gt<Entity>(tpl);
}
Entity& OBFactory::createPJCannonPlasma(
OBCActorND* mShooter, const Vec2i& mPos, float mDeg)
{
    auto tpl(createProjectileBase(
    mShooter, mPos, {150, 150}, 120.f, mDeg, assets.pjCannonPlasma));
    gt<Entity>(tpl)
    .createComponent<OBCFloorSmasher>(gt<OBCPhys>(tpl))
    .setActive(true);
    gt<Entity>(tpl).createComponent<OBCParticleEmitter>(
    gt<OBCPhys>(tpl), &OBGame::createPPlasma, 5);
    gt<OBCProjectile>(tpl).setPierceOrganic(-1);
    gt<OBCProjectile>(tpl).setDamage(5);
    gt<OBCProjectile>(tpl).onDestroy += [this, tpl, mShooter]
    {
        for(int i{0}; i < 360; i += 360 / 8) {
            auto& b(gt<OBCProjectile>(tpl).getCPhys().getBody());
            Vec2i spawnPoint(Vec2f(b.getOldPosition()) - b.getOldVelocity());
            gt<OBCProjectile>(tpl).createChild(createPJBoltPlasma(mShooter,
            spawnPoint + Vec2i(ssvs::getVecFromDeg<float>(i) * 300.f), i));
        }
    };
    gt<OBCDraw>(tpl).setBlendMode(sf::BlendAdd);
    return gt<Entity>(tpl);
}
Entity& OBFactory::createPJRocket(
OBCActorND* mShooter, const Vec2i& mPos, float mDeg)
{
    auto tpl(createProjectileBase(
    mShooter, mPos, {150, 150}, 25.f, mDeg, assets.pjRocket));
    gt<Entity>(tpl).createComponent<OBCParticleEmitter>(
    gt<OBCPhys>(tpl), &OBGame::createPSmoke, 4);
    gt<OBCProjectile>(tpl).setPierceOrganic(0);
    gt<OBCProjectile>(tpl).setDamage(0);
    gt<OBCProjectile>(tpl).setMaxSpeed(175.f);
    gt<OBCProjectile>(tpl).setAcceleration(3.5f);
    gt<OBCProjectile>(tpl).onDestroy += [this, tpl, mShooter]
    {
        deathExplode(mShooter, tpl, 16);
    };
    return gt<Entity>(tpl);
}
Entity& OBFactory::createPJGrenade(
OBCActorND* mShooter, const Vec2i& mPos, float mDeg)
{
    auto tpl(createProjectileBase(
    mShooter, mPos, {150, 150}, 180.f, mDeg, assets.pjGrenade));
    gt<Entity>(tpl).createComponent<OBCParticleEmitter>(
    gt<OBCPhys>(tpl), &OBGame::createPSmoke, 3);
    gt<OBCProjectile>(tpl).setPierceOrganic(0);
    gt<OBCProjectile>(tpl).setDamage(0);
    gt<OBCProjectile>(tpl).setAcceleration(-1.f);
    gt<OBCProjectile>(tpl).setBounce(true);
    gt<OBCProjectile>(tpl).setFallInPit(true);
    gt<OBCProjectile>(tpl).onDestroy += [this, tpl, mShooter]
    {
        deathExplode(mShooter, tpl, 16, 0.5f);
    };
    return gt<Entity>(tpl);
}
Entity& OBFactory::createPJExplosion(
OBCActorND* mShooter, const Vec2i& mPos, float mDeg, float mSpeed)
{
    auto tpl(createProjectileBase(
    mShooter, mPos, {500, 500}, mSpeed, mDeg, assets.null0));
    gt<Entity>(tpl)
    .createComponent<OBCFloorSmasher>(gt<OBCPhys>(tpl))
    .setActive(true);
    gt<Entity>(tpl).createComponent<OBCParticleEmitter>(
    gt<OBCPhys>(tpl), &OBGame::createPExplosion, 2);
    gt<OBCProjectile>(tpl).setPierceOrganic(-1);
    gt<OBCProjectile>(tpl).setDamage(5);
    return gt<Entity>(tpl);
}
Entity& OBFactory::createPJShockwave(
OBCActorND* mShooter, const Vec2i& mPos, float mDeg, int mNum)
{
    auto str(ssvu::getClampedMin(mNum, 1));
    auto tpl(createProjectileBase(
    mShooter, mPos, {150, 150}, 560.f - (mNum * 70), mDeg, assets.pjShockwave));
    gt<Entity>(tpl).createComponent<OBCParticleEmitter>(
    gt<OBCPhys>(tpl), &OBGame::createPShockwave, str);
    gt<OBCProjectile>(tpl).setPierceOrganic(
    ssvu::getClampedMax(str / 2 + 1, 2) + 1);
    gt<OBCProjectile>(tpl).setDamage(str / 2 + 1);
    gt<OBCProjectile>(tpl).setLife(16 + str * 11);
    if(mNum > 0) {
        gt<OBCProjectile>(tpl).onDestroy += [this, tpl, mDeg, mNum, mShooter]
        {
            auto offset(ssvs::getVecFromDeg(mDeg + 180, 360));
            int num{2};
            for(int i{0}; i < num; ++i)
                gt<OBCProjectile>(tpl).createChild(createPJShockwave(mShooter,
                gt<OBCPhys>(tpl).getPosI() + Vec2i(offset) +
                Vec2i(ssvs::getVecFromDeg<float>(i) * 100.f),
                mDeg + 90 + ((360.f / num) * i), mNum - 1));
        };
    }
    gt<OBCDraw>(tpl).setBlendMode(sf::BlendAdd);
    return gt<Entity>(tpl);
}


Entity& OBFactory::createPJTestBomb(OBCActorND* mShooter, const Vec2i& mPos,
float mDeg, float mSpeedMult, float mCurveMult)
{
    auto tpl(createProjectileBase(
    mShooter, mPos, {150, 150}, 150.f * mSpeedMult, mDeg, assets.pjStar));
    gt<Entity>(tpl)
    .createComponent<OBCFloorSmasher>(gt<OBCPhys>(tpl))
    .setActive(true);
    gt<OBCProjectile>(tpl).setCurveSpeed(0.04f * mCurveMult);
    gt<OBCProjectile>(tpl).setPierceOrganic(-1);
    gt<OBCProjectile>(tpl).setDamage(10);
    gt<OBCProjectile>(tpl).onDestroy += [this, tpl, mShooter]
    {
        for(int i{0}; i < 360; i += 360 / 8) {
            gt<OBCProjectile>(tpl).createChild(createPJBoltPlasma(mShooter,
            gt<OBCPhys>(tpl).getPosI() +
            Vec2i(ssvs::getVecFromDeg<float>(i) * 300.f),
            i));
        }
    };
    return gt<Entity>(tpl);
}
Entity& OBFactory::createPJTestShell(
OBCActorND* mShooter, const Vec2i& mPos, float mDeg)
{
    auto tpl(createProjectileBase(mShooter, mPos, {150, 150},
    320.f + getRndI(-5, 25), mDeg, assets.pjBullet));
    gt<OBCProjectile>(tpl).setLife(10.f + getRndI(-5, 15));
    gt<OBCProjectile>(tpl).setPierceOrganic(3);
    return gt<Entity>(tpl);
}

Entity& OBFactory::createVMHealth(const Vec2i& mPos)
{
    auto tpl(createActorBase(mPos, {1000, 1000}, OBLayer::LWall, false));
    emplaceSpriteByTile(gt<OBCDraw>(tpl), assets.txSmall, assets.vmHealth);
    gt<OBCPhys>(tpl).getBody().addGroups(
    OBGroup::GSolidGround, OBGroup::GSolidAir);
    auto& cUsable(gt<Entity>(tpl).createComponent<OBCUsable>(gt<OBCPhys>(tpl)));
    gt<Entity>(tpl).createComponent<OBCVMachine>(gt<OBCDraw>(tpl), cUsable);
    return gt<Entity>(tpl);
}
}
