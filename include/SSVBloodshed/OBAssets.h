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

			// Textures
			sf::Texture* txSmall{nullptr};
			sf::Texture* txMedium{nullptr};
			sf::Texture* txBig{nullptr};
			sf::Texture* txGiant{nullptr};

			// Small tileset (10x10)
			sf::IntRect p1Stand, p1Shoot, p1Gun;							// Player class 1
			sf::IntRect p2Stand, p2Shoot, p2Gun;							// Player class 2
			sf::IntRect p3Stand, p3Shoot, p3Gun;							// Player class 3
			sf::IntRect p4Stand, p4Shoot, p4Gun;							// Player class 4
			sf::IntRect e1Stand;											// Runner alien
			sf::IntRect e2Stand, e2Shoot, e2Gun;							// RunnerArmed alien
			sf::IntRect floor, floorAlt1, floorAlt2;						// Floor
			sf::IntRect floorGrate, floorGrateAlt1, floorGrateAlt2;			// Floor grate
			sf::IntRect wallSingle;											// Wall
			sf::IntRect pit;												// Pit
			sf::IntRect pjBullet, pjPlasma, pjStar;							// Projectiles
			sf::IntRect eBall, eBallFlying;									// Ball enemy
			sf::IntRect eTurret;											// Turret enemy

			// Medium tileset (20x20)
			sf::IntRect e3Stand;											// Charger alien
			sf::IntRect e6Stand, e6Shoot, e6Gun;							// ChargerArmed alien

			// Big tileset (30x30)
			sf::IntRect e4Stand;											// Juggernaut alien
			sf::IntRect e7Stand, e7Shoot, e7Gun;							// JuggernautArmed alien

			// Giant tileset (40x40)
			sf::IntRect e5Stand;											// Giant alien

			inline OBAssets()
			{
				soundPlayer.setVolume(5);
				musicPlayer.setVolume(30);

				ssvs::loadAssetsFromJson(assetManager, "Data/", ssvuj::readFromFile("Data/assets.json"));

				// Textures
				txSmall = &assetManager.get<sf::Texture>("tsSmall.png");
				txMedium = &assetManager.get<sf::Texture>("tsMedium.png");
				txBig = &assetManager.get<sf::Texture>("tsBig.png");
				txGiant = &assetManager.get<sf::Texture>("tsGiant.png");

				// Small tileset (10x10)
				const auto& tsSmall(assetManager.get<ssvs::Tileset>("tsSmall"));
				p1Stand = tsSmall("p1Stand"); p1Shoot = tsSmall("p1Shoot"); p1Gun = tsSmall("p1Gun");
				p2Stand = tsSmall("p2Stand"); p2Shoot = tsSmall("p2Shoot"); p2Gun = tsSmall("p2Gun");
				p3Stand = tsSmall("p3Stand"); p3Shoot = tsSmall("p3Shoot"); p3Gun = tsSmall("p3Gun");
				p4Stand = tsSmall("p4Stand"); p4Shoot = tsSmall("p4Shoot"); p4Gun = tsSmall("p4Gun");
				e1Stand = tsSmall("e1Stand");
				e2Stand = tsSmall("e2Stand"); e2Shoot = tsSmall("e2Shoot"); e2Gun = tsSmall("e2Gun");
				floor = tsSmall("floor"); floorAlt1 = tsSmall("floorAlt1"); floorAlt2 = tsSmall("floorAlt2");
				floorGrate = tsSmall("floorGrate"); floorGrateAlt1 = tsSmall("floorGrateAlt1"); floorGrateAlt2 = tsSmall("floorGrateAlt2");
				wallSingle = tsSmall("wallSingle");
				pit = tsSmall("pit");
				pjBullet = tsSmall("pjBullet"); pjPlasma = tsSmall("pjPlasma"); pjStar = tsSmall("pjStar");
				eBall = tsSmall("eBall"); eBallFlying = tsSmall("eBallFlying");
				eTurret = tsSmall("eTurret");

				// Medium tileset (20x20)
				const auto& tsMedium(assetManager.get<ssvs::Tileset>("tsMedium"));
				e3Stand = tsMedium("e3Stand");
				e6Stand = tsMedium("e6Stand"); e6Shoot = tsMedium("e6Shoot"); e6Gun = tsMedium("e6Gun");

				// Big tileset (30x30)
				const auto& tsBig(assetManager.get<ssvs::Tileset>("tsBig"));
				e4Stand = tsBig("e4Stand");
				e7Stand = tsBig("e7Stand"); e7Shoot = tsBig("e7Shoot"); e7Gun = tsBig("e7Gun");

				// Giant tileset (40x40)
				const auto& tsGiant(assetManager.get<ssvs::Tileset>("tsGiant"));
				e5Stand = tsGiant("e5Stand");
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
	};
}

#endif
