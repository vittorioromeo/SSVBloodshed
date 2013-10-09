#ifndef HGJSIOPHSJH

#include <stack>
#include <vector>
#include <tuple>
#include <numeric>
#include <cstdint>
#include <array>
#include <cassert>
#include <unordered_map>
#include <bitset>
#include <SSVUtils/SSVUtils.h>

using Id = std::size_t;
using Ctr = std::uint8_t;
using TypeId = std::size_t;
static constexpr std::size_t maxEntities{1000000};
static constexpr std::size_t maxComponentsPerEntity{64};
using Bitset = std::bitset<maxComponentsPerEntity>;

class Manager;
class EntityHandle;
class Component { };

namespace Internal
{
	// Last used bit index
	static unsigned int lastIdx{0};

	template<typename T> struct TypeIdStorage
	{
		// TypeIdStorage statically stores the TypeId and bit index of a Component type

		static_assert(std::is_base_of<Component, T>::value, "TypeIdStorage only works with types that derive from Component");

		static const TypeId typeId;
		static const std::size_t idx;
	};
	template<typename T> const TypeId TypeIdStorage<T>::typeId{typeid(T).hash_code()};
	template<typename T> const std::size_t TypeIdStorage<T>::idx{lastIdx++};
}

// Shortcut to get the static TypeId of a Component type from TypeIdStorage
template<typename T> inline constexpr const TypeId& getTypeId() { return Internal::TypeIdStorage<T>::typeId; }

// These functions use variadic template recursion to "build" a bitset for a set of Component types
template<typename T> inline static void buildBitsetHelper(Bitset& mBitset) noexcept { mBitset.set(Internal::TypeIdStorage<T>::idx); }
template<typename T1, typename T2, typename... TArgs> inline static void buildBitsetHelper(Bitset& mBitset) noexcept { buildBitsetHelper<T1>(mBitset); buildBitsetHelper<T2, TArgs...>(mBitset); }
template<typename... TArgs> inline static Bitset getBitset() noexcept { Bitset result; buildBitsetHelper<TArgs...>(result); return result; }

class IdPool
{
	// IdPool stores available Entity ids and is used to check Entity validity

	private:
		std::vector<Id> available;
		std::array<Ctr, maxEntities> counters;

	public:
		inline IdPool() : available(maxEntities)
		{
			// Fill the available ids vector from 0 to maxEntities
			std::iota(std::begin(available), std::end(available), 0);
			std::fill(std::begin(counters), std::end(counters), 0);
		}

		// Returns the first unused id
		inline Id getAvailableId() noexcept	{ Id result(available.back()); available.pop_back(); return result; }

		// Used on Entity death, reclaims the Entity's id so that it can be reused
		inline void reclaim(const Id& mId, const Ctr& mCtr) noexcept { if(mCtr != counters[mId]) return; ssvu::lo<<"reclaiming counter: "<<counters[mId]<<std::endl; ++counters[mId]; available.emplace_back(mId); ssvu::lo<<"reclaimed " <<mId<<" counter is now: " << counters[mId]; }

		// Checks if an Entity is currently alive
		inline bool isAlive(const Id& mId, const Ctr& mCtr) const noexcept { return counters[mId] == mCtr; }
};

class Entity
{
	friend class Manager;
	friend class EntityHandle;

	private:
		Manager& manager;
		std::unordered_map<TypeId, ssvu::Uptr<Component>> components;
		Bitset typeIdBitset;
		std::vector<std::size_t> systemIndices;
		bool mustDestroy{false};

	public:
		inline Entity(Manager& mManager) noexcept : manager(mManager) { }
		~Entity();

		template<typename T, typename... TArgs> inline void createComponent(TArgs&&... mArgs)
		{
			assert(!hasComponent<T>());
			components[getTypeId<T>()] = ssvu::make_unique<T>(std::forward<TArgs>(mArgs)...);
			buildBitsetHelper<T>(typeIdBitset);
		}
		template<typename T> inline bool hasComponent() const noexcept	{ return components.count(getTypeId<T>()) > 0; }
		template<typename T> inline T& getComponent()					{ assert(hasComponent<T>()); return *reinterpret_cast<T*>(components[getTypeId<T>()].get()); }

		inline void destroy() noexcept { mustDestroy = true; }

