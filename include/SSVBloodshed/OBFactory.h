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
	class OBCDraw;
	class OBCPhys;
	class OBCHealth;
	class OBCEnemy;
	class OBCProjectile;
	class OBCKillable;
	class ParticleSystem;

	class OBFactory
	{
		private:
			OBAssets& assets;
			OBGame& game;
			sses::Manager& manager;

			sf::Sprite getSpriteFromTile(const std::string& mTextureId, const sf::IntRect& mTextureRect) const;
			void emplaceSpriteFromTile(OBCDraw& mCDraw, sf::Texture* mTexture, const sf::IntRect& mTextureRect) const;

			inline Entity& createEntity(int mDrawPriority = 0) { auto& result(manager.createEntity()); result.setDrawPriority(mDrawPriority); return result; }
			std::tuple<Entity&, OBCPhys&, OBCDraw&> createActorBase(const Vec2i& mPos, const Vec2i& mSize, int mDrawPriority = 0, bool mStatic = false);
			std::tuple<Entity&, OBCPhys&, OBCDraw&, OBCHealth&, OBCEnemy&, OBCKillable&> createEnemyBase(const Vec2i& mPos, const Vec2i& mSize, int mHealth);
			std::tuple<Entity&, OBCPhys&, OBCDraw&, OBCProjectile&> createProjectileBase(const Vec2i& mPos, const Vec2i& mSize, float mSpeed, float mDegrees, const sf::IntRect& mIntRect);

		public:
			OBFactory(OBAssets& mAssets, OBGame& mGame, sses::Manager& mManager) : assets(mAssets), game(mGame), manager(mManager) { }

			Entity& createParticleSystem(sf::RenderTexture& mRenderTexture, bool mClearOnDraw = false, unsigned char mOpacity = 255, int mDrawPriority = 1000);

			Entity& createFloor(const Vec2i& mPos);
			Entity& createWall(const Vec2i& mPos);
			Entity& createPlayer(const Vec2i& mPos);

			// Enemies
			Entity& createEBall(const Vec2i& mPos);
			Entity& createERunner(const Vec2i& mPos, bool mArmed);
			Entity& createECharger(const Vec2i& mPos, bool mArmed);
			Entity& createEJuggernaut(const Vec2i& mPos, bool mArmed);
			Entity& createEGiant(const Vec2i& mPos);

			// Projectiles
			Entity& createPJBullet(const Vec2i& mPos, float mDegrees);
			Entity& createPJPlasma(const Vec2i& mPos, float mDegrees);
			Entity& createPJEnemyStar(const Vec2i& mPos, float mDegrees);
			Entity& createPJTestBomb(const Vec2i& mPos, float mDegrees, float mSpeedMult = 1.f, float mCurveMult = 1.f);
			Entity& createPJTestShell(const Vec2i& mPos, float mDegrees);
	};
}

#endif
