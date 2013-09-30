// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_ASSETS
#define SSVOB_ASSETS

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBConfig.h"

namespace ob
{
	class OBAssets
	{
		private:
			ssvs::AssetManager assetManager;

		public:
			ssvs::SoundPlayer soundPlayer;
			ssvs::MusicPlayer musicPlayer;

			// BitmapFonts
			ssvs::BitmapFont* obStroked;

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
			sf::IntRect e1UAStand, 		e1AStand,		e1AShoot,		e1AGun;				// Runner alien
			sf::IntRect floor,			floorAlt1,		floorAlt2;							// Floor
			sf::IntRect floorGrate,		floorGrateAlt1,	floorGrateAlt2;						// Floor grate
			sf::IntRect wallSingle,		wallCross,		wallV,			wallH;				// Wall
			sf::IntRect wallCornerSW,	wallCornerSE,	wallCornerNW,	wallCornerNE;		// Wall corners
			sf::IntRect wallVEndS,		wallVEndN,		wallHEndW,		wallHEndE;			// Wall ends
			sf::IntRect wallTS,			wallTN,			wallTW,			wallTE;				// Wall T
			sf::IntRect pit;																// Pit
			sf::IntRect pjBullet,		pjPlasma,		pjStar,			pjBulletPlasma;		// Projectiles
			sf::IntRect pjStarPlasma,	pjCannonPlasma;										// Projectiles
			sf::IntRect eBall,			eBallFlying;										// Ball enemy
			sf::IntRect eTurret0,		eTurret1,		eTurret2;							// Turret enemy
			sf::IntRect wallDSingle,	wallDCross,		wallDV,			wallDH;				// Destructible wall
			sf::IntRect wallDCornerSW,	wallDCornerSE,	wallDCornerNW,	wallDCornerNE;		// Destructible wall corners
			sf::IntRect wallDVEndS,		wallDVEndN,		wallDHEndW,		wallDHEndE;			// Destructible wall ends
			sf::IntRect wallDTS,		wallDTN,		wallDTW,		wallDTE;			// Destructible wall T
			sf::IntRect pPlateOn,		pPlateOff;											// Pressure plates
			sf::IntRect doorSingle,		doorCross,		doorV,			doorH;				// Door
			sf::IntRect doorCornerSW,	doorCornerSE,	doorCornerNW,	doorCornerNE;		// Door corners
			sf::IntRect doorVEndS,		doorVEndN,		doorHEndW,		doorHEndE;			// Door ends
			sf::IntRect doorTS,			doorTN,			doorTW,			doorTE;				// Door T

			// Medium tileset (20x20)
			sf::IntRect e2UAStand,		e2AStand,		e2AShoot,		e2AGun;				// Charger alien
			sf::IntRect e5UAStand;															// Enforcer alien

			// Big tileset (30x30)
			sf::IntRect e3UAStand,		e3AStand,		e3AShoot,		e3AGun;				// Juggernaut alien

			// Giant tileset (40x40)
			sf::IntRect e4UAStand;															// Giant alien

			// Bitmasks
			std::array<sf::IntRect*, 16> wallBitMask;
			std::array<sf::IntRect*, 16> wallDBitMask;
			std::array<sf::IntRect*, 16> doorBitMask;

