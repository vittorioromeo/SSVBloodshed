// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include "SSVBloodshed/OBFactory.h"
#include "SSVBloodshed/Components/OBCPhys.h"
#include "SSVBloodshed/Components/OBCDraw.h"
#include "SSVBloodshed/Components/OBCPlayer.h"
#include "SSVBloodshed/Components/OBCEnemy.h"
#include "SSVBloodshed/Components/OBCProjectile.h"
#include "SSVBloodshed/Components/OBCParticleSystem.h"

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
	void OBFactory::emplaceSpriteFromTile(OBCDraw& mCRender, const std::string& mTextureId, const sf::IntRect& mTextureRect) const { mCRender.emplaceSprite(assets.get<Texture>(mTextureId), mTextureRect); }

	Entity& OBFactory::createParticleSystem(RenderTexture& mRenderTexture, bool mClearOnDraw, unsigned char mOpacity)
	{
		auto& result(manager.createEntity());
		result.setDrawPriority(1000);
		result.createComponent<OBCParticleSystem>(mRenderTexture, game.getGameWindow().getRenderWindow(), mClearOnDraw, mOpacity);
		return result;
	}

	Entity& OBFactory::createFloor(const Vec2i& mPos)
	{
		auto& result(manager.createEntity());
		result.setDrawPriority(10000);
		auto& cPhys(result.createComponent<OBCPhys>(world, true, mPos, Vec2i{1000, 1000}));
		auto& cDraw(result.createComponent<OBCDraw>(game, cPhys.getBody()));

		emplaceSpriteFromTile(cDraw, "tileset.png", assets.tileset[{0, 0}]);
		cDraw.setScaleWithBody(false);

		return result;
	}
	Entity& OBFactory::createWall(const Vec2i& mPos)
	{
		auto& result(manager.createEntity());
		auto& cPhys(result.createComponent<OBCPhys>(world, true, mPos, Vec2i{1000, 1000}));
		auto& cDraw(result.createComponent<OBCDraw>(game, cPhys.getBody()));

		Body& body(cPhys.getBody());
		body.addGroup(OBGroup::Solid);
		body.addGroupToCheck(OBGroup::Solid);

		emplaceSpriteFromTile(cDraw, "tileset.png", assets.tileset[{1, 0}]);
		cDraw.setScaleWithBody(false);

		return result;
	}
	Entity& OBFactory::createPlayer(const Vec2i& mPos)
	{
		auto& result(manager.createEntity());
		result.setDrawPriority(-1000);
		result.addGroup(OBGroup::Player);
		auto& cPhys(result.createComponent<OBCPhys>(world, false, mPos, Vec2i{700, 700}));
		auto& cDraw(result.createComponent<OBCDraw>(game, cPhys.getBody()));
		result.createComponent<OBCPlayer>(game, cPhys, cDraw, assets);

		Body& body(cPhys.getBody());
		body.addGroup(OBGroup::Solid);
		body.addGroup(OBGroup::Player);
		body.addGroup(OBGroup::Organic);
		body.addGroupToCheck(OBGroup::Solid);

		emplaceSpriteFromTile(cDraw, "tilesetPlayer.png", assets.tilesetPlayer[{0, 0}]);
		emplaceSpriteFromTile(cDraw, "tilesetPlayer.png", assets.tilesetPlayer[{4, 0}]);
		cDraw.setScaleWithBody(false);

		return result;
	}
	Entity& OBFactory::createTest(const Vec2i& mPos)
	{
		auto& result(manager.createEntity());
		auto& cPhys(result.createComponent<OBCPhys>(world, false, mPos, Vec2i{700, 700}));
		auto& cDraw(result.createComponent<OBCDraw>(game, cPhys.getBody()));

		Body& body(cPhys.getBody());
		body.addGroup(OBGroup::Solid);
		body.addGroupToCheck(OBGroup::Solid);

		emplaceSpriteFromTile(cDraw, "tileset.png", assets.tileset[{0, 2}]);
		cDraw.setScaleWithBody(false);

		return result;
	}

	Entity& OBFactory::createTestEnemy(const Vec2i& mPos)
	{
		auto& result(manager.createEntity());
		result.setDrawPriority(-500);
		auto& cPhys(result.createComponent<OBCPhys>(world, false, mPos, Vec2i{600, 600}));
		auto& cDraw(result.createComponent<OBCDraw>(game, cPhys.getBody()));
		result.createComponent<OBCEnemy>(game, cPhys, cDraw, assets);

		Body& body(cPhys.getBody());
		body.addGroup(OBGroup::Solid);
		body.addGroup(OBGroup::Organic);
		body.addGroupToCheck(OBGroup::Solid);

		emplaceSpriteFromTile(cDraw, "tilesetEnemy.png", assets.tilesetEnemy[{0, 0}]);
		cDraw.setScaleWithBody(false);

		return result;
	}
	Entity& OBFactory::createTestEnemyBig(const Vec2i& mPos)
	{
		auto& result(manager.createEntity());
		result.setDrawPriority(-500);
		auto& cPhys(result.createComponent<OBCPhys>(world, false, mPos, Vec2i{1100, 1100}));
		auto& cDraw(result.createComponent<OBCDraw>(game, cPhys.getBody()));
		auto& cEnemy(result.createComponent<OBCEnemy>(game, cPhys, cDraw, assets));

		cEnemy.walkSpeed = 20.f;
		cEnemy.turnSpeed = 3.f;
		cEnemy.health = 20;
		cEnemy.gibMult = 2;

		Body& body(cPhys.getBody());
		body.addGroup(OBGroup::Solid);
		body.addGroup(OBGroup::Organic);
		body.addGroupToCheck(OBGroup::Solid);

		emplaceSpriteFromTile(cDraw, "tilesetEnemyBig.png", assets.tilesetEnemyBig[{0, 0}]);
		cDraw.setScaleWithBody(false);

		return result;
	}
	Entity& OBFactory::createTestJuggernaut(const Vec2i& mPos)
	{
		auto& result(manager.createEntity());
		result.setDrawPriority(-500);
		auto& cPhys(result.createComponent<OBCPhys>(world, false, mPos, Vec2i{2000, 2000}));
		auto& cDraw(result.createComponent<OBCDraw>(game, cPhys.getBody()));
		auto& cEnemy(result.createComponent<OBCEnemy>(game, cPhys, cDraw, assets));

		cEnemy.walkSpeed = 10.f;
		cEnemy.turnSpeed = 2.5f;
		cEnemy.health = 40;
		cEnemy.gibMult = 4;

		Body& body(cPhys.getBody());
		body.addGroup(OBGroup::Solid);
		body.addGroup(OBGroup::Organic);
		body.addGroupToCheck(OBGroup::Solid);

		emplaceSpriteFromTile(cDraw, "tilesetJuggernaut.png", assets.tilesetJuggernaut[{0, 0}]);
		cDraw.setScaleWithBody(false);

		return result;
	}

	Entity& OBFactory::createProjectileBullet(const Vec2i& mPos, float mDegrees)
	{
		auto& result(manager.createEntity());
		auto& cPhys(result.createComponent<OBCPhys>(world, false, mPos, Vec2i{150, 150}));
		auto& cDraw(result.createComponent<OBCDraw>(game, cPhys.getBody()));
		result.createComponent<OBCProjectile>(game, cPhys, cDraw, 420.f, mDegrees);

		emplaceSpriteFromTile(cDraw, "tilesetProjectiles.png", assets.tileset[{0, 0}]);
		cDraw.setScaleWithBody(false);

		return result;
	}
	Entity& OBFactory::createProjectilePlasma(const Vec2i& mPos, float mDegrees)
	{
		auto& result(manager.createEntity());
		auto& cPhys(result.createComponent<OBCPhys>(world, false, mPos, Vec2i{150, 150}));
		auto& cDraw(result.createComponent<OBCDraw>(game, cPhys.getBody()));
		result.createComponent<OBCProjectile>(game, cPhys, cDraw, 260.f, mDegrees);

		emplaceSpriteFromTile(cDraw, "tilesetProjectiles.png", assets.tileset[{1, 0}]);
		cDraw.setScaleWithBody(false);

		return result;
	}
	Entity& OBFactory::createProjectileEnemyBullet(const Vec2i& mPos, float mDegrees)
	{
		auto& result(manager.createEntity());
		auto& cPhys(result.createComponent<OBCPhys>(world, false, mPos, Vec2i{150, 150}));
		auto& cDraw(result.createComponent<OBCDraw>(game, cPhys.getBody()));
		result.createComponent<OBCProjectile>(game, cPhys, cDraw, 350.f, mDegrees);

		emplaceSpriteFromTile(cDraw, "tilesetProjectiles.png", assets.tileset[{2, 0}]);
		cDraw.setScaleWithBody(false);

		return result;
	}
}

