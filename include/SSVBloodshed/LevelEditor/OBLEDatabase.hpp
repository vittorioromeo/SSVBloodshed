// Copyright (c) 2013-2015 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_LEVELEDITOR_DATABASE
#define SSVOB_LEVELEDITOR_DATABASE

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBAssets.hpp"

namespace ob
{
    template <typename TGame, typename TLevel, typename TTile,
        typename TFactory>
    class OBLEDatabaseImpl
    {
    public:
        struct Entry
        {
            OBLETType type;
            sf::Texture* texture;
            sf::IntRect intRect;
            std::map<std::string, ssvj::Val> defaultParams;
            std::map<std::string, std::string> enumParams;
            ssvu::Func<void(TLevel&, TTile&, const Vec2i&)> spawn;

            inline Entry() = default;
            inline Entry(OBLETType mType, sf::Texture* mTexture,
                const sf::IntRect& mIntRect,
                const decltype(defaultParams)& mDefaultParams,
                decltype(spawn) mSpawn)
                : type{mType}, texture{mTexture}, intRect{mIntRect},
                  defaultParams{mDefaultParams}, spawn{mSpawn}
            {
            }

            inline bool isEnumParam(const std::string& mKey) const
            {
                return enumParams.count(mKey) > 0;
            }
            inline const std::string& getEnumName(const std::string& mKey) const
            {
                return enumParams.at(mKey);
            }
        };

    private:
        OBAssets& a;
        TGame* game{nullptr};
        TFactory* f{nullptr};
        std::map<OBLETType, Entry> entries;

        template <typename T>
        inline T getP(TTile& mT, const std::string& mKey)
        {
            return mT.template getParam<T>(mKey);
        }
        template <typename T>
        inline T getPE(TTile& mT, const std::string& mKey)
        {
            return T(mT.template getParam<int>(mKey));
        }

        inline int getWallMask(
            TLevel& mL, OBLETType mType, int mX, int mY, int mZ)
        {
            int mask{0}, maxX{mL.getColumns()}, maxY{mL.getRows()};

            auto tileIs = [&mL, maxX, maxY, mZ](
                int mTX, int mTY, OBLETType mTType)
            {
                if(mTX < 0 || mTY < 0 || mTX >= maxX || mTY >= maxY)
                    return false;
                return mL.getTile(mTX, mTY, mZ).getType() == mTType;
            };

            mask += tileIs(mX, mY - 1, mType) << 0;
            mask += tileIs(mX + 1, mY, mType) << 1;
            mask += tileIs(mX, mY + 1, mType) << 2;
            mask += tileIs(mX - 1, mY, mType) << 3;
            return mask;
        }

