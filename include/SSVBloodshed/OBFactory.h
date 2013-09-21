// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_FACTORY
#define SSVOB_FACTORY

#include "SSVBloodshed/OBCommon.h"

namespace ob
{
	class OBAssets;
	class OBGame;
	class ParticleSystem;

	class OBFactory
	{
		private:
			OBAssets& assets;
			OBGame& game;
			sses::Manager& manager;
			ssvsc::World& world;

			sf::Sprite getSpriteFromTile(const std::string& mTextureId, const sf::IntRect& mTextureRect) const;

		public:
			OBFactory(OBAssets& mAssets, OBGame& mGame, sses::Manager& mManager, ssvsc::World& mWorld) : assets(mAssets), game(mGame), manager(mManager), world(mWorld) { }

			Entity& createParticleSystem(sf::RenderTexture& mRenderTexture, bool mClearOnDraw = false, unsigned char mOpacity = 255);

			Entity& createFloor(const Vec2i& mPos);
			Entity& createWall(const Vec2i& mPos);
			Entity& createPlayer(const Vec2i& mPos);
			Entity& createTest(const Vec2i& mPos);
			Entity& createTestProj(const Vec2i& mPos, float mDir);
			Entity& createTestEnemy(const Vec2i& mPos);
			Entity& createTestEnemyBig(const Vec2i& mPos);
	};
}

#endif
