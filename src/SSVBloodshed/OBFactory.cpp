// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include "SSVBloodshed/OBFactory.h"

#include "SSVBloodshed/Components/OBCPhysics.h"
#include "SSVBloodshed/Components/OBCRender.h"
#include "SSVBloodshed/Components/OBCPlayer.h"
#include "SSVBloodshed/Components/OBCEnemy.h"
#include "SSVBloodshed/Components/OBCProjectile.h"

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
	OBFactory::OBFactory(OBAssets& mAssets, OBGame& mGame, Manager& mManager, World& mWorld) : assets(mAssets), game(mGame), manager(mManager), world(mWorld) { }

	Entity& OBFactory::createFloor(const Vec2i& mPos)
	{
		auto& result(manager.createEntity());
		result.setDrawPriority(10000);
		auto& cPhysics(result.createComponent<OBCPhysics>(world, true, mPos, Vec2i{1000, 1000}));
		auto& cRender(result.createComponent<OBCRender>(game, cPhysics.getBody()));

		Sprite s{assets.get<Texture>("tileset.png")};
		s.setTextureRect(assets.tileset[{0, 0}]);

		cRender.addSprite(s);
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

		Sprite s{assets.get<Texture>("tileset.png")};
		s.setTextureRect(assets.tileset[{1, 0}]);

		cRender.addSprite(s);
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

		Sprite s{assets.get<Texture>("tilesetPlayer.png")};
		s.setTextureRect(assets.tilesetPlayer[{0, 0}]);

		Sprite s2{assets.get<Texture>("tilesetPlayer.png")};
		s2.setTextureRect(assets.tilesetPlayer[{4, 0}]);

		cRender.addSprite(s);
		cRender.addSprite(s2);
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

		Sprite s{assets.get<Texture>("tileset.png")};
		s.setTextureRect(assets.tileset[{0, 2}]);

		cRender.addSprite(s);
		cRender.setScaleWithBody(false);

		return result;
	}
	Entity& OBFactory::createTestProj(const Vec2i& mPos, float mDir)
	{
		auto& result(manager.createEntity());
		auto& cPhysics(result.createComponent<OBCPhysics>(world, false, mPos, Vec2i{150, 150}));
		auto& cRender(result.createComponent<OBCRender>(game, cPhysics.getBody()));
		result.createComponent<OBCProjectile>(game, cPhysics, cRender, 420.f, mDir);

		Sprite s{assets.get<Texture>("tilesetProjectiles.png")};
		s.setTextureRect(assets.tilesetProjectiles[{0, 0}]);
		s.setRotation(mDir);

		cRender.addSprite(s);
		cRender.setScaleWithBody(false);

		return result;
	}
	Entity& OBFactory::createTestEnemy(const Vec2i& mPos)
	{
		auto& result(manager.createEntity());
		result.setDrawPriority(-500);
		auto& cPhysics(result.createComponent<OBCPhysics>(world, false, mPos, Vec2i{700, 700}));
		auto& cRender(result.createComponent<OBCRender>(game, cPhysics.getBody()));
		result.createComponent<OBCEnemy>(game, cPhysics, cRender, assets);

		Body& body(cPhysics.getBody());
		body.addGroup(OBGroup::Solid);
		body.addGroup(OBGroup::Organic);
		body.addGroupToCheck(OBGroup::Solid);

		Sprite s{assets.get<Texture>("tilesetEnemy.png")};
		s.setTextureRect(assets.tilesetEnemy[{0, 0}]);

		cRender.addSprite(s);
		cRender.setScaleWithBody(false);

		return result;
	}
}

