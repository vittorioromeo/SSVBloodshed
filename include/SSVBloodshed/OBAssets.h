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

			ssvs::Tileset& tileset{assetManager.load<ssvs::Tileset>("ts", "Data/Tilesets/tileset.json")};
			ssvs::Tileset& tilesetPlayer{assetManager.load<ssvs::Tileset>("tsPlayer", "Data/Tilesets/tilesetPlayer.json")};
			ssvs::Tileset& tilesetProjectiles{assetManager.load<ssvs::Tileset>("tsProjectiles", "Data/Tilesets/tilesetProjectiles.json")};
			ssvs::Tileset& tilesetEnemy{assetManager.load<ssvs::Tileset>("tsEnemy", "Data/Tilesets/tilesetEnemy.json")};
			ssvs::Tileset& tilesetEnemyBig{assetManager.load<ssvs::Tileset>("tsEnemyBig", "Data/Tilesets/tilesetEnemyBig.json")};
			ssvs::Tileset& tilesetJuggernaut{assetManager.load<ssvs::Tileset>("tsJuggernaut", "Data/Tilesets/tilesetJuggernaut.json")};

			inline OBAssets()
			{
				ssvs::loadAssetsFromJson(assetManager, "Data/", ssvuj::readFromFile("Data/assets.json"));

				soundPlayer.setVolume(50);
				musicPlayer.setVolume(30);
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
