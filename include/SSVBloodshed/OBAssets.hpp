// Copyright (c) 2013-2015 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_ASSETS
#define SSVOB_ASSETS

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBConfig.hpp"
#include "SSVBloodshed/Particles/OBParticleData.hpp"

namespace ob
{
	class OBAssets
	{
		private:
			ssvs::AssetManager<> assetManager;

		public:
			ssvs::SoundPlayer soundPlayer;
			ssvs::MusicPlayer musicPlayer;

			// BitmapFonts
			ssvs::BitmapFont* obStroked{nullptr};
			ssvs::BitmapFont* obBigStroked{nullptr};

			// Tilesets
			ssvs::Tileset* tsSmall{nullptr};
			ssvs::Tileset* tsMedium{nullptr};
			ssvs::Tileset* tsBig{nullptr};
			ssvs::Tileset* tsGiant{nullptr};

			// Textures
			sf::Texture* txSmall{nullptr};
			sf::Texture* txMedium{nullptr};
			sf::Texture* txBig{nullptr};
			sf::Texture* txGiant{nullptr};

			// Small tileset (10x10)
			sf::IntRect p1Stand,		p1Shoot,		p1Gun;								// Player class 1
			sf::IntRect p2Stand,		p2Shoot,		p2Gun;								// Player class 2
			sf::IntRect p3Stand,		p3Shoot,		p3Gun;								// Player class 3
			sf::IntRect p4Stand,		p4Shoot,		p4Gun;								// Player class 4
			sf::IntRect e1Stand,		e1Shoot,		e1Gun;								// Runner alien
			sf::IntRect floor,			floorAlt1,		floorAlt2;							// Floor
			sf::IntRect floorGrate,		floorGrateAlt1,	floorGrateAlt2;						// Floor grate
			sf::IntRect pit,			trapdoor,		trapdoorPOnly;						// Pit and trapdoor
			sf::IntRect pjBullet,		pjPlasma,		pjStar,			pjBulletPlasma;		// Projectiles
			sf::IntRect pjStarPlasma,	pjCannonPlasma,	pjRocket,		pjGrenade;			// Projectiles
			sf::IntRect eBall,			eBallFlying;										// Ball enemy
			sf::IntRect eTurret0,		eTurret1,		eTurret2,		eTurret3;			// Turret enemy
			sf::IntRect pPlateSingle,	pPlateMulti,	pPlateOnOff;						// Pressure plates
			sf::IntRect shard;																// Shard
			sf::IntRect explosiveCrate;														// Explosive crate
			sf::IntRect vmHealth;															// Vending machines
			sf::IntRect null0;																// Empty tile
			sf::IntRect gunPCannon,		spawner,		forceArrow,		forceArrowMark;
			sf::IntRect bulletBooster,	bulletChanger,	pjShockwave,	wpnShockwave;

			sf::IntRect ff0,			ff1,			ff2,			ff3,			ff4;	// Force field frames

			sf::IntRect fpj0,			fpj1,			fpj2,			fpj3,			fpj4,
						fpj5,			fpj6,			fpj7,			fpj8,			fpj9;	// Booster frames

			// Medium tileset (20x20)
			sf::IntRect e2Stand,		e2Shoot,		e2Gun,			e2GunGL;			// Charger alien
			sf::IntRect e5Stand;															// Enforcer alien

			// Big tileset (30x30)
			sf::IntRect e3Stand,		e3Shoot,		e3Gun,			e3GunRL;			// Juggernaut alien

			// Giant tileset (40x40)
			sf::IntRect e4Stand;															// Giant alien

			// Animations
			ssvs::Animation aForceField, aBulletBooster;

			// Particle data
			OBParticleData pdBloodRed,		pdGibRed,		pdExplosion,	pdDebris,		pdDebrisFloor;
			OBParticleData pdMuzzleBullet,	pdMuzzlePlasma,	pdMuzzleRocket,	pdPlasma,		pdElectric;
			OBParticleData pdSmoke,			pdShard,		pdCharge,		pdHeal,			pdForceField;
			OBParticleData pdCaseBullet,	pdCaseRocket,	pdShockwave,	pdMuzzleShockwave;

			#define WALLTSDECL(x)	sf::IntRect x ## Single,	x ## Cross,		x ## V,			x ## H, \
												x ## CornerSW,	x ## CornerSE,	x ## CornerNW,	x ## CornerNE, \
												x ## VEndS,		x ## VEndN,		x ## HEndW,		x ## HEndE, \
												x ## TS,		x ## TN,		x ## TW,		x ## TE; \
									std::array<sf::IntRect*, 16> x ## BitMask

			WALLTSDECL(wall); WALLTSDECL(wallD); WALLTSDECL(door); WALLTSDECL(doorG); WALLTSDECL(doorR);
			#undef WALLTSDECL

