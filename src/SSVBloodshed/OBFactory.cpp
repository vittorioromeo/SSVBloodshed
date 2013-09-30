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
	template<typename... TArgs> constexpr inline Entity& getEntity(const std::tuple<TArgs...>& mTuple)				{ return std::get<0>(mTuple); }
	template<typename... TArgs> constexpr inline OBCPhys& getCPhys(const std::tuple<TArgs...>& mTuple)				{ return std::get<1>(mTuple); }
	template<typename... TArgs> constexpr inline OBCDraw& getCDraw(const std::tuple<TArgs...>& mTuple)				{ return std::get<2>(mTuple); }
	template<typename... TArgs> constexpr inline OBCHealth& getCHealth(const std::tuple<TArgs...>& mTuple)			{ return std::get<3>(mTuple); }
	template<typename... TArgs> constexpr inline OBCProjectile& getCProjectile(const std::tuple<TArgs...>& mTuple)	{ return std::get<3>(mTuple); }
	template<typename... TArgs> constexpr inline OBCKillable& getCKillable(const std::tuple<TArgs...>& mTuple)		{ return std::get<4>(mTuple); }
	template<typename... TArgs> constexpr inline OBCEnemy& getCEnemy(const std::tuple<TArgs...>& mTuple)			{ return std::get<5>(mTuple); }

	std::tuple<Entity&, OBCPhys&, OBCDraw&> OBFactory::createActorBase(const Vec2i& mPos, const Vec2i& mSize, int mDrawPriority, bool mStatic)
	{
		auto& result(createEntity(mDrawPriority));
		auto& cPhys(result.createComponent<OBCPhys>(game, mStatic, mPos, mSize));
		auto& cDraw(result.createComponent<OBCDraw>(game, cPhys.getBody()));
		return std::forward_as_tuple(result, cPhys, cDraw);
	}
	std::tuple<Entity&, OBCPhys&, OBCDraw&, OBCHealth&, OBCKillable&> OBFactory::createKillableBase(const Vec2i& mPos, const Vec2i& mSize, int mDrawPriority, int mHealth)
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
		return std::forward_as_tuple(getEntity(tpl), getCPhys(tpl), getCDraw(tpl), getCHealth(tpl), getCKillable(tpl), cEnemy);
	}
	std::tuple<Entity&, OBCPhys&, OBCDraw&, OBCProjectile&> OBFactory::createProjectileBase(const Vec2i& mPos, const Vec2i& mSize, float mSpeed, float mDegrees, const IntRect& mIntRect)
	{
		auto tpl(createActorBase(mPos, mSize, OBLayer::LProjectile));
		auto& cProjectile(getEntity(tpl).createComponent<OBCProjectile>(getCPhys(tpl), getCDraw(tpl), mSpeed, mDegrees));
		emplaceSpriteByTile(getCDraw(tpl), assets.txSmall, mIntRect);
		return std::forward_as_tuple(getEntity(tpl), getCPhys(tpl), getCDraw(tpl), cProjectile);
	}
	Entity& OBFactory::createETurretBase(const Vec2i& mPos, Dir8 mDir, const sf::IntRect& mIntRect, const OBWpnType& mWpn, float mShootDelay, float mPJDelay, int mShootCount)
	{
		auto tpl(createKillableBase(mPos, {1000, 1000}, OBLayer::LEnemy, 18));
		getCKillable(tpl).setType(OBCKillable::Type::Robotic);
		getCKillable(tpl).setParticleMult(0.35f);
		getEntity(tpl).createComponent<OBCTurret>(getCPhys(tpl), getCDraw(tpl), getCKillable(tpl), mDir, mWpn, mShootDelay, mPJDelay, mShootCount);
		emplaceSpriteByTile(getCDraw(tpl), assets.txSmall, mIntRect);
		return getEntity(tpl);
	}

	Entity& OBFactory::createParticleSystem(RenderTexture& mRenderTexture, bool mClearOnDraw, unsigned char mOpacity, int mDrawPriority, sf::BlendMode mBlendMode)
	{
		auto& result(createEntity(mDrawPriority));
		result.createComponent<OBCParticleSystem>(mRenderTexture, game.getGameWindow(), mClearOnDraw, mOpacity, mBlendMode);
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
		getCPhys(tpl).getBody().addGroup(OBGroup::GSolidGround);
		emplaceSpriteByTile(getCDraw(tpl), assets.txSmall, assets.pit);
		return getEntity(tpl);
	}
	Entity& OBFactory::createWall(const Vec2i& mPos, const sf::IntRect& mIntRect)
	{
		auto tpl(createActorBase(mPos, {1000, 1000}, OBLayer::LWall, true));
		getCPhys(tpl).getBody().addGroup(OBGroup::GSolidGround);
		getCPhys(tpl).getBody().addGroup(OBGroup::GSolidAir);
		emplaceSpriteByTile(getCDraw(tpl), assets.txSmall, mIntRect);
		return getEntity(tpl);
	}
	Entity& OBFactory::createWallDestructible(const Vec2i& mPos, const sf::IntRect& mIntRect)
	{
		auto tpl(createKillableBase(mPos, {1000, 1000}, OBLayer::LWall, 20));
		getCPhys(tpl).getBody().addGroup(OBGroup::GSolidGround);
		getCPhys(tpl).getBody().addGroup(OBGroup::GSolidAir);
		getCPhys(tpl).getBody().addGroup(OBGroup::GFriendly);
		getCPhys(tpl).getBody().addGroup(OBGroup::GEnemy);
		getCPhys(tpl).getBody().setStatic(true);
		getCKillable(tpl).setType(OBCKillable::Type::Wall);
		emplaceSpriteByTile(getCDraw(tpl), assets.txSmall, mIntRect);
		return getEntity(tpl);
	}
	Entity& OBFactory::createDoor(const Vec2i& mPos, const sf::IntRect& mIntRect, int mId, bool mOpen)
	{
		auto tpl(createActorBase(mPos, {1000, 1000}, OBLayer::LWall, true));
		auto& cIdReceiver(getEntity(tpl).createComponent<OBCIdReceiver>(mId));
		getEntity(tpl).createComponent<OBCDoor>(getCPhys(tpl), getCDraw(tpl), cIdReceiver, mOpen);
		emplaceSpriteByTile(getCDraw(tpl), assets.txSmall, mIntRect);
		return getEntity(tpl);
	}
	Entity& OBFactory::createPPlate(const Vec2i& mPos, int mId, PPlateType mType, OBIdAction mIdAction)
	{
		auto tpl(createActorBase(mPos, {1000, 1000}, OBLayer::LFloor));
		getEntity(tpl).createComponent<OBCPPlate>(getCPhys(tpl), getCDraw(tpl), mId, mType, mIdAction);
		const auto& intRect(mType == PPlateType::Single ? assets.pPlateSingle : assets.pPlateMulti);
		emplaceSpriteByTile(getCDraw(tpl), assets.txSmall, intRect);
		return getEntity(tpl);
	}
	Entity& OBFactory::createPlayer(const Vec2i& mPos)
	{
		auto tpl(createKillableBase(mPos, {650, 650}, OBLayer::LPlayer, 10));
		auto& cDir8(getEntity(tpl).createComponent<OBCDir8>());
		auto& cWielder(getEntity(tpl).createComponent<OBCWielder>(getCPhys(tpl), getCDraw(tpl), cDir8));
		auto& cWpnController(getEntity(tpl).createComponent<OBCWpnController>(getCPhys(tpl), OBGroup::GEnemy));
		getEntity(tpl).createComponent<OBCPlayer>(getCPhys(tpl), getCDraw(tpl), getCKillable(tpl), cWielder, cWpnController);
		emplaceSpriteByTile(getCDraw(tpl), assets.txSmall, assets.p1Stand);
		emplaceSpriteByTile(getCDraw(tpl), assets.txSmall, assets.p1Gun);
		return getEntity(tpl);
	}
	Entity& OBFactory::createEBall(const Vec2i& mPos, bool mFlying, bool mSmall)
	{
		auto tpl(createEnemyBase(mPos, {750, 750}, mSmall ? 2 : 5));
		if(!mSmall && !mFlying) getEntity(tpl).createComponent<OBCFloorSmasher>(getCPhys(tpl)).setActive(true);
		getEntity(tpl).createComponent<OBCEBall>(getCEnemy(tpl), mFlying, mSmall);
		emplaceSpriteByTile(getCDraw(tpl), assets.txSmall, mFlying ? assets.eBallFlying : assets.eBall);
		if(mSmall) getCDraw(tpl).setGlobalScale({0.7f, 0.7f});
		return getEntity(tpl);
	}
	Entity& OBFactory::createERunner(const Vec2i& mPos, bool mArmed)
	{
		auto tpl(createEnemyBase(mPos, {600, 600}, 4));
		auto& cDir8(getEntity(tpl).createComponent<OBCDir8>());
		auto& cWielder(getEntity(tpl).createComponent<OBCWielder>(getCPhys(tpl), getCDraw(tpl), cDir8));
		auto& cWpnController(getEntity(tpl).createComponent<OBCWpnController>(getCPhys(tpl), OBGroup::GFriendly));
		getEntity(tpl).createComponent<OBCERunner>(getCEnemy(tpl), cWielder, cWpnController, mArmed);
		emplaceSpriteByTile(getCDraw(tpl), assets.txSmall, assets.e1AStand);
		emplaceSpriteByTile(getCDraw(tpl), assets.txSmall, assets.e1AGun);
		return getEntity(tpl);
	}
	Entity& OBFactory::createECharger(const Vec2i& mPos, bool mArmed)
	{
		auto tpl(createEnemyBase(mPos, {1100, 1100}, 18));
		auto& cFloorSmasher(getEntity(tpl).createComponent<OBCFloorSmasher>(getCPhys(tpl)));
		auto& cDir8(getEntity(tpl).createComponent<OBCDir8>());
		auto& cWielder(getEntity(tpl).createComponent<OBCWielder>(getCPhys(tpl), getCDraw(tpl), cDir8));
		auto& cWpnController(getEntity(tpl).createComponent<OBCWpnController>(getCPhys(tpl), OBGroup::GFriendly));
		getEntity(tpl).createComponent<OBCECharger>(getCEnemy(tpl), cFloorSmasher, cWielder, cWpnController, mArmed);
		emplaceSpriteByTile(getCDraw(tpl), assets.txMedium, assets.e2AStand);
		emplaceSpriteByTile(getCDraw(tpl), assets.txMedium, assets.e2AGun);
		return getEntity(tpl);
	}
	Entity& OBFactory::createEJuggernaut(const Vec2i& mPos, bool mArmed)
	{
		auto tpl(createEnemyBase(mPos, {2100, 2100}, 36));
		auto& cDir8(getEntity(tpl).createComponent<OBCDir8>());
		auto& cWielder(getEntity(tpl).createComponent<OBCWielder>(getCPhys(tpl), getCDraw(tpl), cDir8));
		auto& cWpnController(getEntity(tpl).createComponent<OBCWpnController>(getCPhys(tpl), OBGroup::GFriendly));
		getEntity(tpl).createComponent<OBCEJuggernaut>(getCEnemy(tpl), cWielder, cWpnController, mArmed);
		emplaceSpriteByTile(getCDraw(tpl), assets.txBig, assets.e3AStand);
		emplaceSpriteByTile(getCDraw(tpl), assets.txBig, assets.e3AGun);
		return getEntity(tpl);
	}
	Entity& OBFactory::createEGiant(const Vec2i& mPos)
	{
		auto tpl(createEnemyBase(mPos, {2500, 2500}, 100));
		getEntity(tpl).createComponent<OBCFloorSmasher>(getCPhys(tpl), true);
		getEntity(tpl).createComponent<OBCEGiant>(getCEnemy(tpl));
		emplaceSpriteByTile(getCDraw(tpl), assets.txGiant, assets.e4UAStand);
		return getEntity(tpl);
	}
	Entity& OBFactory::createEEnforcer(const Vec2i& mPos)
	{
		auto tpl(createEnemyBase(mPos, {1200, 1200}, 30));
		getEntity(tpl).createComponent<OBCEEnforcer>(getCEnemy(tpl));
		emplaceSpriteByTile(getCDraw(tpl), assets.txMedium, assets.e5UAStand);
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
		return createETurretBase(mPos, mDir, assets.eTurret2, OBWpnTypes::createEPlasmaBulletGun(1, 5.f), 125.f, 1.f, 8);
	}


	Entity& OBFactory::createPJBullet(const Vec2i& mPos, float mDegrees)
	{
		auto tpl(createProjectileBase(mPos, {150, 150}, 420.f, mDegrees, assets.pjBullet));
		return getEntity(tpl);
	}
	Entity& OBFactory::createPJBulletPlasma(const Vec2i& mPos, float mDegrees)
	{
		auto tpl(createProjectileBase(mPos, {150, 150}, 360.f, mDegrees, assets.pjBulletPlasma));
		getEntity(tpl).createComponent<OBCParticleEmitter>(getCPhys(tpl), OBCParticleEmitter::Type::Plasma);
		getCDraw(tpl).setBlendMode(sf::BlendMode::BlendAdd);
		return getEntity(tpl);
	}
	Entity& OBFactory::createPJBoltPlasma(const Vec2i& mPos, float mDegrees)
	{
		auto tpl(createProjectileBase(mPos, {150, 150}, 260.f, mDegrees, assets.pjPlasma));
		getEntity(tpl).createComponent<OBCParticleEmitter>(getCPhys(tpl), OBCParticleEmitter::Type::Plasma);
		getCProjectile(tpl).setPierceOrganic(-1);
		getCDraw(tpl).setBlendMode(sf::BlendMode::BlendAdd);
		return getEntity(tpl);
	}
	Entity& OBFactory::createPJStar(const Vec2i& mPos, float mDegrees)
	{
		auto tpl(createProjectileBase(mPos, {150, 150}, 320.f, mDegrees, assets.pjStar));
		return getEntity(tpl);
	}
	Entity& OBFactory::createPJStarPlasma(const Vec2i& mPos, float mDegrees)
	{
		auto tpl(createProjectileBase(mPos, {150, 150}, 270.f, mDegrees, assets.pjStarPlasma));
		getEntity(tpl).createComponent<OBCParticleEmitter>(getCPhys(tpl), OBCParticleEmitter::Type::Plasma);
		getCDraw(tpl).setBlendMode(sf::BlendMode::BlendAdd);
		return getEntity(tpl);
	}
	Entity& OBFactory::createPJCannonPlasma(const Vec2i& mPos, float mDegrees)
	{
		auto tpl(createProjectileBase(mPos, {150, 150}, 120.f, mDegrees, assets.pjCannonPlasma));
		getEntity(tpl).createComponent<OBCFloorSmasher>(getCPhys(tpl)).setActive(true);
		getEntity(tpl).createComponent<OBCParticleEmitter>(getCPhys(tpl), OBCParticleEmitter::Type::Plasma, 5);
		getCProjectile(tpl).setPierceOrganic(-1);
		getCProjectile(tpl).setDamage(5);
		getCProjectile(tpl).onDestroy += [this, tpl]
		{
			for(int i{0}; i < 360; i += 360 / 10)
			{
				getCProjectile(tpl).createChild(createPJBoltPlasma(getCPhys(tpl).getPosI() + Vec2i(ssvs::getVecFromDegrees<float>(i) * 300.f), i));
			}
		};
		getCDraw(tpl).setBlendMode(sf::BlendMode::BlendAdd);
		return getEntity(tpl);
	}


	Entity& OBFactory::createPJTestBomb(const Vec2i& mPos, float mDegrees, float mSpeedMult, float mCurveMult)
	{
		auto tpl(createProjectileBase(mPos, {150, 150}, 150.f * mSpeedMult, mDegrees, assets.pjStar));
		getEntity(tpl).createComponent<OBCFloorSmasher>(getCPhys(tpl)).setActive(true);
		getCProjectile(tpl).setCurveSpeed(2.f * mCurveMult);
		getCProjectile(tpl).setPierceOrganic(-1);
		getCProjectile(tpl).setDamage(10);
		getCProjectile(tpl).onDestroy += [this, tpl]
		{
			for(int i{0}; i < 360; i += 360 / 10)
			{
				getCProjectile(tpl).createChild(createPJBoltPlasma(getCPhys(tpl).getPosI() + Vec2i(ssvs::getVecFromDegrees<float>(i) * 300.f), i));
			}
		};
		return getEntity(tpl);
	}
	Entity& OBFactory::createPJTestShell(const Vec2i& mPos, float mDegrees)
	{
		auto tpl(createProjectileBase(mPos, {150, 150}, 320.f, mDegrees, assets.pjBullet));
		getCProjectile(tpl).setLife(10.f + getRnd(-5, 15));
		getCProjectile(tpl).setPierceOrganic(3);
		getCProjectile(tpl).setSpeed(getCProjectile(tpl).getSpeed() + getRnd(-5, 25));
		return getEntity(tpl);
	}
}