    public:
        inline OBLEDatabaseImpl(OBAssets& mAssets) : a(mAssets)
        {
            add(OBLETType::LETFloor, a.txSmall, a.floor, {},
                [this](TLevel&, TTile&, const Vec2i& mP)
                {
                    f->createFloor(mP, false);
                });
            add(OBLETType::LETGrate, a.txSmall, a.floorGrate, {},
                [this](TLevel&, TTile&, const Vec2i& mP)
                {
                    f->createFloor(mP, true);
                });
            add(OBLETType::LETPit, a.txSmall, a.pit, {},
                [this](TLevel&, TTile&, const Vec2i& mP)
                {
                    f->createPit(mP);
                });
            add(OBLETType::LETTurretSP, a.txSmall, a.eTurret0, {{"rot", 0}},
                [this](TLevel&, TTile& mT, const Vec2i& mP)
                {
                    f->createETurretStarPlasma(
                        mP, getDir8FromDeg(getP<float>(mT, "rot")));
                });
            add(OBLETType::LETTurretCP, a.txSmall, a.eTurret1, {{"rot", 0}},
                [this](TLevel&, TTile& mT, const Vec2i& mP)
                {
                    f->createETurretCannonPlasma(
                        mP, getDir8FromDeg(getP<float>(mT, "rot")));
                });
            add(OBLETType::LETTurretBP, a.txSmall, a.eTurret2, {{"rot", 0}},
                [this](TLevel&, TTile& mT, const Vec2i& mP)
                {
                    f->createETurretBulletPlasma(
                        mP, getDir8FromDeg(getP<float>(mT, "rot")));
                });
            add(OBLETType::LETTurretRL, a.txSmall, a.eTurret3, {{"rot", 0}},
                [this](TLevel&, TTile& mT, const Vec2i& mP)
                {
                    f->createETurretRocket(
                        mP, getDir8FromDeg(getP<float>(mT, "rot")));
                });
            add(OBLETType::LETPlayer, a.txSmall, a.p1Stand, {{"rot", 0}},
                [this](TLevel&, TTile&, const Vec2i& mP)
                {
                    f->createPlayer(mP);
                });
            add(OBLETType::LETRunner, a.txSmall, a.e1Stand, {{"rot", 0}},
                [this](TLevel&, TTile&, const Vec2i& mP)
                {
                    f->createERunner(mP, RunnerType::Unarmed);
                });
            add(OBLETType::LETRunnerArmed, a.txSmall, a.e1Shoot, {{"rot", 0}},
                [this](TLevel&, TTile&, const Vec2i& mP)
                {
                    f->createERunner(mP, RunnerType::PlasmaBolter);
                });
            add(OBLETType::LETCharger, a.txMedium, a.e2Stand, {{"rot", 0}},
                [this](TLevel&, TTile&, const Vec2i& mP)
                {
                    f->createECharger(mP, ChargerType::Unarmed);
                });
            add(OBLETType::LETChargerArmed, a.txMedium, a.e2Shoot, {{"rot", 0}},
                [this](TLevel&, TTile&, const Vec2i& mP)
                {
                    f->createECharger(mP, ChargerType::PlasmaBolter);
                });
            add(OBLETType::LETJuggernaut, a.txBig, a.e3Stand, {{"rot", 0}},
                [this](TLevel&, TTile&, const Vec2i& mP)
                {
                    f->createEJuggernaut(mP, JuggernautType::Unarmed);
                });
            add(OBLETType::LETJuggernautArmed, a.txBig, a.e3Shoot, {{"rot", 0}},
                [this](TLevel&, TTile&, const Vec2i& mP)
                {
                    f->createEJuggernaut(mP, JuggernautType::PlasmaBolter);
                });
            add(OBLETType::LETGiant, a.txGiant, a.e4Stand, {{"rot", 0}},
                [this](TLevel&, TTile&, const Vec2i& mP)
                {
                    f->createEGiant(mP);
                });
            add(OBLETType::LETBall, a.txSmall, a.eBall, {},
                [this](TLevel&, TTile&, const Vec2i& mP)
                {
                    f->createEBall(mP, BallType::Normal, false);
                });
            add(OBLETType::LETBallFlying, a.txSmall, a.eBallFlying, {},
                [this](TLevel&, TTile&, const Vec2i& mP)
                {
                    f->createEBall(mP, BallType::Flying, false);
                });
            add(OBLETType::LETEnforcer, a.txMedium, a.e5Stand, {{"rot", 0}},
                [this](TLevel&, TTile&, const Vec2i& mP)
                {
                    f->createEEnforcer(mP);
                });
            add(OBLETType::LETTrapdoor, a.txSmall, a.trapdoor, {},
                [this](TLevel&, TTile&, const Vec2i& mP)
                {
                    f->createTrapdoor(mP, false);
                });
            add(OBLETType::LETTrapdoorPOnly, a.txSmall, a.trapdoorPOnly, {},
                [this](TLevel&, TTile&, const Vec2i& mP)
                {
                    f->createTrapdoor(mP, true);
                });
            add(OBLETType::LETExplosiveCrate, a.txSmall, a.explosiveCrate,
                {{"id", -1}}, [this](TLevel&, TTile& mT, const Vec2i& mP)
                {
                    f->createFloor(mP, true);
                    f->createExplosiveCrate(mP, getP<int>(mT, "id"));
                });
            add(OBLETType::LETVMHealth, a.txSmall, a.vmHealth, {},
                [this](TLevel&, TTile&, const Vec2i& mP)
                {
                    f->createVMHealth(mP);
                });

            add(OBLETType::LETWall, a.txSmall, a.wallSingle, {},
                [this](TLevel& mL, TTile& mT, const Vec2i& mP)
                {
                    int mask{getWallMask(mL, OBLETType::LETWall, mT.getX(),
                        mT.getY(), mT.getZ())};
                    f->createWall(mP, *a.wallBitMask[mask]);
                });

            add(OBLETType::LETWallD, a.txSmall, a.wallDSingle, {},
                [this](TLevel& mL, TTile& mT, const Vec2i& mP)
                {
                    int mask{getWallMask(mL, OBLETType::LETWallD, mT.getX(),
                        mT.getY(), mT.getZ())};
                    f->createFloor(mP, true);
                    f->createWallDestructible(mP, *a.wallDBitMask[mask]);
                });

            add(OBLETType::LETDoor, a.txSmall, a.doorSingle,
                {{"id", 0}, {"open", false}},
                [this](TLevel& mL, TTile& mT, const Vec2i& mP)
                {
                    int mask{getWallMask(mL, OBLETType::LETDoor, mT.getX(),
                        mT.getY(), mT.getZ())};
                    f->createFloor(mP, true);
                    f->createDoor(mP, *a.doorBitMask[mask], getP<int>(mT, "id"),
                        getP<bool>(mT, "open"));
                });

            add(OBLETType::LETDoorG, a.txSmall, a.doorGSingle,
                {{"open", false}},
                [this](TLevel& mL, TTile& mT, const Vec2i& mP)
                {
                    int mask{getWallMask(mL, OBLETType::LETDoorG, mT.getX(),
                        mT.getY(), mT.getZ())};
                    f->createFloor(mP, true);
                    f->createDoorG(
                        mP, *a.doorGBitMask[mask], getP<bool>(mT, "open"));
                });

            add(OBLETType::LETDoorR, a.txSmall, a.doorRSingle,
                {{"open", false}},
                [this](TLevel& mL, TTile& mT, const Vec2i& mP)
                {
                    int mask{getWallMask(mL, OBLETType::LETDoorR, mT.getX(),
                        mT.getY(), mT.getZ())};
                    f->createFloor(mP, true);
                    f->createDoorR(
                        mP, *a.doorRBitMask[mask], getP<bool>(mT, "open"));
                });

            add(OBLETType::LETSpawner, a.txSmall, a.spawner,
                {{"id", -1}, {"enemyType", 0}, {"delayStart", 0.f},
                    {"delaySpawn", 200.f}, {"spawnCount", 1}},
                [this](TLevel&, TTile& mT, const Vec2i& mP)
                {
                    f->createSpawner(mP, getPE<SpawnerItem>(mT, "enemyType"),
                        getP<int>(mT, "id"), getP<float>(mT, "delayStart"),
                        getP<float>(mT, "delaySpawn"),
                        getP<int>(mT, "spawnCount"));
                },
                {{"enemyType", "SpawnerItem"}});

            add(OBLETType::LETPPlateSingle, a.txSmall, a.pPlateSingle,
                {{"id", 0}, {"action", 0}, {"playerOnly", false}},
                [this](TLevel&, TTile& mT, const Vec2i& mP)
                {
                    f->createPPlate(mP, getP<int>(mT, "id"), PPlateType::Single,
                        getPE<IdAction>(mT, "action"),
                        getP<bool>(mT, "playerOnly"));
                },
                {{"action", "IdAction"}});

            add(OBLETType::LETPPlateMulti, a.txSmall, a.pPlateMulti,
                {{"id", 0}, {"action", 0}, {"playerOnly", false}},
                [this](TLevel&, TTile& mT, const Vec2i& mP)
                {
                    f->createPPlate(mP, getP<int>(mT, "id"), PPlateType::Multi,
                        getPE<IdAction>(mT, "action"),
                        getP<bool>(mT, "playerOnly"));
                },
                {{"action", "IdAction"}});

            add(OBLETType::LETPPlateOnOff, a.txSmall, a.pPlateOnOff,
                {{"id", 0}, {"action", 0}, {"playerOnly", false}},
                [this](TLevel&, TTile& mT, const Vec2i& mP)
                {
                    f->createPPlate(mP, getP<int>(mT, "id"), PPlateType::OnOff,
                        getPE<IdAction>(mT, "action"),
                        getP<bool>(mT, "playerOnly"));
                },
                {{"action", "IdAction"}});

            add(OBLETType::LETForceField, a.txSmall, a.ff0,
                {{"id", 0}, {"rot", 0}, {"blockFriendly", true},
                    {"blockEnemy", true}, {"forceMult", 100.f}},
                [this](TLevel&, TTile& mT, const Vec2i& mP)
                {
                    f->createForceField(mP, getP<int>(mT, "id"),
                        getDir8FromDeg(getP<float>(mT, "rot")),
                        getP<bool>(mT, "blockFriendly"),
                        getP<bool>(mT, "blockEnemy"),
                        getP<float>(mT, "forceMult") / 100.f);
                });

            add(OBLETType::LETBulletForceField, a.txSmall, a.forceArrowMark,
                {{"id", 0}, {"rot", 0}, {"blockFriendly", true},
                    {"blockEnemy", true}},
                [this](TLevel&, TTile& mT, const Vec2i& mP)
                {
                    f->createBulletForceField(mP, getP<int>(mT, "id"),
                        getDir8FromDeg(getP<float>(mT, "rot")),
                        getP<bool>(mT, "blockFriendly"),
                        getP<bool>(mT, "blockEnemy"));
                });

            add(OBLETType::LETPjBooster, a.txSmall, a.fpj0,
                {{"id", 0}, {"rot", 0}, {"forceMult", 100.f}},
                [this](TLevel&, TTile& mT, const Vec2i& mP)
                {
                    f->createBooster(mP, getP<int>(mT, "id"),
                        getDir8FromDeg(getP<float>(mT, "rot")),
                        getP<float>(mT, "forceMult") / 100.f);
                });

            add(OBLETType::LETPjChanger, a.txSmall, a.bulletChanger,
                {{"id", 0}, {"rot", 0}},
                [this](TLevel&, TTile& mT, const Vec2i& mP)
                {
                    f->createBooster(mP, getP<int>(mT, "id"),
                        getDir8FromDeg(getP<float>(mT, "rot")), 0.f);
                });
        }

