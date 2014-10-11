// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_SHAREDDATA
#define SSVOB_SHAREDDATA

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBAssets.hpp"
#include "SSVBloodshed/OBConfig.hpp"
#include "SSVBloodshed/LevelEditor/OBLEJson.hpp"
#include "SSVBloodshed/LevelEditor/OBLEDatabase.hpp"
#include "SSVBloodshed/LevelEditor/OBLEPack.hpp"
#include "SSVBloodshed/LevelEditor/OBLESector.hpp"
#include "SSVBloodshed/LevelEditor/OBLELevel.hpp"

namespace ob
{
	class OBSharedData
	{
		private:
			OBLEDatabase* database{nullptr};
			OBLEPack pack;
			OBLESector* currentSector{nullptr};
			OBLELevel* currentLevel{nullptr};
			ssvufs::Path currentPath;
			int currentSectorIdx{0}, currentLevelX{0}, currentLevelY{0};

			inline void setPath(const ssvufs::Path& mPath)
			{
				currentPath = mPath;
				SSVU_ASSERT(!currentPath.isNull());
			}

		public:
			inline void createEmptyPack()
			{
				SSVU_ASSERT(database != nullptr);
				pack = OBLEPack{};
				currentSector = nullptr;
				currentLevel = nullptr;
			}
			inline void loadPack(const ssvufs::Path& mPath)
			{
				SSVU_ASSERT(database != nullptr && mPath.exists<ssvufs::Type::File>());
				setPath(mPath);

				try { pack = ssvj::Val::fromFile(currentPath).as<OBLEPack>(); }
				catch(...) { ssvu::lo("Fatal error") << "Failed to load pack" << std::endl; }
			}
			inline void savePack(const ssvufs::Path& mPath)
			{
				setPath(mPath);

				try	{ ssvj::Val{pack}.writeToFile<ssvj::WSMinified>(currentPath); }
				catch(...) { ssvu::lo("Fatal error") << "Failed to save pack" << std::endl; }
			}

			inline void setDatabase(OBLEDatabase& mDatabase, OBGame* mGame = nullptr) noexcept
			{
				database = &mDatabase;
				if(mGame != nullptr) database->setGame(*mGame);
			}
			inline void setCurrentSector(int mIdx)
			{
				SSVU_ASSERT(database != nullptr);
				if(!isSectorValid(mIdx)) ssvu::lo("Warning") << "Sector of idx " << mIdx << " is not valid\n";

				currentSectorIdx = mIdx;
				currentSector = &pack.getSector(mIdx);
			}
			inline void setCurrentLevel(int mX, int mY)
			{
				if(!isLevelValid(mX, mY)) ssvu::lo("Warning") << "Level (" << mX << ";" << mY << ") is not valid\n";

				currentLevelX = mX; currentLevelY = mY;
				currentLevel = &getCurrentSector().getLevel(mX, mY);
			}

			inline OBLEDatabase& getDatabase() noexcept							{ return *database; }
			inline OBLEPack& getPack() noexcept									{ return pack; }
			inline OBLESector& getCurrentSector() const noexcept				{ SSVU_ASSERT(currentSector != nullptr); return *currentSector; }
			inline OBLELevel& getCurrentLevel() const noexcept					{ SSVU_ASSERT(currentLevel != nullptr); return *currentLevel; }
			inline decltype(currentLevel->getTiles()) getCurrentTiles() const	{ return getCurrentLevel().getTiles(); }
			inline const ssvufs::Path& getCurrentPath() const noexcept			{ return currentPath; }

			inline bool isCurrentLevelNull() const noexcept						{ return currentLevel == nullptr; }
			inline bool isSectorValid(int mIdx) const noexcept					{ return pack.isValid(mIdx); }
			inline bool isLevelValid(int mX, int mY) const noexcept				{ return getCurrentSector().isValid(mX, mY); }
			inline bool isTileValid(int mX, int mY, int mZ) const noexcept		{ return getCurrentLevel().isValid(mX, mY, mZ); }

			inline int getCurrentSectorIdx() const noexcept		{ SSVU_ASSERT(currentSector != nullptr); return currentSectorIdx; }
			inline int getCurrentLevelX() const noexcept		{ SSVU_ASSERT(currentLevel != nullptr); return currentLevelX; }
			inline int getCurrentLevelY() const noexcept		{ SSVU_ASSERT(currentLevel != nullptr); return currentLevelY; }
	};
}

#endif
