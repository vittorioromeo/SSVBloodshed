// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include "SSVBloodshed/OBFactory.h"
#include "SSVBloodshed/Components/OBCPhysics.h"
#include "SSVBloodshed/Components/OBCRender.h"
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
		auto& cPhysics(result.createComponent<OBCPhysics>(world, true, mPos, Vec2i{1000, 1000}));
		auto& cRender(result.createComponent<OBCRender>(game, cPhysics.getBody()));

		cRender.addSprite(getSpriteFromTile("tileset.png", assets.tileset[{0, 0}]));
		cRender.setScaleWithBody(false);

		return result;
	}
	Entity& OBFactory::createWall(const Vec2i& mPos)
	{
		auto& result(manager.createEntity());
		auto& cPhysics(result.createComponent<OBCPhysics>(world, true, mPos, Vec2i{1000, 1000}));
		auto& cRender(result.createComponent<OBCRender>(game, cPhysics.getBody()));

		Body& body(cPhysics.getBody());
		body.addGroup(OBGroup::Solid);
		body.addGroupToCheck(OBGroup::Solid);

		cRender.addSprite(getSpriteFromTile("tileset.png", assets.tileset[{1, 0}]));
		cRender.setScaleWithBody(false);

		return result;
	}
	Entity& OBFactory::createPlayer(const Vec2i& mPos)
	{
		auto& result(manager.createEntity());
		result.setDrawPriority(-1000);
		result.addGroup(OBGroup::Player);
		auto& cPhysics(result.createComponent<OBCPhysics>(world, false, mPos, Vec2i{700, 700}));
		auto& cRender(result.createComponent<OBCRender>(game, cPhysics.getBody()));
		result.createComponent<OBCPlayer>(game, cPhysics, cRender, assets);

		Body& body(cPhysics.getBody());
		body.addGroup(OBGroup::Solid);
		body.addGroup(OBGroup::Player);
		body.addGroup(OBGroup::Organic);
		body.addGroupToCheck(OBGroup::Solid);

		cRender.addSprite(getSpriteFromTile("tilesetPlayer.png", assets.tilesetPlayer[{0, 0}]));
		cRender.addSprite(getSpriteFromTile("tilesetPlayer.png", assets.tilesetPlayer[{4, 0}]));
		cRender.setScaleWithBody(false);

		return result;
	}
	Entity& OBFactory::createTest(const Vec2i& mPos)
	{
		auto& result(manager.createEntity());
		auto& cPhysics(result.createComponent<OBCPhysics>(world, false, mPos, Vec2i{700, 700}));
		auto& cRender(result.createComponent<OBCRender>(game, cPhysics.getBody()));

		Body& body(cPhysics.getBody());
		body.addGroup(OBGroup::Solid);
		body.addGroupToCheck(OBGroup::Solid);

		cRender.addSprite(getSpriteFromTile("tileset.png", assets.tileset[{0, 2}]));
		cRender.setScaleWithBody(false);

		return result;
	}
	Entity& OBFactory::createTestProj(const Vec2i& mPos, float mDir)
	{
		auto& result(manager.createEntity());
		auto& cPhysics(result.createComponent<OBCPhysics>(world, false, mPos, Vec2i{150, 150}));
		auto& cRender(result.createComponent<OBCRender>(game, cPhysics.getBody()));
		result.createComponent<OBCProjectile>(game, cPhysics, cRender, 420.f, mDir);

		cRender.addSprite(getSpriteFromTile("tilesetProjectiles.png", assets.tileset[{0, 0}]));
		cRender.setScaleWithBody(false);

		return result;
	}
	Entity& OBFactory::createTestEnemy(const Vec2i& mPos)
	{
		auto& result(manager.createEntity());
		result.setDrawPriority(-500);
		auto& cPhysics(result.createComponent<OBCPhysics>(world, false, mPos, Vec2i{600, 600}));
		auto& cRender(result.createComponent<OBCRender>(game, cPhysics.getBody()));
		result.createComponent<OBCEnemy>(game, cPhysics, cRender, assets);

		Body& body(cPhysics.getBody());
		body.addGroup(OBGroup::Solid);
		body.addGroup(OBGroup::Organic);
		body.addGroupToCheck(OBGroup::Solid);

		cRender.addSprite(getSpriteFromTile("tilesetEnemy.png", assets.tilesetEnemy[{0, 0}]));
		cRender.setScaleWithBody(false);

		return result;
	}
	Entity& OBFactory::createTestEnemyBig(const Vec2i& mPos)
	{
		auto& result(manager.createEntity());
		result.setDrawPriority(-500);
		auto& cPhysics(result.createComponent<OBCPhysics>(world, false, mPos, Vec2i{1100, 1100}));
		auto& cRender(result.createComponent<OBCRender>(game, cPhysics.getBody()));
		auto& cEnemy(result.createComponent<OBCEnemy>(game, cPhysics, cRender, assets));

		cEnemy.walkSpeed = 20.f;
		cEnemy.turnSpeed = 3.f;
		cEnemy.health = 20;
		cEnemy.gibMult = 10;

		Body& body(cPhysics.getBody());
		body.addGroup(OBGroup::Solid);
		body.addGroup(OBGroup::Organic);
		body.addGroupToCheck(OBGroup::Solid);

		cRender.addSprite(getSpriteFromTile("tilesetEnemyBig.png", assets.tilesetEnemyBig[{0, 0}]));
		cRender.setScaleWithBody(false);

		return result;
	}
}

