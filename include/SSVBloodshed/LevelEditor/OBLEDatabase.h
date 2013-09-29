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

		public:
			inline OBLEDatabaseImpl(OBAssets& mAssets, TGame* mGame = nullptr) : a(mAssets), game{mGame}
			{
				if(game != nullptr) f = &game->getFactory();

				add(OBLETType::LETFloor,			a.txSmall,		a.floor,				{},						[this](TLevel&, TTile&, const Vec2i& mP){ f->createFloor(mP, false); });
				add(OBLETType::LETGrate,			a.txSmall,		a.floorGrate,			{},						[this](TLevel&, TTile&, const Vec2i& mP){ f->createFloor(mP, true); });
				add(OBLETType::LETPit,				a.txSmall,		a.pit,					{},						[this](TLevel&, TTile&, const Vec2i& mP){ f->createPit(mP); });
				add(OBLETType::LETSpawner,			a.txSmall,		a.pjCannonPlasma,		{{"spawns", {}}},		[this](TLevel&, TTile&, const Vec2i&){ /* TODO: */ });
				add(OBLETType::LETTurret,			a.txSmall,		a.eTurret,				{{"rot", 0}},			[this](TLevel&, TTile& mT, const Vec2i& mP){ f->createETurret(mP, getDir8FromDeg(mT.template getParam<float>("rot"))); });
				add(OBLETType::LETPlayer,			a.txSmall,		a.p1Stand,				{{"rot", 0}},			[this](TLevel&, TTile&, const Vec2i& mP){ f->createPlayer(mP); });
				add(OBLETType::LETRunner,			a.txSmall,		a.e1UAStand,			{{"rot", 0}},			[this](TLevel&, TTile&, const Vec2i& mP){ f->createERunner(mP, false); });
				add(OBLETType::LETRunnerArmed,		a.txSmall,		a.e1AStand,				{{"rot", 0}},			[this](TLevel&, TTile&, const Vec2i& mP){ f->createERunner(mP, true); });
				add(OBLETType::LETCharger,			a.txMedium,		a.e2UAStand,			{{"rot", 0}},			[this](TLevel&, TTile&, const Vec2i& mP){ f->createECharger(mP, false); });
				add(OBLETType::LETChargerArmed,		a.txMedium,		a.e2AStand,				{{"rot", 0}},			[this](TLevel&, TTile&, const Vec2i& mP){ f->createECharger(mP, true); });
				add(OBLETType::LETJuggernaut,		a.txBig,		a.e3UAStand,			{{"rot", 0}},			[this](TLevel&, TTile&, const Vec2i& mP){ f->createEJuggernaut(mP, false); });
				add(OBLETType::LETJuggernautArmed,	a.txBig,		a.e3AStand,				{{"rot", 0}},			[this](TLevel&, TTile&, const Vec2i& mP){ f->createEJuggernaut(mP, true); });
				add(OBLETType::LETGiant,			a.txGiant,		a.e4UAStand,			{{"rot", 0}},			[this](TLevel&, TTile&, const Vec2i& mP){ f->createEGiant(mP); });
				add(OBLETType::LETBall,				a.txSmall,		a.eBall,				{},						[this](TLevel&, TTile&, const Vec2i& mP){ f->createEBall(mP, false, false); });
				add(OBLETType::LETBallFlying,		a.txSmall,		a.eBallFlying,			{},						[this](TLevel&, TTile&, const Vec2i& mP){ f->createEBall(mP, true, false); });

				add(OBLETType::LETWall,				a.txSmall,		a.wallSingle,			{},
				[this](TLevel& mL, TTile& mT, const Vec2i& mP)
				{
					constexpr int maxX{320 / 10}, maxY{240 / 10 - 2}; // TODO: get level size from mL
					int x{mT.getX()}, y{mT.getY()};
					auto tileIs = [&](int mX, int mY, OBLETType mType){ if(mX < 0 || mY < 0 || mX >= maxX || mY >= maxY) return false; return mL.getTile(mX, mY, 0).getType() == mType; };

					int mask{0};
					mask += tileIs(x, y - 1, OBLETType::LETWall) << 0;
					mask += tileIs(x + 1, y, OBLETType::LETWall) << 1;
					mask += tileIs(x, y + 1, OBLETType::LETWall) << 2;
					mask += tileIs(x - 1, y, OBLETType::LETWall) << 3;
					f->createWall(mP, *a.wallBitMask[mask]);
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