			inline OBAssets()
			{
				soundPlayer.setVolume(5);
				musicPlayer.setVolume(30);

				ssvs::loadAssetsFromJson(assetManager, "Data/", ssvuj::readFromFile("Data/assets.json"));

				// Tileset references
				const auto& tsSmall(assetManager.get<ssvs::Tileset>("tsSmall"));
				const auto& tsMedium(assetManager.get<ssvs::Tileset>("tsMedium"));
				const auto& tsBig(assetManager.get<ssvs::Tileset>("tsBig"));
				const auto& tsGiant(assetManager.get<ssvs::Tileset>("tsGiant"));

				#define T_TSSMALL(x)	x = tsSmall(#x)
				#define T_TSMEDIUM(x)	x = tsMedium(#x)
				#define T_TSBIG(x)		x = tsBig(#x)
				#define T_TSGIANT(x)	x = tsGiant(#x)

				// BitmapFonts
				obStroked = &assetManager.get<ssvs::BitmapFont>("fontObStroked");

				// Textures
				txSmall = 	&assetManager.get<sf::Texture>("tsSmall.png");
				txMedium = 	&assetManager.get<sf::Texture>("tsMedium.png");
				txBig = 	&assetManager.get<sf::Texture>("tsBig.png");
				txGiant = 	&assetManager.get<sf::Texture>("tsGiant.png");

				// Small tileset (10x10)
				T_TSSMALL(p1Stand);			T_TSSMALL(p1Shoot);			T_TSSMALL(p1Gun);
				T_TSSMALL(p2Stand);			T_TSSMALL(p2Shoot);			T_TSSMALL(p2Gun);
				T_TSSMALL(p3Stand);			T_TSSMALL(p3Shoot);			T_TSSMALL(p3Gun);
				T_TSSMALL(p4Stand);			T_TSSMALL(p4Shoot);			T_TSSMALL(p4Gun);
				T_TSSMALL(e1UAStand); 		T_TSSMALL(e1AStand);		T_TSSMALL(e1AShoot);		T_TSSMALL(e1AGun);
				T_TSSMALL(floor);			T_TSSMALL(floorAlt1);		T_TSSMALL(floorAlt2);
				T_TSSMALL(floorGrate);		T_TSSMALL(floorGrateAlt1);	T_TSSMALL(floorGrateAlt2);
				T_TSSMALL(wallSingle);		T_TSSMALL(wallCross);		T_TSSMALL(wallV);			T_TSSMALL(wallH);
				T_TSSMALL(wallCornerSW);	T_TSSMALL(wallCornerSE);	T_TSSMALL(wallCornerNW);	T_TSSMALL(wallCornerNE);
				T_TSSMALL(wallVEndS);		T_TSSMALL(wallVEndN);		T_TSSMALL(wallHEndW);		T_TSSMALL(wallHEndE);
				T_TSSMALL(wallTS);			T_TSSMALL(wallTN);			T_TSSMALL(wallTW);			T_TSSMALL(wallTE);
				T_TSSMALL(pit);
				T_TSSMALL(pjBullet);		T_TSSMALL(pjPlasma);		T_TSSMALL(pjStar);			T_TSSMALL(pjBulletPlasma);
				T_TSSMALL(pjStarPlasma);	T_TSSMALL(pjCannonPlasma);
				T_TSSMALL(eBall);			T_TSSMALL(eBallFlying);
				T_TSSMALL(eTurret0);		T_TSSMALL(eTurret1);		T_TSSMALL(eTurret2);
				T_TSSMALL(wallDSingle);		T_TSSMALL(wallDCross);		T_TSSMALL(wallDV);			T_TSSMALL(wallDH);
				T_TSSMALL(wallDCornerSW);	T_TSSMALL(wallDCornerSE);	T_TSSMALL(wallDCornerNW);	T_TSSMALL(wallDCornerNE);
				T_TSSMALL(wallDVEndS);		T_TSSMALL(wallDVEndN);		T_TSSMALL(wallDHEndW);		T_TSSMALL(wallDHEndE);
				T_TSSMALL(wallDTS);			T_TSSMALL(wallDTN);			T_TSSMALL(wallDTW);			T_TSSMALL(wallDTE);
				T_TSSMALL(pPlateOn);		T_TSSMALL(pPlateOff);
				T_TSSMALL(doorSingle);		T_TSSMALL(doorCross);		T_TSSMALL(doorV);			T_TSSMALL(doorH);
				T_TSSMALL(doorCornerSW);	T_TSSMALL(doorCornerSE);	T_TSSMALL(doorCornerNW);	T_TSSMALL(doorCornerNE);
				T_TSSMALL(doorVEndS);		T_TSSMALL(doorVEndN);		T_TSSMALL(doorHEndW);		T_TSSMALL(doorHEndE);
				T_TSSMALL(doorTS);			T_TSSMALL(doorTN);			T_TSSMALL(doorTW);			T_TSSMALL(doorTE);

				// Medium tileset (20x20)
				T_TSMEDIUM(e2UAStand); 		T_TSMEDIUM(e2AStand);		T_TSMEDIUM(e2AShoot);		T_TSMEDIUM(e2AGun);
				T_TSMEDIUM(e5UAStand);

				// Big tileset (30x30)
				T_TSBIG(e3UAStand);			T_TSBIG(e3AStand);			T_TSBIG(e3AShoot);			T_TSBIG(e3AGun);

				// Giant tileset (40x40)
				T_TSGIANT(e4UAStand);

				#undef T_TSSMALL
				#undef T_TSMEDIUM
				#undef T_TSBIG
				#undef T_TSGIANT

				// Wall bitmask
				wallBitMask[0] = &wallSingle;		wallBitMask[1] = &wallVEndS;	wallBitMask[2] = &wallHEndW;
				wallBitMask[3] = &wallCornerSW;		wallBitMask[4] = &wallVEndN;	wallBitMask[5] = &wallV;
				wallBitMask[6] = &wallCornerNW;		wallBitMask[7] = &wallTW;		wallBitMask[8] = &wallHEndE;
				wallBitMask[9] = &wallCornerSE;		wallBitMask[10] = &wallH;		wallBitMask[11] = &wallTS;
				wallBitMask[12] = &wallCornerNE;	wallBitMask[13] = &wallTE;		wallBitMask[14] = &wallTN;
				wallBitMask[15] = &wallCross;

				// Destructible wall bitmask
				wallDBitMask[0] = &wallDSingle;		wallDBitMask[1] = &wallDVEndS;	wallDBitMask[2] = &wallDHEndW;
				wallDBitMask[3] = &wallDCornerSW;	wallDBitMask[4] = &wallDVEndN;	wallDBitMask[5] = &wallDV;
				wallDBitMask[6] = &wallDCornerNW;	wallDBitMask[7] = &wallDTW;		wallDBitMask[8] = &wallDHEndE;
				wallDBitMask[9] = &wallDCornerSE;	wallDBitMask[10] = &wallDH;		wallDBitMask[11] = &wallDTS;
				wallDBitMask[12] = &wallDCornerNE;	wallDBitMask[13] = &wallDTE;	wallDBitMask[14] = &wallDTN;
				wallDBitMask[15] = &wallDCross;

				// Door bitmask
				doorBitMask[0] = &doorSingle;		doorBitMask[1] = &doorVEndS;	doorBitMask[2] = &doorHEndW;
				doorBitMask[3] = &doorCornerSW;	doorBitMask[4] = &doorVEndN;	doorBitMask[5] = &doorV;
				doorBitMask[6] = &doorCornerNW;	doorBitMask[7] = &doorTW;		doorBitMask[8] = &doorHEndE;
				doorBitMask[9] = &doorCornerSE;	doorBitMask[10] = &doorH;		doorBitMask[11] = &doorTS;
				doorBitMask[12] = &doorCornerNE;	doorBitMask[13] = &doorTE;	doorBitMask[14] = &doorTN;
				doorBitMask[15] = &doorCross;
			}

			inline ssvs::AssetManager& operator()() noexcept { return assetManager; }
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

			inline const sf::IntRect& getFloorVariant() const noexcept		{ return ssvu::getRnd(0, 10) < 9 ? floor : (ssvu::getRnd(0, 2) < 1 ? floorAlt1 : floorAlt2); }
			inline const sf::IntRect& getFloorGrateVariant() const noexcept	{ return ssvu::getRnd(0, 10) < 9 ? floorGrate : (ssvu::getRnd(0, 2) < 1 ? floorGrateAlt1 : floorGrateAlt2); }
	};
}

#endif
