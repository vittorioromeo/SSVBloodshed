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
	Sprite OBFactory::getSpriteFromTile(const std::string& mTextureId, const IntRect& mTextureRect) const { return {assets.get<Texture>(mTextureId), mTextureRect}; }
	void OBFactory::emplaceSpriteFromTile(OBCDraw& mCDraw, sf::Texture* mTexture, const sf::IntRect& mTextureRect) const { mCDraw.emplaceSprite(*mTexture, mTextureRect); }

	template<typename... TArgs> constexpr inline Entity& getEntity(const std::tuple<Entity&, TArgs...>& mTuple) { return std::get<0>(mTuple); }
	template<typename... TArgs> constexpr inline OBCPhys& getCPhys(const std::tuple<Entity&, OBCPhys&, TArgs...>& mTuple) { return std::get<1>(mTuple); }
	template<typename T1, typename... TArgs> constexpr inline OBCDraw& getCDraw(const std::tuple<Entity&, T1, OBCDraw&, TArgs...>& mTuple) { return std::get<2>(mTuple); }
	template<typename T1, typename T2, typename... TArgs> constexpr inline OBCProjectile& getCProjectile(const std::tuple<Entity&, T1, T2, OBCProjectile&, TArgs...>& mTuple) { return std::get<3>(mTuple); }
	template<typename T1, typename T2, typename T3, typename... TArgs> constexpr inline OBCEnemy& getCEnemy(const std::tuple<Entity&, T1, T2, T3, OBCEnemy&, TArgs...>& mTuple) { return std::get<4>(mTuple); }
	template<typename T1, typename T2, typename T3, typename T4, typename... TArgs> constexpr inline OBCKillable& getCKillable(const std::tuple<Entity&, T1, T2, T3, T4, OBCKillable&, TArgs...>& mTuple) { return std::get<5>(mTuple); }
	std::tuple<Entity&, OBCPhys&, OBCDraw&> OBFactory::createActorBase(const Vec2i& mPos, const Vec2i& mSize, int mDrawPriority, bool mStatic)
	{
		auto& result(createEntity(mDrawPriority));
		auto& cPhys(result.createComponent<OBCPhys>(game, mStatic, mPos, mSize));
		auto& cDraw(result.createComponent<OBCDraw>(game, cPhys.getBody()));
		cDraw.setScaleWithBody(false);
		return std::forward_as_tuple(result, cPhys, cDraw);
	}
	std::tuple<Entity&, OBCPhys&, OBCDraw&, OBCHealth&, OBCEnemy&, OBCKillable&> OBFactory::createEnemyBase(const Vec2i& mPos, const Vec2i& mSize, int mHealth)
	{
		auto tpl(createActorBase(mPos, mSize, OBLayer::PEnemy));
		auto& cHealth(getEntity(tpl).createComponent<OBCHealth>(mHealth));
		auto& cKillable(getEntity(tpl).createComponent<OBCKillable>(getCPhys(tpl), cHealth, OBCKillable::Type::Organic));
		auto& cTargeter(getEntity(tpl).createComponent<OBCTargeter>(getCPhys(tpl), OBGroup::GFriendly));
		auto& cBoid(getEntity(tpl).createComponent<OBCBoid>(getCPhys(tpl)));
		auto& cEnemy(getEntity(tpl).createComponent<OBCEnemy>(getCPhys(tpl), getCDraw(tpl), cKillable, cTargeter, cBoid));
		return std::forward_as_tuple(getEntity(tpl), getCPhys(tpl), getCDraw(tpl), cHealth, cEnemy, cKillable);
	}
	std::tuple<Entity&, OBCPhys&, OBCDraw&, OBCProjectile&> OBFactory::createProjectileBase(const Vec2i& mPos, const Vec2i& mSize, float mSpeed, float mDegrees, const IntRect& mIntRect)
	{
		auto tpl(createActorBase(mPos, mSize, OBLayer::PProjectile));
		auto& cProjectile(getEntity(tpl).createComponent<OBCProjectile>(getCPhys(tpl), getCDraw(tpl), mSpeed, mDegrees));
		emplaceSpriteFromTile(getCDraw(tpl), assets.txSmall, mIntRect);
		return std::forward_as_tuple(getEntity(tpl), getCPhys(tpl), getCDraw(tpl), cProjectile);
	}

	Entity& OBFactory::createParticleSystem(RenderTexture& mRenderTexture, bool mClearOnDraw, unsigned char mOpacity, int mDrawPriority)
	{
		auto& result(createEntity(mDrawPriority));
		result.createComponent<OBCParticleSystem>(mRenderTexture, game.getGameWindow(), mClearOnDraw, mOpacity);
		return result;
	}
	Entity& OBFactory::createFloor(const Vec2i& mPos)
	{
		auto tpl(createActorBase(mPos, {1000, 1000}, OBLayer::PFloor));
		getEntity(tpl).createComponent<OBCFloor>(getCPhys(tpl), getCDraw(tpl));
		emplaceSpriteFromTile(getCDraw(tpl), assets.txSmall, getRnd(0, 10) < 9 ? assets.floor : (ssvu::getRnd(0, 2) < 1 ? assets.floorAlt1 : assets.floorAlt2));
		return getEntity(tpl);
	}
	Entity& OBFactory::createWall(const Vec2i& mPos)
	{
		auto tpl(createActorBase(mPos, {1000, 1000}, OBLayer::PWall, true));
		getCPhys(tpl).getBody().addGroup(OBGroup::GSolid);
		emplaceSpriteFromTile(getCDraw(tpl), assets.txSmall, assets.wallSingle);
		return getEntity(tpl);
	}
	Entity& OBFactory::createPlayer(const Vec2i& mPos)
	{
		auto tpl(createActorBase(mPos, {650, 650}, OBLayer::PPlayer));
		auto& cHealth(getEntity(tpl).createComponent<OBCHealth>(10));
		auto& cKillable(getEntity(tpl).createComponent<OBCKillable>(getCPhys(tpl), cHealth, OBCKillable::Type::Organic));
		auto& cDirection8(getEntity(tpl).createComponent<OBCDirection8>());
		auto& cWielder(getEntity(tpl).createComponent<OBCWielder>(getCPhys(tpl), getCDraw(tpl), cDirection8));
		getEntity(tpl).createComponent<OBCPlayer>(getCPhys(tpl), getCDraw(tpl), cKillable, cWielder);
		emplaceSpriteFromTile(getCDraw(tpl), assets.txSmall, assets.p1Stand);
		emplaceSpriteFromTile(getCDraw(tpl), assets.txSmall, assets.p1Gun);
		return getEntity(tpl);
	}
	Entity& OBFactory::createTestEnemyBall(const Vec2i& mPos)
	{
		auto tpl(createEnemyBase(mPos, {750, 750}, 5));
		auto& cFloorSmasher(getEntity(tpl).createComponent<OBCFloorSmasher>(getCPhys(tpl)));
		getEntity(tpl).createComponent<OBCEBall>(getCEnemy(tpl), cFloorSmasher);
		emplaceSpriteFromTile(getCDraw(tpl), assets.txSmall, assets.eBall);
		return getEntity(tpl);
	}

	Entity& OBFactory::createERunner(const Vec2i& mPos, bool mArmed)
	{
		auto tpl(createEnemyBase(mPos, {600, 600}, 4));
		auto& cDirection8(getEntity(tpl).createComponent<OBCDirection8>());
		auto& cWielder(getEntity(tpl).createComponent<OBCWielder>(getCPhys(tpl), getCDraw(tpl), cDirection8));
		getEntity(tpl).createComponent<OBCERunner>(getCEnemy(tpl), cWielder, mArmed);
		emplaceSpriteFromTile(getCDraw(tpl), assets.txSmall, assets.e2Stand);
		emplaceSpriteFromTile(getCDraw(tpl), assets.txSmall, assets.e2Gun);
		return getEntity(tpl);
	}
	Entity& OBFactory::createECharger(const Vec2i& mPos, bool mArmed)
	{
		auto tpl(createEnemyBase(mPos, {1100, 1100}, 18));
		auto& cFloorSmasher(getEntity(tpl).createComponent<OBCFloorSmasher>(getCPhys(tpl)));
		auto& cDirection8(getEntity(tpl).createComponent<OBCDirection8>());
		auto& cWielder(getEntity(tpl).createComponent<OBCWielder>(getCPhys(tpl), getCDraw(tpl), cDirection8));
		getEntity(tpl).createComponent<OBCECharger>(getCEnemy(tpl), cFloorSmasher, cWielder, mArmed);
		emplaceSpriteFromTile(getCDraw(tpl), assets.txMedium, assets.e6Stand);
		emplaceSpriteFromTile(getCDraw(tpl), assets.txMedium, assets.e6Gun);
		return getEntity(tpl);
	}
	Entity& OBFactory::createEJuggernaut(const Vec2i& mPos, bool mArmed)
	{
		auto tpl(createEnemyBase(mPos, {2100, 2100}, 36));
		auto& cDirection8(getEntity(tpl).createComponent<OBCDirection8>());
		auto& cWielder(getEntity(tpl).createComponent<OBCWielder>(getCPhys(tpl), getCDraw(tpl), cDirection8));
		getEntity(tpl).createComponent<OBCEJuggernaut>(getCEnemy(tpl), cWielder, mArmed);
		emplaceSpriteFromTile(getCDraw(tpl), assets.txBig, assets.e7Stand);
		emplaceSpriteFromTile(getCDraw(tpl), assets.txBig, assets.e7Gun);
		return getEntity(tpl);
	}
	Entity& OBFactory::createEGiant(const Vec2i& mPos)
	{
		auto tpl(createEnemyBase(mPos, {2500, 2500}, 42));
		auto& cFloorSmasher(getEntity(tpl).createComponent<OBCFloorSmasher>(getCPhys(tpl)));
		getEntity(tpl).createComponent<OBCEGiant>(getCEnemy(tpl), cFloorSmasher);
		emplaceSpriteFromTile(getCDraw(tpl), assets.txGiant, assets.e5Stand);
		return getEntity(tpl);
	}

	Entity& OBFactory::createProjectileBullet(const Vec2i& mPos, float mDegrees) { return getEntity(createProjectileBase(mPos, {150, 150}, 420.f, mDegrees, assets.pjBullet)); }
	Entity& OBFactory::createProjectilePlasma(const Vec2i& mPos, float mDegrees)
	{
		auto tpl(createProjectileBase(mPos, {150, 150}, 260.f, mDegrees, assets.pjPlasma));
		getEntity(tpl).createComponent<OBCParticleEmitter>(getCPhys(tpl), OBCParticleEmitter::Type::Plasma);
		getCProjectile(tpl).setPierceOrganic(-1);
		return getEntity(tpl);
	}
	Entity& OBFactory::createProjectileEnemyBullet(const Vec2i& mPos, float mDegrees)
	{
		auto tpl(createProjectileBase(mPos, {150, 150}, 320.f, mDegrees, assets.pjStar));
		getCProjectile(tpl).setTargetGroup(OBGroup::GFriendly);
		return getEntity(tpl);
	}

	Entity& OBFactory::createProjectileTestBomb(const Vec2i& mPos, float mDegrees, float mSpeedMult, float mCurveMult)
	{
		auto tpl(createProjectileBase(mPos, {150, 150}, 150.f * mSpeedMult, mDegrees, assets.pjStar));
		getEntity(tpl).createComponent<OBCFloorSmasher>(getCPhys(tpl)).setActive(true);
		getCProjectile(tpl).setCurveSpeed(2.f * mCurveMult);
		getCProjectile(tpl).setPierceOrganic(-1);
		getCProjectile(tpl).setDamage(10);
		getCProjectile(tpl).onDestroy += [this, tpl]
		{
			for(int i{0}; i < 360; i += 360 / 10) createProjectilePlasma(getCPhys(tpl).getPosI() + Vec2i(ssvs::getVecFromDegrees<float>(i) * 300.f), i);
		};
		return getEntity(tpl);
	}
	Entity& OBFactory::createProjectileTestShell(const Vec2i& mPos, float mDegrees)
	{
		auto tpl(createProjectileBase(mPos, {150, 150}, 320.f, mDegrees, assets.pjBullet));
		getCProjectile(tpl).setLife(10.f + getRnd(-5, 15));
		getCProjectile(tpl).setPierceOrganic(3);
		getCProjectile(tpl).setSpeed(getCProjectile(tpl).getSpeed() + getRnd(-5, 25));
		return getEntity(tpl);
	}
}

