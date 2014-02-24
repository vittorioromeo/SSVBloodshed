// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMMON
#define SSVOB_COMMON

#include <array>
#include <unordered_map>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Network.hpp>
#include <SSVUtils/SSVUtils.hpp>
#include <SSVUtilsJson/SSVUtilsJson.hpp>
#include <SSVStart/SSVStart.hpp>
#include <SSVEntitySystem/SSVEntitySystem.hpp>
#include <SSVMenuSystem/SSVMenuSystem.hpp>
#include <SSVSCollision/SSVSCollision.hpp>

#include "SSVBloodshed/SSVVM/OpCodes.hpp"

namespace ob
{
	// Enum helpers
	namespace Internal
	{
		inline std::map<std::string, std::vector<std::string>*>& getEnumsMap() noexcept { static std::map<std::string, std::vector<std::string>*> map; return map; }
		template<typename T> inline std::vector<std::string>& getEnumStrVec() noexcept;
		template<typename T> inline std::string getEnumStr(T mValue) noexcept { return getEnumStrVec<T>()[int(mValue)]; }
	}

	SSVU_FAT_ENUM_MGR(OBEnumMgr);

	#define OB_ENUM_MKSTR(mIdx, mData, mArg) SSVPP_TOSTR(mArg) SSVPP_COMMA_IF(mIdx)

	#define OB_ENUM_DEF(mName, ...) \
		SSVU_FAT_ENUM_DEF(OBEnumMgr, mName, int, __VA_ARGS__); \
		namespace Internal \
		{ \
			template<> inline std::vector<std::string>& getEnumStrVec<mName>() noexcept \
			{ \
				static std::vector<std::string> strings \
				{ \
					SSVPP_FOREACH(OB_ENUM_MKSTR, SSVPP_EMPTY(), __VA_ARGS__) \
				}; \
				return strings; \
			} \
			volatile static struct __initStruct ## mName \
			{ \
				inline __initStruct ## mName() \
				{ \
					Internal::getEnumsMap()[#mName] = &getEnumStrVec<mName>(); \
				} \
			} t ## mName; \
		} \
		SSVU_DEFINE_DUMMY_STRUCT(mName)

	inline std::vector<std::string>& getEnumStrVecByName(const std::string& mName) { return *Internal::getEnumsMap()[mName]; }

	// Typedefs
	template<typename T> using Vec2 = ssvs::Vec2<T>;
	template<typename T, typename TDeleter = std::default_delete<T>> using Uptr = ssvs::Uptr<T, TDeleter>;
	using Vec2i = ssvs::Vec2i;
	using Vec2f = ssvs::Vec2f;
	using Vec2u = ssvs::Vec2u;
	using Entity = sses::Entity;
	using World = ssvsc::World<ssvsc::HashGrid, ssvsc::Impulse>;
	using Body = World::BodyType;
	using Sensor = World::SensorType;
	using DetectionInfo = World::DetectionInfoType;
	using ResolutionInfo = World::ResolutionInfoType;
	using FT = ssvu::FT;
	using Trigger = ssvs::Input::Trigger;

	// Pixel <-> coords utils
	template<typename T> inline constexpr float toPixels(T mValue) noexcept		{ return mValue / 100; }
	template<typename T> inline constexpr int toCoords(T mValue) noexcept		{ return mValue * 100; }
	template<typename T> inline Vec2f toPixels(const Vec2<T>& mValue) noexcept	{ return {toPixels(mValue.x), toPixels(mValue.y)}; }
	template<typename T> inline Vec2i toCoords(const Vec2<T>& mValue) noexcept	{ return {toCoords(mValue.x), toCoords(mValue.y)}; }

	// Level values
	constexpr int tileSize{10};
	constexpr int levelCols{32};
	constexpr int levelRows{22};
	constexpr int levelWidthPx{levelCols * tileSize};
	constexpr int levelHeightPx{levelRows * tileSize};
	constexpr int levelWidthCoords{toCoords(levelWidthPx)};
	constexpr int levelHeightCoords{toCoords(levelHeightPx)};

	// Game enums
	enum OBGroup : unsigned int
	{
		GSolidGround,
		GSolidAir,
		GProjectile,
		GOrganic,
		GFloor,
		GFriendly,
		GFriendlyKillable,
		GEnemy,
		GEnemyKillable,
		GIdReceiver,
		GPPlate,
		GTrapdoor,
		GPlayer,
		GShard,
		GLevelBound,
		GPit,
		GKillable,
		GEnvDestructible,
		GFlying,
		GForceField,
		GBulletForceField,
		GBooster,
		GUsable
	};
	enum OBLayer : int
	{
		LPlayer,
		LEnemy,
		LProjectile,
		LShard,
		LWall,
		LPSTemp,
		LTrapdoor,
		LPit,
		LFloorGrate,
		LPSPerm,
		LFloor,
		LBackground
	};



	OB_ENUM_DEF(PPlateType,		Single, Multi, OnOff);
	SSVU_FAT_ENUM_VALS(OBEnumMgr, IdAction, int,		(Toggle, 0), (Open, 1), (Close, 2));

	OB_ENUM_DEF(SpawnerItem,		RunnerUA, RunnerPB, ChargerUA, ChargerPB, ChargerGL, JuggerUA, JuggerPB, JuggerRL, Giant, Enforcer, BallN, BallF);

