// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_FACTORY
#define SSVOB_FACTORY

#include "SSVBloodshed/OBCommon.hpp"

namespace ob
{
	class OBAssets;
	class OBGame;
	class OBCActor;
	class OBCActorND;
	class OBCDraw;
	class OBCPhys;
	class OBCHealth;
	class OBCEnemy;
	class OBCProjectile;
	class OBCKillable;
	class OBParticleSystem;
	class OBWpnType;

	template<typename T, typename TTpl> inline constexpr T& gt(const TTpl& mTpl) noexcept { return std::get<T&>(mTpl); }

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
			std::tuple<Entity&, OBCPhys&, OBCDraw&, OBCHealth&, OBCKillable&> createKillableBase(const Vec2i& mPos, const Vec2i& mSize, int mDrawPriority, float mHealth);
			std::tuple<Entity&, OBCPhys&, OBCDraw&, OBCHealth&, OBCKillable&, OBCEnemy&> createEnemyBase(const Vec2i& mPos, const Vec2i& mSize, int mHealth);
			std::tuple<Entity&, OBCPhys&, OBCDraw&, OBCProjectile&> createProjectileBase(OBCActorND* mShooter, const Vec2i& mPos, const Vec2i& mSize, float mSpeed, float mDeg, const sf::IntRect& mIntRect);
			Entity& createETurretBase(const Vec2i& mPos, Dir8 mDir, const sf::IntRect& mIntRect, const OBWpnType& mWpn, float mShootDelay, float mPJDelay, int mShootCount);

		public:
			OBFactory(OBAssets& mAssets, OBGame& mGame, sses::Manager& mManager) : assets(mAssets), game(mGame), manager(mManager) { }

			Entity& createParticleSystem(sf::RenderTexture& mRenderTexture, bool mClearOnDraw = false, unsigned char mOpacity = 255, int mDrawPriority = 1000, sf::BlendMode mBlendMode = sf::BlendMode::BlendAlpha);
			Entity& createTrail(const Vec2i& mA, const Vec2i& mB, const sf::Color& mColor);

			Entity& createFloor(const Vec2i& mPos, bool mGrate = false);
			Entity& createPit(const Vec2i& mPos);
			Entity& createTrapdoor(const Vec2i& mPos, bool mPlayerOnly);
			Entity& createWall(const Vec2i& mPos, const sf::IntRect& mIntRect);
			Entity& createWallDestructible(const Vec2i& mPos, const sf::IntRect& mIntRect);
			Entity& createDoor(const Vec2i& mPos, const sf::IntRect& mIntRect, int mId, bool mOpen);
			Entity& createDoorG(const Vec2i& mPos, const sf::IntRect& mIntRect, bool mOpen);
			Entity& createDoorR(const Vec2i& mPos, const sf::IntRect& mIntRect, bool mOpen);
			Entity& createPPlate(const Vec2i& mPos, int mId, PPlateType mType, IdAction mIdAction, bool mPlayerOnly);
			Entity& createPlayer(const Vec2i& mPos);
			Entity& createExplosiveCrate(const Vec2i& mPos, int mId);
			Entity& createShard(const Vec2i& mPos);
			Entity& createSpawner(const Vec2i& mPos, SpawnerItem mType, int mId, float mDelayStart = 0.f, float mDelaySpawn = 200.f, int mSpawnCount = 1);
			Entity& createForceField(const Vec2i& mPos, int mId, Dir8 mDir, bool mBlockFriendly, bool mBlockEnemy, float mForceMult);
			Entity& createBulletForceField(const Vec2i& mPos, int mId, Dir8 mDir, bool mBlockFriendly, bool mBlockEnemy);
			Entity& createBooster(const Vec2i& mPos, int mId, Dir8 mDir, float mForceMult);

			// Enemies
			Entity& createERunner(const Vec2i& mPos, RunnerType mType);
			Entity& createECharger(const Vec2i& mPos, ChargerType mType);
			Entity& createEJuggernaut(const Vec2i& mPos, JuggernautType mType);
			Entity& createEGiant(const Vec2i& mPos);
			Entity& createEEnforcer(const Vec2i& mPos);
			Entity& createEBall(const Vec2i& mPos, BallType mType, bool mSmall);
			Entity& createETurretStarPlasma(const Vec2i& mPos, Dir8 mDir);
			Entity& createETurretCannonPlasma(const Vec2i& mPos, Dir8 mDir);
			Entity& createETurretBulletPlasma(const Vec2i& mPos, Dir8 mDir);
			Entity& createETurretRocket(const Vec2i& mPos, Dir8 mDir);

			// Projectiles
			Entity& createPJBullet(OBCActorND* mShooter, const Vec2i& mPos, float mDeg);
			Entity& createPJBulletPlasma(OBCActorND* mShooter, const Vec2i& mPos, float mDeg);
			Entity& createPJBoltPlasma(OBCActorND* mShooter, const Vec2i& mPos, float mDeg);
			Entity& createPJStar(OBCActorND* mShooter, const Vec2i& mPos, float mDeg);
			Entity& createPJStarPlasma(OBCActorND* mShooter, const Vec2i& mPos, float mDeg);
			Entity& createPJCannonPlasma(OBCActorND* mShooter, const Vec2i& mPos, float mDeg);
			Entity& createPJRocket(OBCActorND* mShooter, const Vec2i& mPos, float mDeg);
			Entity& createPJGrenade(OBCActorND* mShooter, const Vec2i& mPos, float mDeg);
			Entity& createPJExplosion(OBCActorND* mShooter, const Vec2i& mPos, float mDeg, float mSpeed = 300.f);
			Entity& createPJShockwave(OBCActorND* mShooter, const Vec2i& mPos, float mDeg, int mNum);

			// Vending machines
			Entity& createVMHealth(const Vec2i& mPos);

			Entity& createPJTestBomb(OBCActorND* mShooter, const Vec2i& mPos, float mDeg, float mSpeedMult = 1.f, float mCurveMult = 1.f);
			Entity& createPJTestShell(OBCActorND* mShooter, const Vec2i& mPos, float mDeg);

			template<typename T> inline void deathExplode(OBCActorND* mShooter, T& mTpl, std::size_t mCount, float mRangeMult = 1.f)
			{
				auto& cp(createPJExplosion(mShooter, gt<OBCPhys>(mTpl).getPosI(), 0, 0).template getComponent<OBCProjectile>());
				cp.setTargetGroup(OBGroup::GKillable); cp.setLife(16.f * mRangeMult); cp.setKillDestructible(true);

				SSVU_ASSERT(mCount != 0);
				for(int i{0}; i < 360; i += 360 / mCount)
				{
					auto& cpMv(createPJExplosion(mShooter, gt<OBCPhys>(mTpl).getPosI() + Vec2i(ssvs::getVecFromDeg(i, 251.f)), i).template getComponent<OBCProjectile>());
					cpMv.setTargetGroup(OBGroup::GKillable); cpMv.setLife(16.f * mRangeMult); cpMv.setKillDestructible(true);
				}
			}
	};
}

#endif