        template <typename T>
        inline void add(OBLETType mType, sf::Texture* mTexture,
            const sf::IntRect& mIntRect,
            const std::map<std::string, ssvj::Val>& mDefaultParams,
            const T& mSpawn,
            const std::initializer_list<std::pair<std::string, std::string>>&
                mEnumParams = {})
        {
            entries[mType] =
                Entry{mType, mTexture, mIntRect, mDefaultParams, mSpawn};
            for(auto& p : mEnumParams) entries[mType].enumParams.insert(p);
        }
        inline void spawn(TLevel& mLevel, TTile& mTile, const Vec2i& mPos)
        {
            if(mTile.getType() == OBLETType::LETNull) return;
            get(mTile.getType()).spawn(mLevel, mTile, mPos);
        }

        inline const Entry& get(OBLETType mType) const
        {
            return entries.at(mType);
        }
        inline int getSize() const noexcept { return entries.size(); }

        inline void setGame(TGame& mGame) noexcept
        {
            game = &mGame;
            f = &game->getFactory();
        }
    };

    // Template magic to temporarily avoid creating .cpp source files
    class OBGame;
    class OBFactory;
    class OBLELevel;
    class OBLETile;

    using OBLEDatabase =
        OBLEDatabaseImpl<OBGame, OBLELevel, OBLETile, OBFactory>;
    using OBLEDatabaseEntry = OBLEDatabase::Entry;
}

#endif
