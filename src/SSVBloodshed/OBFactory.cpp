// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include "SSVBloodshed/OBFactory.h"
#include "SSVBloodshed/Components/OBCPhys.h"
#include "SSVBloodshed/Components/OBCDraw.h"
#include "SSVBloodshed/Components/OBCPlayer.h"
#include "SSVBloodshed/Components/OBCEnemy.h"
#include "SSVBloodshed/Components/OBCProjectile.h"
#include "SSVBloodshed/Components/OBCParticleEmitter.h"
#include "SSVBloodshed/Components/OBCParticleSystem.h"
#include "SSVBloodshed/Components/OBCFloor.h"
#include "SSVBloodshed/Components/OBCHealth.h"
#include "SSVBloodshed/Components/OBCEnemyTypes.h"
#include "SSVBloodshed/Components/OBCKillable.h"
#include "SSVBloodshed/Components/OBCTurret.h"
#include "SSVBloodshed/Components/OBCWpnController.h"
#include "SSVBloodshed/Components/OBCIdReceiver.h"
#include "SSVBloodshed/Components/OBCDoor.h"
#include "SSVBloodshed/Components/OBCPPlate.h"
#include "SSVBloodshed/Components/OBCTrapdoor.h"
#include "SSVBloodshed/Components/OBCShard.h"
#include "SSVBloodshed/Components/OBCSpawner.h"
#include "SSVBloodshed/Components/OBCDamageOnTouch.h"
#include "SSVBloodshed/Components/OBCVMachine.h"

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
	Sprite OBFactory::getSpriteByTile(const std::string& mTextureId, const IntRect& mRect) const					{ return {assets.get<Texture>(mTextureId), mRect}; }
	void OBFactory::emplaceSpriteByTile(OBCDraw& mCDraw, sf::Texture* mTexture, const sf::IntRect& mRect) const		{ mCDraw.emplaceSprite(*mTexture, mRect); }

	template<typename T> constexpr inline Entity& getEntity(const T& mTpl)				{ return std::get<0>(mTpl); }
	template<typename T> constexpr inline OBCPhys& getCPhys(const T& mTpl)				{ return std::get<1>(mTpl); }
	template<typename T> constexpr inline OBCDraw& getCDraw(const T& mTpl)				{ return std::get<2>(mTpl); }
	template<typename T> constexpr inline OBCHealth& getCHealth(const T& mTpl)			{ return std::get<3>(mTpl); }
	template<typename T> constexpr inline OBCProjectile& getCProjectile(const T& mTpl)	{ return std::get<3>(mTpl); }
	template<typename T> constexpr inline OBCKillable& getCKillable(const T& mTpl)		{ return std::get<4>(mTpl); }
	template<typename T> constexpr inline OBCEnemy& getCEnemy(const T& mTpl)			{ return std::get<5>(mTpl); }



	std::tuple<Entity&, OBCPhys&, OBCDraw&> OBFactory::createActorBase(const Vec2i& mPos, const Vec2i& mSize, int mDrawPriority, bool mStatic)
	{
		auto& result(createEntity(mDrawPriority));
		auto& cPhys(result.createComponent<OBCPhys>(game, mStatic, mPos, mSize));
		auto& cDraw(result.createComponent<OBCDraw>(game, cPhys.getBody()));
		return std::forward_as_tuple(result, cPhys, cDraw);
	}
	std::tuple<Entity&, OBCPhys&, OBCDraw&, OBCHealth&, OBCKillable&> OBFactory::createKillableBase(const Vec2i& mPos, const Vec2i& mSize, int mDrawPriority, float mHealth)
	{
		auto tpl(createActorBase(mPos, mSize, mDrawPriority));
		auto& cHealth(getEntity(tpl).createComponent<OBCHealth>(mHealth));
		auto& cKillable(getEntity(tpl).createComponent<OBCKillable>(getCPhys(tpl), cHealth, OBCKillable::Type::Organic));
		return std::forward_as_tuple(getEntity(tpl), getCPhys(tpl), getCDraw(tpl), cHealth, cKillable);
	}
	std::tuple<Entity&, OBCPhys&, OBCDraw&, OBCHealth&, OBCKillable&, OBCEnemy&> OBFactory::createEnemyBase(const Vec2i& mPos, const Vec2i& mSize, int mHealth)
	{
		auto tpl(createKillableBase(mPos, mSize, OBLayer::LEnemy, mHealth));
		auto& cTargeter(getEntity(tpl).createComponent<OBCTargeter>(getCPhys(tpl), OBGroup::GFriendly));
		auto& cBoid(getEntity(tpl).createComponent<OBCBoid>(getCPhys(tpl)));
		auto& cEnemy(getEntity(tpl).createComponent<OBCEnemy>(getCPhys(tpl), getCDraw(tpl), getCKillable(tpl), cTargeter, cBoid));
		getEntity(tpl).createComponent<OBCDamageOnTouch>(getCPhys(tpl), 1.f, OBGroup::GFriendlyKillable);
		return std::forward_as_tuple(getEntity(tpl), getCPhys(tpl), getCDraw(tpl), getCHealth(tpl), getCKillable(tpl), cEnemy);
	}
	std::tuple<Entity&, OBCPhys&, OBCDraw&, OBCProjectile&> OBFactory::createProjectileBase(const Vec2i& mPos, const Vec2i& mSize, float mSpeed, float mDeg, const IntRect& mIntRect)
	{
		auto tpl(createActorBase(mPos, mSize, OBLayer::LProjectile));
		emplaceSpriteByTile(getCDraw(tpl), assets.txSmall, mIntRect);
		auto& cProjectile(getEntity(tpl).createComponent<OBCProjectile>(getCPhys(tpl), getCDraw(tpl), mSpeed, mDeg));
		return std::forward_as_tuple(getEntity(tpl), getCPhys(tpl), getCDraw(tpl), cProjectile);
	}
	Entity& OBFactory::createETurretBase(const Vec2i& mPos, Dir8 mDir, const sf::IntRect& mIntRect, const OBWpnType& mWpn, float mShootDelay, float mPJDelay, int mShootCount)
	{
		auto tpl(createKillableBase(mPos, {1000, 1000}, OBLayer::LEnemy, 18));
		emplaceSpriteByTile(getCDraw(tpl), assets.txSmall, mIntRect);
		getCKillable(tpl).setType(OBCKillable::Type::Robotic);
		getCKillable(tpl).setParticleMult(0.35f);
		getEntity(tpl).createComponent<OBCTurret>(getCPhys(tpl), getCDraw(tpl), getCKillable(tpl), mDir, mWpn, mShootDelay, mPJDelay, mShootCount);
		return getEntity(tpl);
	}

	Entity& OBFactory::createParticleSystem(RenderTexture& mRenderTexture, bool mClearOnDraw, unsigned char mOpacity, int mDrawPriority, sf::BlendMode mBlendMode)
	{
		auto& result(createEntity(mDrawPriority));
		result.createComponent<OBCParticleSystem>(mRenderTexture, game.getGameWindow(), mClearOnDraw, mOpacity, mBlendMode);
		return result;
	}
	Entity& OBFactory::createTrail(const Vec2i& mA, const Vec2i& mB, const Color& mColor)
	{
		auto& result(manager.createEntity());
		result.createComponent<OBCTrail>(game, mA, mB, mColor);
		return result;
	}

	Entity& OBFactory::createFloor(const Vec2i& mPos, bool mGrate)
	{
		auto tpl(createActorBase(mPos, {1000, 1000}, OBLayer::LFloor));
		emplaceSpriteByTile(getCDraw(tpl), assets.txSmall, assets.getFloorVariant());
		getEntity(tpl).createComponent<OBCFloor>(getCPhys(tpl), getCDraw(tpl), mGrate);
		return getEntity(tpl);
	}
	Entity& OBFactory::createPit(const Vec2i& mPos)
	{
		auto tpl(createActorBase(mPos, {1000, 1000}, OBLayer::LPit, true));
		emplaceSpriteByTile(getCDraw(tpl), assets.txSmall, assets.pit);
		getCPhys(tpl).getBody().addGroups(OBGroup::GSolidGround, OBGroup::GPit);
		return getEntity(tpl);
	}
	Entity& OBFactory::createTrapdoor(const Vec2i& mPos, bool mPlayerOnly)
	{
		auto tpl(createActorBase(mPos, {1000, 1000}, OBLayer::LTrapdoor));
		emplaceSpriteByTile(getCDraw(tpl), assets.txSmall, mPlayerOnly ? assets.trapdoorPOnly : assets.trapdoor);
		getEntity(tpl).createComponent<OBCTrapdoor>(getCPhys(tpl), getCDraw(tpl), mPlayerOnly);
		return getEntity(tpl);
	}
	Entity& OBFactory::createWall(const Vec2i& mPos, const sf::IntRect& mIntRect)
	{
		auto tpl(createActorBase(mPos, {1000, 1000}, OBLayer::LWall, true));
		emplaceSpriteByTile(getCDraw(tpl), assets.txSmall, mIntRect);
		getCPhys(tpl).getBody().addGroups(OBGroup::GSolidGround, OBGroup::GSolidAir);
		return getEntity(tpl);
	}
	Entity& OBFactory::createWallDestructible(const Vec2i& mPos, const sf::IntRect& mIntRect)
	{
		auto tpl(createKillableBase(mPos, {1000, 1000}, OBLayer::LWall, 20));
		emplaceSpriteByTile(getCDraw(tpl), assets.txSmall, mIntRect);
		getCPhys(tpl).getBody().addGroups(OBGroup::GSolidGround, OBGroup::GSolidAir, OBGroup::GKillable, OBGroup::GFriendlyKillable, OBGroup::GEnemyKillable, OBGroup::GEnvDestructible);
		getCPhys(tpl).getBody().setStatic(true);
		getCKillable(tpl).setType(OBCKillable::Type::Wall);
		return getEntity(tpl);
	}
	Entity& OBFactory::createDoor(const Vec2i& mPos, const sf::IntRect& mIntRect, int mId, bool mOpen)
	{
		auto tpl(createActorBase(mPos, {1000, 1000}, OBLayer::LWall, true));
		emplaceSpriteByTile(getCDraw(tpl), assets.txSmall, mIntRect);
		auto& cIdReceiver(getEntity(tpl).createComponent<OBCIdReceiver>(mId));
		getEntity(tpl).createComponent<OBCDoor>(getCPhys(tpl), getCDraw(tpl), cIdReceiver, mOpen);
		return getEntity(tpl);
	}
	Entity& OBFactory::createDoorG(const Vec2i& mPos, const sf::IntRect& mIntRect, bool mOpen)
	{
		auto tpl(createActorBase(mPos, {1000, 1000}, OBLayer::LWall, true));
		emplaceSpriteByTile(getCDraw(tpl), assets.txSmall, mIntRect);
		getEntity(tpl).createComponent<OBCDoorG>(getCPhys(tpl), getCDraw(tpl), mOpen);
		return getEntity(tpl);
	}
	Entity& OBFactory::createDoorR(const Vec2i& mPos, const sf::IntRect& mIntRect, bool mOpen)
	{
		auto tpl(createActorBase(mPos, {1000, 1000}, OBLayer::LWall, true));
		emplaceSpriteByTile(getCDraw(tpl), assets.txSmall, mIntRect);
		getEntity(tpl).createComponent<OBCDoorR>(getCPhys(tpl), getCDraw(tpl), mOpen);
		return getEntity(tpl);
	}
	Entity& OBFactory::createPPlate(const Vec2i& mPos, int mId, PPlateType mType, IdAction mIdAction, bool mPlayerOnly)
	{
		auto tpl(createActorBase(mPos, {1000, 1000}, OBLayer::LFloor));
		const auto& intRect(mType == PPlateType::Single ? assets.pPlateSingle : (mType == PPlateType::Multi ? assets.pPlateMulti : assets.pPlateOnOff));
		emplaceSpriteByTile(getCDraw(tpl), assets.txSmall, intRect);
		getEntity(tpl).createComponent<OBCPPlate>(getCPhys(tpl), getCDraw(tpl), mId, mType, mIdAction, mPlayerOnly);
		return getEntity(tpl);
	}
	Entity& OBFactory::createPlayer(const Vec2i& mPos)
	{
		auto tpl(createKillableBase(mPos, {650, 650}, OBLayer::LPlayer, 10));
		emplaceSpriteByTile(getCDraw(tpl), assets.txSmall, assets.p1Stand);
		emplaceSpriteByTile(getCDraw(tpl), assets.txSmall, assets.p1Gun);
		auto& cDir8(getEntity(tpl).createComponent<OBCDir8>());
		auto& cWielder(getEntity(tpl).createComponent<OBCWielder>(getCPhys(tpl), getCDraw(tpl), cDir8, assets.p1Stand, assets.p1Shoot));
		auto& cWpnController(getEntity(tpl).createComponent<OBCWpnController>(getCPhys(tpl), OBGroup::GEnemyKillable));
		getEntity(tpl).createComponent<OBCPlayer>(getCPhys(tpl), getCDraw(tpl), getCKillable(tpl), cWielder, cWpnController);
		return getEntity(tpl);
	}
	Entity& OBFactory::createExplosiveCrate(const Vec2i& mPos, int mId)
	{
		auto tpl(createKillableBase(mPos, {1000, 1000}, OBLayer::LWall, 10));
		emplaceSpriteByTile(getCDraw(tpl), assets.txSmall, assets.explosiveCrate);
		auto& cIdReceiver(getEntity(tpl).createComponent<OBCIdReceiver>(mId));
		getCPhys(tpl).getBody().addGroups(OBGroup::GSolidGround, OBGroup::GSolidAir, OBGroup::GKillable, OBGroup::GFriendlyKillable, OBGroup::GEnemyKillable, OBGroup::GEnvDestructible);
		getCPhys(tpl).getBody().setStatic(true);
		getCKillable(tpl).setType(OBCKillable::Type::ExplosiveCrate);

		cIdReceiver.onActivate += [tpl](IdAction){ getCKillable(tpl).kill(); };
		getCKillable(tpl).onDeath += [this, tpl]
		{
			deathExplode(tpl, 16);
		};

		return getEntity(tpl);
	}
	Entity& OBFactory::createShard(const Vec2i& mPos)
	{
		auto tpl(createActorBase(mPos, {400, 400}, OBLayer::LShard));
		emplaceSpriteByTile(getCDraw(tpl), assets.txSmall, assets.shard);
		getEntity(tpl).createComponent<OBCShard>(getCPhys(tpl), getCDraw(tpl));
		return getEntity(tpl);
	}
	Entity& OBFactory::createSpawner(const Vec2i& mPos, int mType, int mId, float mDelayStart, float mDelaySpawn, int mSpawnCount)
	{
		auto tpl(createActorBase(mPos, {400, 400}, OBLayer::LShard));
		emplaceSpriteByTile(getCDraw(tpl), assets.txSmall, assets.spawner);
		auto& cIdReceiver(getEntity(tpl).createComponent<OBCIdReceiver>(mId));
		auto& cSpawner(getEntity(tpl).createComponent<OBCSpawner>(getCPhys(tpl), getCDraw(tpl), cIdReceiver, mType, mDelayStart, mDelaySpawn, mSpawnCount));

		if(mId != -1) cSpawner.setActive(false);
		return getEntity(tpl);
	}

	Entity& OBFactory::createEBall(const Vec2i& mPos, bool mFlying, bool mSmall)
	{
		auto tpl(createEnemyBase(mPos, {750, 750}, mSmall ? 2 : 5));
		emplaceSpriteByTile(getCDraw(tpl), assets.txSmall, mFlying ? assets.eBallFlying : assets.eBall);
		if(!mSmall && !mFlying) getEntity(tpl).createComponent<OBCFloorSmasher>(getCPhys(tpl)).setActive(true);
		getEntity(tpl).createComponent<OBCEBall>(getCEnemy(tpl), mFlying, mSmall);
		if(mSmall) getCDraw(tpl).setGlobalScale(0.7f);
		return getEntity(tpl);
	}
	Entity& OBFactory::createERunner(const Vec2i& mPos, bool mArmed)
	{
		auto tpl(createEnemyBase(mPos, {600, 600}, 4));
		emplaceSpriteByTile(getCDraw(tpl), assets.txSmall, assets.e1Shoot);
		emplaceSpriteByTile(getCDraw(tpl), assets.txSmall, assets.e1Gun);
		auto& cDir8(getEntity(tpl).createComponent<OBCDir8>());
		auto& cWielder(getEntity(tpl).createComponent<OBCWielder>(getCPhys(tpl), getCDraw(tpl), cDir8, assets.e1Stand, assets.e1Shoot));
		auto& cWpnController(getEntity(tpl).createComponent<OBCWpnController>(getCPhys(tpl), OBGroup::GFriendly));
		getEntity(tpl).createComponent<OBCERunner>(getCEnemy(tpl), cWielder, cWpnController, mArmed);
		return getEntity(tpl);
	}
	Entity& OBFactory::createECharger(const Vec2i& mPos, bool mArmed, bool mGL)
	{
		auto tpl(createEnemyBase(mPos, {1100, 1100}, 18));
		emplaceSpriteByTile(getCDraw(tpl), assets.txMedium, assets.e2Stand);
		emplaceSpriteByTile(getCDraw(tpl), assets.txMedium, assets.e2Gun);
		auto& cFloorSmasher(getEntity(tpl).createComponent<OBCFloorSmasher>(getCPhys(tpl)));
		auto& cDir8(getEntity(tpl).createComponent<OBCDir8>());
		auto& cWielder(getEntity(tpl).createComponent<OBCWielder>(getCPhys(tpl), getCDraw(tpl), cDir8, assets.e2Stand, assets.e2Shoot));
		auto& cWpnController(getEntity(tpl).createComponent<OBCWpnController>(getCPhys(tpl), OBGroup::GFriendly));
		auto type(OBCECharger::Type::Unarmed);
		if(mArmed) type = OBCECharger::Type::Plasma;
		if(mGL) type = OBCECharger::Type::Grenade;
		getEntity(tpl).createComponent<OBCECharger>(getCEnemy(tpl), cFloorSmasher, cWielder, cWpnController, type);
		return getEntity(tpl);
	}
	Entity& OBFactory::createEJuggernaut(const Vec2i& mPos, bool mArmed, bool mRL)
	{
		auto tpl(createEnemyBase(mPos, {1900, 1900}, 36));
		emplaceSpriteByTile(getCDraw(tpl), assets.txBig, assets.e3Stand);
		emplaceSpriteByTile(getCDraw(tpl), assets.txBig, assets.e3Gun);
		auto& cDir8(getEntity(tpl).createComponent<OBCDir8>());
		auto& cWielder(getEntity(tpl).createComponent<OBCWielder>(getCPhys(tpl), getCDraw(tpl), cDir8, assets.e3Stand, assets.e3Shoot));
		auto& cWpnController(getEntity(tpl).createComponent<OBCWpnController>(getCPhys(tpl), OBGroup::GFriendly));
		auto type(OBCEJuggernaut::Type::Unarmed);
		if(mArmed) type = OBCEJuggernaut::Type::Plasma;
		if(mRL) type = OBCEJuggernaut::Type::Rocket;
		getEntity(tpl).createComponent<OBCEJuggernaut>(getCEnemy(tpl), cWielder, cWpnController, type);
		return getEntity(tpl);
	}
	Entity& OBFactory::createEGiant(const Vec2i& mPos)
	{
		auto tpl(createEnemyBase(mPos, {2400, 2400}, 100));
		emplaceSpriteByTile(getCDraw(tpl), assets.txGiant, assets.e4Stand);
		getEntity(tpl).createComponent<OBCFloorSmasher>(getCPhys(tpl), true);
		getEntity(tpl).createComponent<OBCEGiant>(getCEnemy(tpl));
		return getEntity(tpl);
	}
	Entity& OBFactory::createEEnforcer(const Vec2i& mPos)
	{
		auto tpl(createEnemyBase(mPos, {1200, 1200}, 30));
		emplaceSpriteByTile(getCDraw(tpl), assets.txMedium, assets.e5Stand);
		getEntity(tpl).createComponent<OBCEEnforcer>(getCEnemy(tpl));
		return getEntity(tpl);
	}
	Entity& OBFactory::createETurretStarPlasma(const Vec2i& mPos, Dir8 mDir)
	{
		return createETurretBase(mPos, mDir, assets.eTurret0, OBWpnTypes::createEPlasmaStarGun(), 125.f, 5.f, 3);
	}
	Entity& OBFactory::createETurretCannonPlasma(const Vec2i& mPos, Dir8 mDir)
	{
		return createETurretBase(mPos, mDir, assets.eTurret1, OBWpnTypes::createPlasmaCannon(), 125.f, 5.f, 1);
	}
	Entity& OBFactory::createETurretBulletPlasma(const Vec2i& mPos, Dir8 mDir)
	{
		return createETurretBase(mPos, mDir, assets.eTurret2, OBWpnTypes::createEPlasmaBulletGun(1, 5.f), 125.f, 2.f, 4);
	}
	Entity& OBFactory::createETurretRocket(const Vec2i& mPos, Dir8 mDir)
	{
		return createETurretBase(mPos, mDir, assets.eTurret3, OBWpnTypes::createRocketLauncher(), 250.f, 0.f, 1);
	}



	Entity& OBFactory::createPJBullet(const Vec2i& mPos, float mDeg)
	{
		auto tpl(createProjectileBase(mPos, {150, 150}, 420.f, mDeg, assets.pjBullet));
		return getEntity(tpl);
	}
	Entity& OBFactory::createPJBulletPlasma(const Vec2i& mPos, float mDeg)
	{
		auto tpl(createProjectileBase(mPos, {150, 150}, 360.f, mDeg, assets.pjBulletPlasma));
		getEntity(tpl).createComponent<OBCParticleEmitter>(getCPhys(tpl), &OBGame::createPPlasma);
		getCDraw(tpl).setBlendMode(sf::BlendMode::BlendAdd);
		return getEntity(tpl);
	}
	Entity& OBFactory::createPJBoltPlasma(const Vec2i& mPos, float mDeg)
	{
		auto tpl(createProjectileBase(mPos, {150, 150}, 260.f, mDeg, assets.pjPlasma));
		getEntity(tpl).createComponent<OBCParticleEmitter>(getCPhys(tpl), &OBGame::createPPlasma);
		getCProjectile(tpl).setPierceOrganic(-1);
		getCDraw(tpl).setBlendMode(sf::BlendMode::BlendAdd);
		return getEntity(tpl);
	}
	Entity& OBFactory::createPJStar(const Vec2i& mPos, float mDeg)
	{
		auto tpl(createProjectileBase(mPos, {150, 150}, 320.f, mDeg, assets.pjStar));
		return getEntity(tpl);
	}
	Entity& OBFactory::createPJStarPlasma(const Vec2i& mPos, float mDeg)
	{
		auto tpl(createProjectileBase(mPos, {150, 150}, 270.f, mDeg, assets.pjStarPlasma));
		getEntity(tpl).createComponent<OBCParticleEmitter>(getCPhys(tpl), &OBGame::createPPlasma);
		getCDraw(tpl).setBlendMode(sf::BlendMode::BlendAdd);
		return getEntity(tpl);
	}
	Entity& OBFactory::createPJCannonPlasma(const Vec2i& mPos, float mDeg)
	{
		auto tpl(createProjectileBase(mPos, {150, 150}, 120.f, mDeg, assets.pjCannonPlasma));
		getEntity(tpl).createComponent<OBCFloorSmasher>(getCPhys(tpl)).setActive(true);
		getEntity(tpl).createComponent<OBCParticleEmitter>(getCPhys(tpl), &OBGame::createPPlasma, 5);
		getCProjectile(tpl).setPierceOrganic(-1);
		getCProjectile(tpl).setDamage(5);
		getCProjectile(tpl).onDestroy += [this, tpl]
		{
			for(int i{0}; i < 360; i += 360 / 8)
			{
				getCProjectile(tpl).createChild(createPJBoltPlasma(getCPhys(tpl).getPosI() + Vec2i(ssvs::getVecFromDeg<float>(i) * 300.f), i));
			}
		};
		getCDraw(tpl).setBlendMode(sf::BlendMode::BlendAdd);
		return getEntity(tpl);
	}
	Entity& OBFactory::createPJRocket(const Vec2i& mPos, float mDeg)
	{
		auto tpl(createProjectileBase(mPos, {150, 150}, 25.f, mDeg, assets.pjRocket));
		getEntity(tpl).createComponent<OBCParticleEmitter>(getCPhys(tpl), &OBGame::createPSmoke, 4);
		getCProjectile(tpl).setPierceOrganic(0);
		getCProjectile(tpl).setDamage(0);
		getCProjectile(tpl).setMaxSpeed(175.f);
		getCProjectile(tpl).setAcceleration(3.5f);
		getCProjectile(tpl).onDestroy += [this, tpl]
		{
			deathExplode(tpl, 16);
		};
		return getEntity(tpl);
	}
	Entity& OBFactory::createPJGrenade(const Vec2i& mPos, float mDeg)
	{
		auto tpl(createProjectileBase(mPos, {150, 150}, 180.f, mDeg, assets.pjGrenade));
		getEntity(tpl).createComponent<OBCParticleEmitter>(getCPhys(tpl), &OBGame::createPSmoke, 3);
		getCProjectile(tpl).setPierceOrganic(0);
		getCProjectile(tpl).setDamage(0);
		getCProjectile(tpl).setAcceleration(-1.f);
		getCProjectile(tpl).setBounce(true);
		getCProjectile(tpl).setFallInPit(true);
		getCProjectile(tpl).onDestroy += [this, tpl]
		{
			deathExplode(tpl, 16, 0.5f);
		};
		return getEntity(tpl);
	}
	Entity& OBFactory::createPJExplosion(const Vec2i& mPos, float mDeg, float mSpeed)
	{
		auto tpl(createProjectileBase(mPos, {500, 500}, mSpeed, mDeg, assets.null0));
		getEntity(tpl).createComponent<OBCFloorSmasher>(getCPhys(tpl)).setActive(true);
		getEntity(tpl).createComponent<OBCParticleEmitter>(getCPhys(tpl), &OBGame::createPExplosion, 2);
		getCProjectile(tpl).setPierceOrganic(-1);
		getCProjectile(tpl).setDamage(5);
		return getEntity(tpl);
	}


	Entity& OBFactory::createPJTestBomb(const Vec2i& mPos, float mDeg, float mSpeedMult, float mCurveMult)
	{
		auto tpl(createProjectileBase(mPos, {150, 150}, 150.f * mSpeedMult, mDeg, assets.pjStar));
		getEntity(tpl).createComponent<OBCFloorSmasher>(getCPhys(tpl)).setActive(true);
		getCProjectile(tpl).setCurveSpeed(2.f * mCurveMult);
		getCProjectile(tpl).setPierceOrganic(-1);
		getCProjectile(tpl).setDamage(10);
		getCProjectile(tpl).onDestroy += [this, tpl]
		{
			for(int i{0}; i < 360; i += 360 / 8)
			{
				getCProjectile(tpl).createChild(createPJBoltPlasma(getCPhys(tpl).getPosI() + Vec2i(ssvs::getVecFromDeg<float>(i) * 300.f), i));
			}
		};
		return getEntity(tpl);
	}
	Entity& OBFactory::createPJTestShell(const Vec2i& mPos, float mDeg)
	{
		auto tpl(createProjectileBase(mPos, {150, 150}, 320.f + getRnd(-5, 25), mDeg, assets.pjBullet));
		getCProjectile(tpl).setLife(10.f + getRnd(-5, 15));
		getCProjectile(tpl).setPierceOrganic(3);
		return getEntity(tpl);
	}

	Entity& OBFactory::createVMHealth(const Vec2i& mPos)
	{
		auto tpl(createActorBase(mPos, {1000, 1000}, OBLayer::LWall, false));
		emplaceSpriteByTile(getCDraw(tpl), assets.txSmall, assets.vmHealth);
		getCPhys(tpl).getBody().addGroups(OBGroup::GSolidGround, OBGroup::GSolidAir);
		getEntity(tpl).createComponent<OBCVMachine>(getCPhys(tpl), getCDraw(tpl));
		return getEntity(tpl);
	}
}
