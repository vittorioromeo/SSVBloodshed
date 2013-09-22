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
	void OBFactory::emplaceSpriteFromTile(OBCDraw& mCDraw, const std::string& mTextureId, const sf::IntRect& mTextureRect) const { mCDraw.emplaceSprite(assets.get<Texture>(mTextureId), mTextureRect); }

	template<typename... TArgs> constexpr inline Entity& getEntity(const std::tuple<Entity&, TArgs...>& mTuple) { return std::get<0>(mTuple); }
	template<typename... TArgs> constexpr inline OBCPhys& getCPhys(const std::tuple<Entity&, OBCPhys&, TArgs...>& mTuple) { return std::get<1>(mTuple); }
	template<typename T1, typename... TArgs> constexpr inline OBCDraw& getCDraw(const std::tuple<Entity&, T1, OBCDraw&, TArgs...>& mTuple) { return std::get<2>(mTuple); }
	template<typename T1, typename T2, typename... TArgs> constexpr inline OBCProjectile& getCProjectile(const std::tuple<Entity&, T1, T2, OBCProjectile&, TArgs...>& mTuple) { return std::get<3>(mTuple); }
	template<typename T1, typename T2, typename T3, typename... TArgs> constexpr inline OBCEnemy& getCEnemy(const std::tuple<Entity&, T1, T2, T3, OBCEnemy&, TArgs...>& mTuple) { return std::get<4>(mTuple); }
	std::tuple<Entity&, OBCPhys&, OBCDraw&> OBFactory::createActorBase(const Vec2i& mPos, const Vec2i& mSize, int mDrawPriority, bool mStatic)
	{
		auto& result(manager.createEntity()); result.setDrawPriority(mDrawPriority);
		auto& cPhys(result.createComponent<OBCPhys>(world, mStatic, mPos, mSize));
		auto& cDraw(result.createComponent<OBCDraw>(game, cPhys.getBody()));
		cDraw.setScaleWithBody(false);
		return std::forward_as_tuple(result, cPhys, cDraw);
	}
	std::tuple<Entity&, OBCPhys&, OBCDraw&, OBCHealth&, OBCEnemy&> OBFactory::createEnemyBase(const Vec2i& mPos, const Vec2i& mSize, int mHealth)
	{
		auto tpl(createActorBase(mPos, mSize, OBDrawPriority::OBDPEnemy));
		auto& cHealth(getEntity(tpl).createComponent<OBCHealth>(mHealth));
		auto& cEnemy(getEntity(tpl).createComponent<OBCEnemy>(getCPhys(tpl), getCDraw(tpl), cHealth));
		return std::forward_as_tuple(getEntity(tpl), getCPhys(tpl), getCDraw(tpl), cHealth, cEnemy);
	}
	std::tuple<Entity&, OBCPhys&, OBCDraw&, OBCProjectile&> OBFactory::createProjectileBase(const Vec2i& mPos, const Vec2i& mSize, float mSpeed, float mDegrees, const Vec2u& mTileIdx)
	{
		auto tpl(createActorBase(mPos, mSize, OBDrawPriority::OBDPProjectile));
		auto& cProjectile(getEntity(tpl).createComponent<OBCProjectile>(getCPhys(tpl), getCDraw(tpl), mSpeed, mDegrees));
		emplaceSpriteFromTile(getCDraw(tpl), "tilesetProjectiles.png", assets.tilesetProjectiles[mTileIdx]);
		return std::forward_as_tuple(getEntity(tpl), getCPhys(tpl), getCDraw(tpl), cProjectile);
	}

	Entity& OBFactory::createParticleSystem(RenderTexture& mRenderTexture, bool mClearOnDraw, unsigned char mOpacity, int mDrawPriority)
	{
		auto& result(manager.createEntity()); result.setDrawPriority(mDrawPriority);
		result.createComponent<OBCParticleSystem>(mRenderTexture, game.getGameWindow(), mClearOnDraw, mOpacity);
		return result;
	}
	Entity& OBFactory::createFloor(const Vec2i& mPos)
	{
		auto tpl(createActorBase(mPos, {1000, 1000}, OBDrawPriority::OBDPFloor));
		getEntity(tpl).createComponent<OBCFloor>(getCPhys(tpl), getCDraw(tpl));
		emplaceSpriteFromTile(getCDraw(tpl), "tileset.png", assets.tileset[{0, 0}]);
		return getEntity(tpl);
	}
	Entity& OBFactory::createWall(const Vec2i& mPos)
	{
		auto tpl(createActorBase(mPos, {1000, 1000}, OBDrawPriority::OBDPWall, true));
		getCPhys(tpl).getBody().addGroup(OBGroup::OBGSolid);
		emplaceSpriteFromTile(getCDraw(tpl), "tileset.png", assets.tileset[{1, 0}]);
		return getEntity(tpl);
	}
	Entity& OBFactory::createPlayer(const Vec2i& mPos)
	{
		auto tpl(createActorBase(mPos, {650, 650}, OBDrawPriority::OBDPPlayer));
		auto& cHealth(getEntity(tpl).createComponent<OBCHealth>(10));
		getEntity(tpl).createComponent<OBCPlayer>(getCPhys(tpl), getCDraw(tpl), cHealth);
		emplaceSpriteFromTile(getCDraw(tpl), "tsCharSmall.png", assets.tsCharSmall[{0, 0}]);
		emplaceSpriteFromTile(getCDraw(tpl), "tsCharSmall.png", assets.tsCharSmall[{2, 0}]);
		return getEntity(tpl);
	}
	Entity& OBFactory::createTestEnemy(const Vec2i& mPos)
	{
		auto tpl(createEnemyBase(mPos, {600, 600}, 2));
		emplaceSpriteFromTile(getCDraw(tpl), "tsCharSmall.png", assets.tsCharSmall[{0, 4}]);
		return getEntity(tpl);
	}
	Entity& OBFactory::createTestEnemyBig(const Vec2i& mPos)
	{
		auto tpl(createEnemyBase(mPos, {1100, 1100}, 18));
		getEntity(tpl).createComponent<OBCECharger>(getCEnemy(tpl));
		getCEnemy(tpl).setWalkSpeed(20.f);
		getCEnemy(tpl).setTurnSpeed(3.f);
		getCEnemy(tpl).setGibMult(2);
		emplaceSpriteFromTile(getCDraw(tpl), "tilesetEnemyBig.png", assets.tilesetEnemyBig[{0, 0}]);
		return getEntity(tpl);
	}
	Entity& OBFactory::createTestJuggernaut(const Vec2i& mPos)
	{
		auto tpl(createEnemyBase(mPos, {2000, 2000}, 36));
		getEntity(tpl).createComponent<OBCEJuggernaut>(getCEnemy(tpl));
		getCEnemy(tpl).setWalkSpeed(10.f);
		getCEnemy(tpl).setTurnSpeed(2.5f);
		getCEnemy(tpl).setGibMult(4);
		emplaceSpriteFromTile(getCDraw(tpl), "tilesetJuggernaut.png", assets.tilesetJuggernaut[{0, 0}]);
		return getEntity(tpl);
	}
	Entity& OBFactory::createTestGiant(const Vec2i& mPos)
	{
		auto tpl(createEnemyBase(mPos, {2500, 2500}, 42));
		getEntity(tpl).createComponent<OBCEGiant>(getCEnemy(tpl));
		getCEnemy(tpl).setWalkSpeed(10.f);
		getCEnemy(tpl).setTurnSpeed(2.5f);
		getCEnemy(tpl).setGibMult(8);
		emplaceSpriteFromTile(getCDraw(tpl), "tilesetGiant.png", assets.tilesetGiant[{0, 0}]);
		return getEntity(tpl);
	}


	Entity& OBFactory::createProjectileBullet(const Vec2i& mPos, float mDegrees) { return getEntity(createProjectileBase(mPos, {150, 150}, 420.f, mDegrees, {0, 0})); }
	Entity& OBFactory::createProjectilePlasma(const Vec2i& mPos, float mDegrees)
	{
		auto tpl(createProjectileBase(mPos, {150, 150}, 260.f, mDegrees, {1, 0}));
		getEntity(tpl).createComponent<OBCParticleEmitter>(game, getCPhys(tpl), OBCParticleEmitter::Type::Plasma);
		getCProjectile(tpl).setPierceOrganic(-1);
		return getEntity(tpl);
	}
	Entity& OBFactory::createProjectileEnemyBullet(const Vec2i& mPos, float mDegrees)
	{
		auto tpl(createProjectileBase(mPos, {150, 150}, 320.f, mDegrees, {2, 0}));
		getCProjectile(tpl).setTargetGroup(OBGroup::OBGFriendly);
		return getEntity(tpl);
	}

	Entity& OBFactory::createProjectileTestBomb(const Vec2i& mPos, float mDegrees, float mSpeedMult, float mCurveMult)
	{
		auto tpl(createProjectileBase(mPos, {150, 150}, 150.f * mSpeedMult, mDegrees, {2, 0}));
		getCProjectile(tpl).setCurveSpeed(2.f * mCurveMult);
		getCProjectile(tpl).setPierceOrganic(-1);
		getCProjectile(tpl).setDestroyFloor(true);
		getCProjectile(tpl).setDamage(10);
		getCProjectile(tpl).onDestroy += [this, tpl]
		{
			for(int i{0}; i < 360; i += 360 / 10) createProjectilePlasma(getCPhys(tpl).getPos() + Vec2i(ssvs::getVecFromDegrees<float>(i) * 300.f), i);
		};
		return getEntity(tpl);
	}
}