			inline OBAssets()
			{
				soundPlayer.setVolume(5);
				musicPlayer.setVolume(30);

				ssvs::loadAssetsFromJson(assetManager, "Data/", ssvj::fromFile("Data/assets.json"));

				// Tilesets
				tsSmall = &assetManager.get<ssvs::Tileset>("tsSmall");
				tsMedium = &assetManager.get<ssvs::Tileset>("tsMedium");
				tsBig = &assetManager.get<ssvs::Tileset>("tsBig");
				tsGiant = &assetManager.get<ssvs::Tileset>("tsGiant");

				#define T_TSSMALL(x)	x = (*tsSmall)(#x)
				#define T_TSMEDIUM(x)	x = (*tsMedium)(#x)
				#define T_TSBIG(x)		x = (*tsBig)(#x)
				#define T_TSGIANT(x)	x = (*tsGiant)(#x)

				#define WALLTS(x)	do { \
									T_TSSMALL(x ## Single);		T_TSSMALL(x ## Cross);		T_TSSMALL(x ## V);			T_TSSMALL(x ## H); \
									T_TSSMALL(x ## CornerSW);	T_TSSMALL(x ## CornerSE);	T_TSSMALL(x ## CornerNW);	T_TSSMALL(x ## CornerNE); \
									T_TSSMALL(x ## VEndS);		T_TSSMALL(x ## VEndN);		T_TSSMALL(x ## HEndW);		T_TSSMALL(x ## HEndE); \
									T_TSSMALL(x ## TS);			T_TSSMALL(x ## TN);			T_TSSMALL(x ## TW);			T_TSSMALL(x ## TE); \
									x ## BitMask[0] = &x ## Single;		x ## BitMask[1] = &x ## VEndS;	x ## BitMask[2] = &x ## HEndW; \
									x ## BitMask[3] = &x ## CornerSW;	x ## BitMask[4] = &x ## VEndN;	x ## BitMask[5] = &x ## V; \
									x ## BitMask[6] = &x ## CornerNW;	x ## BitMask[7] = &x ## TW;		x ## BitMask[8] = &x ## HEndE; \
									x ## BitMask[9] = &x ## CornerSE;	x ## BitMask[10] = &x ## H;		x ## BitMask[11] = &x ## TS; \
									x ## BitMask[12] = &x ## CornerNE;	x ## BitMask[13] = &x ## TE;	x ## BitMask[14] = &x ## TN; \
									x ## BitMask[15] = &x ## Cross; } while(false);


				// BitmapFonts
				obStroked = &assetManager.get<ssvs::BitmapFont>("fontObStroked");
				obBigStroked = &assetManager.get<ssvs::BitmapFont>("fontObBigStroked");

				// Textures
				txSmall =	&assetManager.get<sf::Texture>("tsSmall.png");
				txMedium =	&assetManager.get<sf::Texture>("tsMedium.png");
				txBig =	&assetManager.get<sf::Texture>("tsBig.png");
				txGiant =	&assetManager.get<sf::Texture>("tsGiant.png");

				// Small tileset (10x10)
				T_TSSMALL(p1Stand);			T_TSSMALL(p1Shoot);			T_TSSMALL(p1Gun);
				T_TSSMALL(p2Stand);			T_TSSMALL(p2Shoot);			T_TSSMALL(p2Gun);
				T_TSSMALL(p3Stand);			T_TSSMALL(p3Shoot);			T_TSSMALL(p3Gun);
				T_TSSMALL(p4Stand);			T_TSSMALL(p4Shoot);			T_TSSMALL(p4Gun);
				T_TSSMALL(e1Stand);			T_TSSMALL(e1Shoot);			T_TSSMALL(e1Gun);
				T_TSSMALL(floor);			T_TSSMALL(floorAlt1);		T_TSSMALL(floorAlt2);
				T_TSSMALL(floorGrate);		T_TSSMALL(floorGrateAlt1);	T_TSSMALL(floorGrateAlt2);
				T_TSSMALL(pit);				T_TSSMALL(trapdoor);		T_TSSMALL(trapdoorPOnly);
				T_TSSMALL(pjBullet);		T_TSSMALL(pjPlasma);		T_TSSMALL(pjStar);			T_TSSMALL(pjBulletPlasma);
				T_TSSMALL(pjStarPlasma);	T_TSSMALL(pjCannonPlasma);	T_TSSMALL(pjRocket);		T_TSSMALL(pjGrenade);
				T_TSSMALL(eBall);			T_TSSMALL(eBallFlying);
				T_TSSMALL(eTurret0);		T_TSSMALL(eTurret1);		T_TSSMALL(eTurret2);		T_TSSMALL(eTurret3);
				T_TSSMALL(pPlateSingle);	T_TSSMALL(pPlateMulti);		T_TSSMALL(pPlateOnOff);
				T_TSSMALL(shard);
				T_TSSMALL(explosiveCrate);
				T_TSSMALL(vmHealth);
				T_TSSMALL(null0);
				T_TSSMALL(gunPCannon);		T_TSSMALL(spawner);			T_TSSMALL(forceArrow);		T_TSSMALL(forceArrowMark);
				T_TSSMALL(bulletBooster);	T_TSSMALL(bulletChanger);	T_TSSMALL(pjShockwave);		T_TSSMALL(wpnShockwave);

				T_TSSMALL(ff0);				T_TSSMALL(ff1);				T_TSSMALL(ff2);				T_TSSMALL(ff3);				T_TSSMALL(ff4);

				T_TSSMALL(fpj0);			T_TSSMALL(fpj1);			T_TSSMALL(fpj2);			T_TSSMALL(fpj3);			T_TSSMALL(fpj4);
				T_TSSMALL(fpj5);			T_TSSMALL(fpj6);			T_TSSMALL(fpj7);			T_TSSMALL(fpj8);			T_TSSMALL(fpj9);

				WALLTS(wall); WALLTS(wallD); WALLTS(door); WALLTS(doorG); WALLTS(doorR);

				// Medium tileset (20x20)
				T_TSMEDIUM(e2Stand);		T_TSMEDIUM(e2Shoot);		T_TSMEDIUM(e2Gun);			T_TSMEDIUM(e2GunGL);
				T_TSMEDIUM(e5Stand);

				// Big tileset (30x30)
				T_TSBIG(e3Stand);			T_TSBIG(e3Shoot);			T_TSBIG(e3Gun);				T_TSBIG(e3GunRL);

				// Giant tileset (40x40)
				T_TSGIANT(e4Stand);

				// Animations
				ssvj::Val jAForceField{ssvj::fromFile("Data/Animations/forceField.json")};
				aForceField = ssvs::getAnimationFromJson(*tsSmall, jAForceField["on"]);

				ssvj::Val jABulletBooster{ssvj::fromFile("Data/Animations/bulletBooster.json")};
				aBulletBooster = ssvs::getAnimationFromJson(*tsSmall, jABulletBooster["on"]);

				// Particle data
				auto gpd([this](const std::string& mName){ return ssvj::fromFile("Data/Particles/" + mName + ".json").as<OBParticleData>(); });
				pdBloodRed =		gpd("bloodRed");
				pdGibRed =			gpd("gibRed");
				pdExplosion =		gpd("explosion");
				pdDebris =			gpd("debris");
				pdDebrisFloor =		gpd("debrisFloor");
				pdMuzzleBullet =	gpd("muzzleBullet");
				pdMuzzlePlasma =	gpd("muzzlePlasma");
				pdMuzzleRocket =	gpd("muzzleRocket");
				pdPlasma =			gpd("plasma");
				pdElectric =		gpd("electric");
				pdSmoke =			gpd("smoke");
				pdShard =			gpd("shard");
				pdCharge =			gpd("charge");
				pdHeal =			gpd("heal");
				pdForceField =		gpd("forceField");
				pdCaseBullet =		gpd("caseBullet");
				pdCaseRocket =		gpd("caseRocket");
				pdShockwave =		gpd("shockwave");
				pdMuzzleShockwave =	gpd("muzzleShockwave");

				#undef T_TSSMALL
				#undef T_TSMEDIUM
				#undef T_TSBIG
				#undef T_TSGIANT
				#undef WALLTS

			}

			inline auto& operator()() noexcept { return assetManager; }
			template<typename T> inline T& get(const std::string& mId) { return assetManager.get<T>(mId); }

			inline void playSound(const std::string& mName, ssvs::SoundPlayer::Mode mMode = ssvs::SoundPlayer::Mode::Overlap, float mPitch = 1.f)
			{
				if(!OBConfig::isSoundEnabled()) return;
				soundPlayer.play(get<sf::SoundBuffer>(mName), mMode, mPitch);
			}
			inline void playMusic(const std::string& mName)
			{
				if(!OBConfig::isMusicEnabled()) return;
				musicPlayer.play(get<sf::Music>(mName));
				musicPlayer.setLoop(true);
			}

			inline const sf::IntRect& getFloorVariant() const noexcept		{ return ssvu::getRndI(0, 10) < 9 ? floor : (ssvu::getRndI(0, 2) < 1 ? floorAlt1 : floorAlt2); }
			inline const sf::IntRect& getFloorGrateVariant() const noexcept	{ return ssvu::getRndI(0, 10) < 9 ? floorGrate : (ssvu::getRndI(0, 2) < 1 ? floorGrateAlt1 : floorGrateAlt2); }
	};
}

#endif
