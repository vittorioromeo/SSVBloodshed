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
	class OBParticleSystem;
	class OBWpnType;

	class OBFactory
	{
		private:
			OBAssets& assets;
			OBGame& game;
			sses::Manager& manager;

			sf::Sprite getSpriteByTile(const std::string& mTextureId, const sf::IntRect& mRect) const;
			void emplaceSpriteByTile(OBCDraw& mCDraw, sf::Texture* mTexture, const sf::IntRect& mRect) const;

			inline Entity& createEntity(int mDrawPriority = 0) { auto& result(manager.createEntity()); result.setDrawPriority(mDrawPriority); return result; }
			std::tuple<Entity&, OBCPhys&, OBCDraw&> createActorBase(const Vec2i& mPos, const Vec2i& mSize, int mDrawPriority = 0, bool mStatic = false);
			std::tuple<Entity&, OBCPhys&, OBCDraw&, OBCHealth&, OBCKillable&> createKillableBase(const Vec2i& mPos, const Vec2i& mSize, int mDrawPriority, int mHealth);
			std::tuple<Entity&, OBCPhys&, OBCDraw&, OBCHealth&, OBCKillable&, OBCEnemy&> createEnemyBase(const Vec2i& mPos, const Vec2i& mSize, int mHealth);
			std::tuple<Entity&, OBCPhys&, OBCDraw&, OBCProjectile&> createProjectileBase(const Vec2i& mPos, const Vec2i& mSize, float mSpeed, float mDegrees, const sf::IntRect& mIntRect);
			Entity& createETurretBase(const Vec2i& mPos, Dir8 mDir, const sf::IntRect& mIntRect, const OBWpnType& mWpn, float mShootDelay, float mPJDelay, int mShootCount);

		public:
			OBFactory(OBAssets& mAssets, OBGame& mGame, sses::Manager& mManager) : assets(mAssets), game(mGame), manager(mManager) { }

			Entity& createParticleSystem(sf::RenderTexture& mRenderTexture, bool mClearOnDraw = false, unsigned char mOpacity = 255, int mDrawPriority = 1000, sf::BlendMode mBlendMode = sf::BlendMode::BlendAlpha);

			Entity& createFloor(const Vec2i& mPos, bool mGrate = false);
			Entity& createPit(const Vec2i& mPos);
			Entity& createTrapdoor(const Vec2i& mPos);
			Entity& createWall(const Vec2i& mPos, const sf::IntRect& mIntRect);
			Entity& createWallDestructible(const Vec2i& mPos, const sf::IntRect& mIntRect);
			Entity& createDoor(const Vec2i& mPos, const sf::IntRect& mIntRect, int mId, bool mOpen);
			Entity& createDoorG(const Vec2i& mPos, const sf::IntRect& mIntRect, bool mOpen);
			Entity& createDoorR(const Vec2i& mPos, const sf::IntRect& mIntRect, bool mOpen);
			Entity& createPPlate(const Vec2i& mPos, int mId, PPlateType mType, OBIdAction mIdAction);
			Entity& createPlayer(const Vec2i& mPos);
			Entity& createShard(const Vec2i& mPos);

			// Enemies
			Entity& createEBall(const Vec2i& mPos, bool mFlying, bool mSmall = false);
			Entity& createERunner(const Vec2i& mPos, bool mArmed = false);
			Entity& createECharger(const Vec2i& mPos, bool mArmed = false);
			Entity& createEJuggernaut(const Vec2i& mPos, bool mArmed = false);
			Entity& createEGiant(const Vec2i& mPos);
			Entity& createEEnforcer(const Vec2i& mPos);
			Entity& createETurretStarPlasma(const Vec2i& mPos, Dir8 mDir);
			Entity& createETurretCannonPlasma(const Vec2i& mPos, Dir8 mDir);
			Entity& createETurretBulletPlasma(const Vec2i& mPos, Dir8 mDir);

			// Projectiles
			Entity& createPJBullet(const Vec2i& mPos, float mDegrees);
			Entity& createPJBulletPlasma(const Vec2i& mPos, float mDegrees);
			Entity& createPJBoltPlasma(const Vec2i& mPos, float mDegrees);
			Entity& createPJStar(const Vec2i& mPos, float mDegrees);
			Entity& createPJStarPlasma(const Vec2i& mPos, float mDegrees);
			Entity& createPJCannonPlasma(const Vec2i& mPos, float mDegrees);
			Entity& createPJTestBomb(const Vec2i& mPos, float mDegrees, float mSpeedMult = 1.f, float mCurveMult = 1.f);
			Entity& createPJTestShell(const Vec2i& mPos, float mDegrees);
	};
}

#endif