		inline Manager& getManager() noexcept					{ return manager; }
		inline const Bitset& getTypeIdBitset() const noexcept	{ return typeIdBitset; }
};

template<typename T> constexpr inline static std::tuple<T*> buildComponentsTuple(Entity& mEntity) { return std::tuple<T*>{&mEntity.getComponent<T>()}; }
template<typename T1, typename T2, typename... TArgs> constexpr inline static std::tuple<T1*, T2*, TArgs*...> buildComponentsTuple(Entity& mEntity) { return std::tuple_cat(buildComponentsTuple<T1>(mEntity), buildComponentsTuple<T2, TArgs...>(mEntity)); }

struct SystemBase
{
	private:
		Bitset typeIdBitset;

	protected:
		inline SystemBase(Bitset mTypeIdBitset) : typeIdBitset{std::move(mTypeIdBitset)} { }

	public:
		virtual void update() = 0;
		virtual void registerEntity(Entity&) = 0;
		virtual void unregisterEntity(Entity&) = 0;
		inline const Bitset& getTypeIdBitset() const noexcept { return typeIdBitset; }
};

template<typename... TArgs> class System : public SystemBase
{
	private:
		std::vector<std::tuple<Entity*, TArgs*...>> tuples;

	public:
		ssvu::Func<void(Entity*, TArgs*...)> onUpdate;

		inline System() noexcept : SystemBase{getBitset<TArgs...>()} { }

		inline void registerEntity(Entity& mEntity) override { tuples.push_back(std::tuple_cat(std::tuple<Entity*>{&mEntity}, buildComponentsTuple<TArgs...>(mEntity))); }
		inline void unregisterEntity(Entity& mEntity) override
		{
			for(auto itr(std::begin(tuples)); itr != std::end(tuples); ++itr) if(std::get<0>(*itr) == &mEntity) { tuples.erase(itr); return; }
		}
		inline void update() override { for(auto& t : tuples) ssvu::explode(onUpdate, t); }
};

class EntityHandle
{
	private:
		Manager& manager;
		IdPool& idPool;
		Entity& entity;
		Id id;
		Ctr counter;

	public:
		inline EntityHandle(Manager& mManager, IdPool& mIdPool, Entity& mEntity, const Id& mId) noexcept : manager(mManager), idPool(mIdPool), entity(mEntity), id{mId} { }

		template<typename T, typename... TArgs> inline void createComponent(TArgs&&... mArgs)	{ assert(isAlive()); entity.createComponent<T>(std::forward<TArgs>(mArgs)...); }
		template<typename T> inline bool hasComponent() const noexcept							{ assert(isAlive()); return entity.hasComponent<T>(); }
		template<typename T> inline T& getComponent()											{ assert(isAlive()); return entity.getComponent<T>(); }

		inline void destroy() noexcept
		{
			entity.destroy();
			idPool.reclaim(id, counter);

			ssvu::lo<<isAlive()<<std::endl;
			assert(!isAlive());
		}

		inline bool isAlive() const noexcept		{ return idPool.isAlive(id, counter); }
		inline Manager& getManager() noexcept		{ return manager; }
		inline Entity& getEntity() noexcept			{ return entity; }
		inline const Id& getId() const noexcept		{ return id; }
		inline const Ctr& getCtr() const noexcept	{ return counter; }
};

class Manager
{
	private:
		IdPool entityIdPool;
		std::vector<Entity*> entitiesToAdd;
		std::vector<ssvu::Uptr<SystemBase>> systems;
		std::vector<ssvu::Uptr<Entity>> entities;

	public:
		inline void delFromSystems(Entity& mEntity, const std::vector<std::size_t>& mIndices)
		{
			for(const auto& i : mIndices) systems[i]->unregisterEntity(mEntity);
		}

		inline void refresh()
		{
			ssvu::eraseRemoveIf(entities, [](const ssvu::Uptr<Entity>& mEntity){ return mEntity->mustDestroy; });

			for(auto& e : entitiesToAdd)
			{
				for(auto i(0u); i < systems.size(); ++i)
				{
					auto& s(systems[i]);
					if((e->getTypeIdBitset() & s->getTypeIdBitset()) == s->getTypeIdBitset()) s->registerEntity(*e);
					e->systemIndices.push_back(i);
				}
				entities.emplace_back(e);
			}

			entitiesToAdd.clear();
		}
		inline void update() { for(auto& s : systems) s->update(); }