	OB_ENUM_DEF(RunnerType,		Unarmed, PlasmaBolter);
	OB_ENUM_DEF(ChargerType,		Unarmed, PlasmaBolter, GrenadeLauncher);
	OB_ENUM_DEF(JuggernautType, 	Unarmed, PlasmaBolter, RocketLauncher);
	OB_ENUM_DEF(BallType,			Normal, Flying);

	// Level editor enums
	enum class OBLETType : int
	{
		LETNull				= -1,
		LETFloor			= 0,
		LETGrate			= 1,
		LETWall				= 2,
		LETWallD			= 3,
		LETPit				= 4,
		LETDoor				= 5,
		LETDoorG			= 6,
		LETDoorR			= 7,
		LETPPlateSingle		= 8,
		LETPPlateMulti		= 9,
		LETPPlateOnOff		= 10,
		LETTrapdoor			= 11,
		LETTrapdoorPOnly	= 12,
		LETExplosiveCrate	= 13,
		LETVMHealth			= 14,
		LETTurretSP			= 15,
		LETTurretCP			= 16,
		LETTurretBP			= 17,
		LETSpawner			= 18,
		LETPlayer			= 19,
		LETRunner			= 20,
		LETRunnerArmed		= 21,
		LETCharger			= 22,
		LETChargerArmed		= 23,
		LETJuggernaut		= 24,
		LETJuggernautArmed	= 25,
		LETEnforcer			= 26,
		LETGiant			= 27,
		LETBall				= 28,
		LETBallFlying		= 29,
		LETTurretRL			= 30,
		LETForceField		= 31,
		LETPjBooster		= 32,
		LETPjChanger		= 33,
		LETBulletForceField	= 34
	};

	// Direction utils
	constexpr float dir8StepDeg{45.f};
	constexpr float dir8StepRad{ssvu::toRad(dir8StepDeg)};
	enum Dir8 : int {E = 0, SE = 1, S = 2, SW = 3, W = 4, NW = 5, N = 6, NE = 7};
	template<typename T = float> constexpr inline T getDegFromDir8(Dir8 mDir) noexcept	{ return T(int(mDir) * dir8StepDeg); }
	template<typename T = float> constexpr inline T getRadFromDir8(Dir8 mDir) noexcept	{ return T(int(mDir) * dir8StepRad); }
	template<typename T> inline constexpr Dir8 getDir8FromDeg(T mDeg) noexcept			{ return Dir8(int(std::round(mDeg / dir8StepDeg)) % 8); }
	template<typename T> inline constexpr Dir8 getDir8FromRad(T mRad) noexcept			{ return Dir8(int(std::round(mRad / dir8StepRad)) % 8); }
	template<typename T> inline Dir8 getDir8FromXY(T mX, T mY) noexcept
	{
		if(mX < 0 && mY == 0)		return Dir8::W;
		else if(mX > 0 && mY == 0)	return Dir8::E;
		else if(mX == 0 && mY < 0)	return Dir8::N;
		else if(mX == 0 && mY > 0)	return Dir8::S;
		else if(mX < 0 && mY < 0)	return Dir8::NW;
		else if(mX < 0 && mY > 0)	return Dir8::SW;
		else if(mX > 0 && mY < 0)	return Dir8::NE;
		return Dir8::SE;
	}
	template<typename T = int> inline std::array<T, 2> getXYFromDir8(Dir8 mDir) noexcept
	{
		static std::array<std::array<T, 2>, 8> xys
		{{
			{{1, 0}},
			{{1, 1}},
			{{0, 1}},
			{{-1, 1}},
			{{-1, 0}},
			{{-1, -1}},
			{{0, -1}},
			{{1, -1}}
		}};

		return xys[int(mDir)];
	}
	template<typename T> inline Dir8 getDir8FromVec(const Vec2<T>& mVec) noexcept		{ return getDir8FromXY(mVec.x, mVec.y); }
	template<typename T = int> inline Vec2<T> getVecFromDir8(Dir8 mDir) noexcept		{ const auto& xy(getXYFromDir8<T>(mDir)); return {xy[0], xy[1]}; }
	template<typename T> inline T getSnappedDeg(const T& mDeg) noexcept					{ return getDegFromDir8(getDir8FromDeg(mDeg)); }
	template<typename T> inline Vec2<T> getSnappedVec(const Vec2<T>& mVec) noexcept		{ return Vec2<T>(getVecFromDir8(getDir8FromRad(ssvs::getRad(mVec)))); }

	// Timeline shortcuts
	inline void repeat(ssvu::Timeline& mTimeline, const ssvu::Action& mAction, unsigned int mTimes, FT mWait)
	{
		SSVU_ASSERT(mTimes > 0);
		auto& action(mTimeline.append<ssvu::Do>(mAction));
		mTimeline.append<ssvu::Wait>(mWait);
		mTimeline.append<ssvu::Goto>(action, mTimes - 1);
	}

	// Other utils
	inline Entity& getEntityFromBody(Body& mBody) { return *reinterpret_cast<Entity*>(mBody.getUserData()); }
	template<typename T> inline T& getComponentFromBody(Body& mBody){ return getEntityFromBody(mBody).getComponent<T>(); }
}


#endif
