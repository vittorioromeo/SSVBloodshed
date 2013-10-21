// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_LEVELEDITOR_DATABASE
#define SSVOB_LEVELEDITOR_DATABASE

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBAssets.h"

namespace ob
{
	template<typename TGame , typename TLevel, typename TTile, typename TFactory> class OBLEDatabaseImpl
	{
		public:
			struct Entry
			{
				OBLETType type;
				sf::Texture* texture;
				sf::IntRect intRect;
				std::map<std::string, ssvuj::Obj> defaultParams;
				ssvu::Func<void(TLevel&, TTile&, const Vec2i&)> spawn;

				Entry() = default;
				Entry(OBLETType mType, sf::Texture* mTexture, const sf::IntRect& mIntRect, const decltype(defaultParams)& mDefaultParams, decltype(spawn) mSpawn)
					: type{mType}, texture{mTexture}, intRect{mIntRect}, defaultParams{mDefaultParams}, spawn{mSpawn} { }
			};

		private:
			OBAssets& a;
			TGame* game;
			TFactory* f{nullptr};
			std::map<OBLETType, Entry> entries;

			template<typename T> inline T getP(TTile& mT, const std::string& mKey) { return mT.template getParam<T>(mKey); }

			inline int getWallMask(TLevel& mL, OBLETType mType, int mX, int mY, int mZ)
			{
				int mask{0}, maxX{mL.getColumns()}, maxY{mL.getRows()};
				auto tileIs = [&](int mTX, int mTY, OBLETType mType){ if(mTX < 0 || mTY < 0 || mTX >= maxX || mTY >= maxY) return false; return mL.getTile(mTX, mTY, mZ).getType() == mType; };
				mask += tileIs(mX, mY - 1, mType) << 0;
				mask += tileIs(mX + 1, mY, mType) << 1;
				mask += tileIs(mX, mY + 1, mType) << 2;
				mask += tileIs(mX - 1, mY, mType) << 3;
				return mask;
			}

		public:
			inline OBLEDatabaseImpl(OBAssets& mAssets, TGame* mGame = nullptr) : a(mAssets), game{mGame}
			{
				if(game != nullptr) f = &game->getFactory();

				add(OBLETType::LETFloor,			a.txSmall,		a.floor,				{},							[this](TLevel&, TTile&, const Vec2i& mP){ f->createFloor(mP, false); });
				add(OBLETType::LETGrate,			a.txSmall,		a.floorGrate,			{},							[this](TLevel&, TTile&, const Vec2i& mP){ f->createFloor(mP, true); });
				add(OBLETType::LETPit,				a.txSmall,		a.pit,					{},							[this](TLevel&, TTile&, const Vec2i& mP){ f->createPit(mP); });
				add(OBLETType::LETTurretSP,			a.txSmall,		a.eTurret0,				{{"rot", 0}},				[this](TLevel&, TTile& mT, const Vec2i& mP){ f->createETurretStarPlasma(mP, getDir8FromDeg(getP<float>(mT, "rot"))); });
				add(OBLETType::LETTurretCP,			a.txSmall,		a.eTurret1,				{{"rot", 0}},				[this](TLevel&, TTile& mT, const Vec2i& mP){ f->createETurretCannonPlasma(mP, getDir8FromDeg(getP<float>(mT, "rot"))); });
				add(OBLETType::LETTurretBP,			a.txSmall,		a.eTurret2,				{{"rot", 0}},				[this](TLevel&, TTile& mT, const Vec2i& mP){ f->createETurretBulletPlasma(mP, getDir8FromDeg(getP<float>(mT, "rot"))); });
				add(OBLETType::LETPlayer,			a.txSmall,		a.p1Stand,				{{"rot", 0}},				[this](TLevel&, TTile&, const Vec2i& mP){ f->createPlayer(mP); });
				add(OBLETType::LETRunner,			a.txSmall,		a.e1Stand,				{{"rot", 0}},				[this](TLevel&, TTile&, const Vec2i& mP){ f->createERunner(mP, false); });
				add(OBLETType::LETRunnerArmed,		a.txSmall,		a.e1Shoot,				{{"rot", 0}},				[this](TLevel&, TTile&, const Vec2i& mP){ f->createERunner(mP, true); });
				add(OBLETType::LETCharger,			a.txMedium,		a.e2Stand,				{{"rot", 0}},				[this](TLevel&, TTile&, const Vec2i& mP){ f->createECharger(mP, false); });
				add(OBLETType::LETChargerArmed,		a.txMedium,		a.e2Shoot,				{{"rot", 0}},				[this](TLevel&, TTile&, const Vec2i& mP){ f->createECharger(mP, true); });
				add(OBLETType::LETJuggernaut,		a.txBig,		a.e3Stand,				{{"rot", 0}},				[this](TLevel&, TTile&, const Vec2i& mP){ f->createEJuggernaut(mP, false); });
				add(OBLETType::LETJuggernautArmed,	a.txBig,		a.e3Shoot,				{{"rot", 0}},				[this](TLevel&, TTile&, const Vec2i& mP){ f->createEJuggernaut(mP, true); });
				add(OBLETType::LETGiant,			a.txGiant,		a.e4Stand,				{{"rot", 0}},				[this](TLevel&, TTile&, const Vec2i& mP){ f->createEGiant(mP); });
				add(OBLETType::LETBall,				a.txSmall,		a.eBall,				{},							[this](TLevel&, TTile&, const Vec2i& mP){ f->createEBall(mP, false, false); });
				add(OBLETType::LETBallFlying,		a.txSmall,		a.eBallFlying,			{},							[this](TLevel&, TTile&, const Vec2i& mP){ f->createEBall(mP, true, false); });
				add(OBLETType::LETEnforcer,			a.txMedium,		a.e5Stand,				{{"rot", 0}},				[this](TLevel&, TTile&, const Vec2i& mP){ f->createEEnforcer(mP); });
				add(OBLETType::LETTrapdoor,			a.txSmall,		a.trapdoor,				{},							[this](TLevel&, TTile&, const Vec2i& mP){ f->createTrapdoor(mP, false); });
				add(OBLETType::LETTrapdoorPOnly,	a.txSmall,		a.trapdoorPOnly,		{},							[this](TLevel&, TTile&, const Vec2i& mP){ f->createTrapdoor(mP, true); });
				add(OBLETType::LETExplosiveCrate,	a.txSmall,		a.explosiveCrate,		{{"id", -1}},				[this](TLevel&, TTile& mT, const Vec2i& mP){ f->createFloor(mP, true); f->createExplosiveCrate(mP, getP<int>(mT, "id")); });
				add(OBLETType::LETVMHealth,			a.txSmall,		a.vmHealth,				{},							[this](TLevel&, TTile&, const Vec2i& mP){ f->createVMHealth(mP); });

				add(OBLETType::LETWall,				a.txSmall,		a.wallSingle,			{},
				[this](TLevel& mL, TTile& mT, const Vec2i& mP)
				{
					int mask{getWallMask(mL, OBLETType::LETWall, mT.getX(), mT.getY(), mT.getZ())};
					f->createWall(mP, *a.wallBitMask[mask]);
				});

				add(OBLETType::LETWallD,			a.txSmall,		a.wallDSingle,			{},
				[this](TLevel& mL, TTile& mT, const Vec2i& mP)
				{
					int mask{getWallMask(mL, OBLETType::LETWallD, mT.getX(), mT.getY(), mT.getZ())};
					f->createFloor(mP, true); f->createWallDestructible(mP, *a.wallDBitMask[mask]);
				});

				add(OBLETType::LETDoor,				a.txSmall,		a.doorSingle,			{{"id", 0}, {"open", 0}},
				[this](TLevel& mL, TTile& mT, const Vec2i& mP)
				{
					int mask{getWallMask(mL, OBLETType::LETDoor, mT.getX(), mT.getY(), mT.getZ())};
					f->createFloor(mP, true); f->createDoor(mP, *a.doorBitMask[mask], getP<int>(mT, "id"), bool(getP<int>(mT, "open")));
				});

				add(OBLETType::LETDoorG,			a.txSmall,		a.doorGSingle,			{{"open", 0}},
				[this](TLevel& mL, TTile& mT, const Vec2i& mP)
				{
					int mask{getWallMask(mL, OBLETType::LETDoorG, mT.getX(), mT.getY(), mT.getZ())};
					f->createFloor(mP, true); f->createDoorG(mP, *a.doorGBitMask[mask], bool(getP<int>(mT, "open")));
				});

				add(OBLETType::LETDoorR,			a.txSmall,		a.doorRSingle,			{{"open", 0}},
				[this](TLevel& mL, TTile& mT, const Vec2i& mP)
				{
					int mask{getWallMask(mL, OBLETType::LETDoorR, mT.getX(), mT.getY(), mT.getZ())};
					f->createFloor(mP, true); f->createDoorR(mP, *a.doorRBitMask[mask], bool(getP<int>(mT, "open")));
				});

				add(OBLETType::LETSpawner,			a.txSmall,		a.shard,				{{"id", -1}, {"type", 0}, {"delayStart", 0.f}, {"delaySpawn", 200.f}, {"spawnCount", 1}},
				[this](TLevel&, TTile& mT, const Vec2i& mP)
				{
					f->createSpawner(mP, getP<int>(mT, "type"), getP<int>(mT, "id"), getP<float>(mT, "delayStart"), getP<float>(mT, "delaySpawn"), getP<int>(mT, "spawnCount"));
				});

				add(OBLETType::LETPPlateSingle,		a.txSmall,		a.pPlateSingle,			{{"id", 0}, {"action", 0}, {"playerOnly", false}},
				[this](TLevel&, TTile& mT, const Vec2i& mP)
				{
					f->createPPlate(mP, getP<int>(mT, "id"), PPlateType::Single, IdAction(getP<int>(mT, "action")), getP<bool>(mT, "playerOnly"));
				});

				add(OBLETType::LETPPlateMulti,		a.txSmall,		a.pPlateMulti,			{{"id", 0}, {"action", 0}, {"playerOnly", false}},
				[this](TLevel&, TTile& mT, const Vec2i& mP)
				{
					f->createPPlate(mP, getP<int>(mT, "id"), PPlateType::Multi, IdAction(getP<int>(mT, "action")), getP<bool>(mT, "playerOnly"));
				});

				add(OBLETType::LETPPlateOnOff,		a.txSmall,		a.pPlateOnOff,			{{"id", 0}, {"action", 0}, {"playerOnly", false}},
				[this](TLevel&, TTile& mT, const Vec2i& mP)
				{
					f->createPPlate(mP, getP<int>(mT, "id"), PPlateType::OnOff, IdAction(getP<int>(mT, "action")), getP<bool>(mT, "playerOnly"));
				});
			}

			template<typename T> inline void add(OBLETType mType, sf::Texture* mTexture, const sf::IntRect& mIntRect, const std::map<std::string, ssvuj::Obj>& mDefaultParams, T mSpawn)
			{
				entries[mType] = Entry{mType, mTexture, mIntRect, mDefaultParams, mSpawn};
			}
			inline void spawn(TLevel& mLevel, TTile& mTile, const Vec2i& mPos) { get(mTile.getType()).spawn(mLevel, mTile, mPos); }

			inline const Entry& get(OBLETType mType) const	{ return entries.at(mType); }
			inline int getSize() const noexcept				{ return entries.size(); }
	};

	// Template magic to temporarily avoid creating .cpp source files
	class OBGame;
	class OBFactory;
	class OBLELevel;
	class OBLETile;

	using OBLEDatabase = OBLEDatabaseImpl<OBGame, OBLELevel, OBLETile, OBFactory>;
	using OBLEDatabaseEntry = OBLEDatabase::Entry;
}

#endif