		inline EntityHandle createEntity()
		{
			auto entity(new Entity{*this});
			entitiesToAdd.push_back(entity);
			return {*this, entityIdPool, *entity, entityIdPool.getAvailableId()};
		}

		template<typename T, typename... TArgs> void registerSystem(TArgs&&... mArgs)
		{
			systems.emplace_back(new T{std::forward<TArgs>(mArgs)...});
		}

		inline IdPool& getEntityIdPool() noexcept { return entityIdPool; }
};

inline Entity::~Entity() { manager.delFromSystems(*this, systemIndices); }

// ---

#define SYSTEM_UPDATE(system, body) inline system() { onUpdate = []body ;}

struct CPosition : Component		{ int x, y; CPosition(int mX, int mY) : x{mX}, y{mY} { } };
struct CVelocity : Component		{ int x, y; CVelocity(int mX, int mY) : x{mX}, y{mY} { } };
struct CAcceleration : Component	{ int x, y; CAcceleration(int mX, int mY) : x{mX}, y{mY} { } };

struct SMovement : System<CPosition, CVelocity>
{
	SYSTEM_UPDATE(SMovement, (Entity* mEntity, CPosition* mCPosition, CVelocity* mCVelocity)
	{
		mCPosition->x += mCVelocity->x;
		mCPosition->y += mCVelocity->y;
		ssvu::lo << "sm:" << mCPosition->x << " ; " << mCPosition->y << std::endl;
	})
};

struct SMovementAdv : System<CVelocity, CAcceleration>
{
	inline SMovementAdv()
	{
		onUpdate = [](Entity* mEntity, CVelocity* mCVelocity, CAcceleration* mCAcceleration)
		{
			mCVelocity->x += mCAcceleration->x;
			mCVelocity->y += mCAcceleration->y;
			ssvu::lo << "smadv:" << mCVelocity->x << " ; " << mCVelocity->y << std::endl;
		};
	}
};


int main()
{
	using namespace std;
	using namespace ssvu;


	Manager manager;
	manager.registerSystem<SMovement>();
	manager.registerSystem<SMovementAdv>();


	auto e = manager.createEntity();
	e.createComponent<CPosition>(0, 0);
	e.createComponent<CVelocity>(10, 5);
	e.createComponent<CAcceleration>(-3, 5);


	{
		auto e = manager.createEntity();
		e.createComponent<CPosition>(0, 0);
		e.createComponent<CAcceleration>(-3, 5);
	}

	manager.refresh();
	manager.update();
	manager.update();
	manager.update();

	e.destroy();
	e.destroy();e.destroy();e.destroy();e.destroy();e.destroy();e.destroy();e.destroy();

	manager.refresh();
	manager.update();
	manager.update();
	manager.update();

	/*TestSystem ts;
	Manager m;
	auto handle = m.createEntity();
	handle.createComponent<TestComponent>();*/

	return 0;
}

#else

// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBAssets.h"
#include "SSVBloodshed/OBConfig.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Weapons/OBWpnTypes.h"
#include "SSVBloodshed/LevelEditor/OBLEEditor.h"

using namespace ob;
using namespace std;
using namespace sf;
using namespace ssvu;
using namespace ssvu::FileSystem;
using namespace ssvs;
using namespace ssvms;

int main()
{
	OBConfig::setSoundEnabled(false);

	unsigned int width{VideoMode::getDesktopMode().width}, height{VideoMode::getDesktopMode().height};
	width = 640; height = 480;

	OBAssets assets;

	GameWindow gameWindow;
	gameWindow.setTitle("operation bloodshed");
	gameWindow.setTimer<StaticTimer>(0.5f, 0.5f);
	gameWindow.setSize(width, height);
	gameWindow.setFullscreen(false);
	gameWindow.setFPSLimited(true);
	gameWindow.setMaxFPS(200);

	OBGame game{gameWindow, assets};
	OBLEEditor editor{gameWindow, assets};

	game.setEditor(editor);
	editor.setGame(game);

	gameWindow.setGameState(editor.getGameState());
	gameWindow.run();

	return 0;
}

#endif
