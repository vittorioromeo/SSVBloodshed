#define TESTING

#ifdef TESTINGMPL

#include <tuple>
#include <SSVUtils/SSVUtils.hpp>

namespace ssvu
{
	namespace MPL
	{
		template<typename...> struct List;

		namespace Internal
		{
			struct NullType { };
		}

		using Null = typename Internal::NullType;
		template<bool TCond> using BoolResult = Conditional<TCond, std::true_type, std::false_type>;

		template<int TN1, int TN2> inline constexpr int getMin() noexcept		{ return TN1 < TN2 ? TN1 : TN2; }
		template<int TN1, int TN2> inline constexpr int getMax() noexcept		{ return TN1 > TN2 ? TN1 : TN2; }

		namespace Internal
		{
			// VA Args Head
			template<typename... TArgs> struct VAHeadHelper;
			template<typename T, typename... TArgs> struct VAHeadHelper<T, TArgs...>	{ using Type = T; };
			template<> struct VAHeadHelper<>											{ using Type = Null; };
			template<typename... TArgs> using VAHead = typename Internal::VAHeadHelper<TArgs...>::Type;

			// VA Args Tail
			template<typename... TArgs> struct VATailHelper;
			template<typename T, typename... TArgs> struct VATailHelper<T, TArgs...>	{ using Type = typename VATailHelper<TArgs...>::Type; };
			template<typename T> struct VATailHelper<T>									{ using Type = T; };
			template<> struct VATailHelper<>											{ using Type = Null; };
			template<typename... TArgs> using VATail = typename Internal::VATailHelper<TArgs...>::Type;

			// List::PopFront
			template<typename> struct PopFrontHelper;
			template<typename TL, typename... TLAs> struct PopFrontHelper<List<TL, TLAs...>>	{ using Type = List<TLAs...>; };
			template<> struct PopFrontHelper<List<>>											{ using Type = List<>; };

			// List::PopBack
			template<typename, typename> struct RemoveLastHelper;
			template<typename TLA1, typename TLA2, typename... TLAs1, typename... TLAs2> struct RemoveLastHelper<List<TLA1, TLA2, TLAs1...>, List<TLAs2...>>
			{
				using List1 = List<TLA1, TLA2, TLAs1...>;
				using List2 = List<TLAs2...>;
				using List1WithPop = typename PopFrontHelper<List1>::Type;
				using List2WithPush = typename List2::template PushBack<typename List1::Head>;

				using Type = typename RemoveLastHelper<List1WithPop, List2WithPush>::Type;
			};
			template<typename TL1, typename... TLAs2> struct RemoveLastHelper<List<TL1>, List<TLAs2...>> { using Type = List<TLAs2...>; };
			template<typename> struct PopBackHelper;
			template<typename... TLAs> struct PopBackHelper<List<TLAs...>> { using Type = typename RemoveLastHelper<List<TLAs...>, List<>>::Type; };
			template<> struct PopBackHelper<List<>> { using Type = List<>; };

			// List::SubList
			template<std::size_t, std::size_t, std::size_t, typename, typename, typename> struct SubListImpl;
			template<std::size_t, std::size_t, std::size_t, typename, typename> struct SubListDispatch;
			template<std::size_t TS1, std::size_t TS2, std::size_t TSC, typename... TLA1s, typename... TLA2s> struct SubListImpl<TS1, TS2, TSC, List<TLA1s...>, List<TLA2s...>, std::false_type>
			{
				using Type = typename SubListDispatch<TS1, TS2, TSC + 1, List<TLA1s...>, typename List<TLA2s...>::template PushBack<typename List<TLA1s...>::template Elem<TSC>>>::Type;
			};
			template<std::size_t TS1, std::size_t TS2, std::size_t TSC, typename... TLA1s, typename... TLA2s> struct SubListImpl<TS1, TS2, TSC, List<TLA1s...>, List<TLA2s...>, std::true_type>
			{
				using Type = List<TLA2s...>;
			};
			template<std::size_t TS1, std::size_t TS2, std::size_t TSC, typename TL1, typename TL2> struct SubListDispatch
			{
				using Type = typename SubListImpl<TS1, TS2, TSC, TL1, TL2, BoolResult<TSC == TS2 || TSC >= TL1::getSize()>>::Type;
			};
			template<std::size_t, std::size_t, std::size_t, typename> struct SubListHelper;
			template<std::size_t TS1, std::size_t TS2, std::size_t TSC, typename... TLAs> struct SubListHelper<TS1, TS2, TSC, List<TLAs...>>
			{
				using Type = typename SubListDispatch<TS1, TS2, TSC, List<TLAs...>, List<>>::Type;
			};
		}

		template<typename... TArgs> struct List
		{
			using Head = Internal::VAHead<TArgs...>;
			using Tail = Internal::VATail<TArgs...>;

			inline static constexpr std::size_t getSize() noexcept { return sizeof...(TArgs); }

			template<std::size_t TS> using Elem = std::tuple_element_t<TS, std::tuple<TArgs...>>;
			template<std::size_t TS> using ElemReverse = Elem<getSize() - 1 - TS>;

			template<typename T> using PushBack = List<TArgs..., T>;
			template<typename T> using PushFront = List<T, TArgs...>;

			using PopBack = typename Internal::PopBackHelper<List<TArgs...>>::Type;
			using PopFront = typename Internal::PopFrontHelper<List<TArgs...>>::Type;

			template<std::size_t TS1, std::size_t TS2> using SubList = typename Internal::SubListHelper<TS1, TS2, TS1, List<TArgs...>>::Type;

			using Clear = List<>;

			// TODO: insert
		};

		namespace Internal
		{
			namespace Tests
			{
				static_assert(getMin<15, 10>() == 10, "");
				static_assert(getMin<-15, 10>() == -15, "");

				static_assert(getMax<15, 10>() == 15, "");
				static_assert(getMax<-15, 10>() == 10, "");

				static_assert(isSame<Internal::VAHead<int, float, char>, int>(), "");
				static_assert(isSame<Internal::VATail<int, float, char>, char>(), "");

				static_assert(isSame<List<int, float, char>::Head, int>(), "");
				static_assert(isSame<List<int, float, char>::Tail, char>(), "");
				static_assert(isSame<List<>::Head, Null>(), "");
				static_assert(isSame<List<>::Tail, Null>(), "");

				static_assert(isSame<List<int, float, char>::Elem<0>, int>(), "");
				static_assert(isSame<List<int, float, char>::Elem<1>, float>(), "");
				static_assert(isSame<List<int, float, char>::Elem<2>, char>(), "");

				static_assert(isSame<List<int, float, char>::ElemReverse<0>, char>(), "");
				static_assert(isSame<List<int, float, char>::ElemReverse<1>, float>(), "");
				static_assert(isSame<List<int, float, char>::ElemReverse<2>, int>(), "");

				static_assert(isSame<List<>::PushBack<int>, List<int>>(), "");
				static_assert(isSame<List<int>::PushBack<float>, List<int, float>>(), "");
				static_assert(isSame<List<int, float>::PushBack<char>, List<int, float, char>>(), "");

				static_assert(isSame<List<>::PushFront<int>, List<int>>(), "");
				static_assert(isSame<List<int>::PushFront<float>, List<float, int>>(), "");
				static_assert(isSame<List<float, int>::PushFront<char>, List<char, float, int>>(), "");

				static_assert(isSame<List<int, float, char>::PopBack, List<int, float>>(), "");
				static_assert(isSame<List<int, float>::PopBack, List<int>>(), "");
				static_assert(isSame<List<int>::PopBack, List<>>(), "");
				static_assert(isSame<List<>::PopBack, List<>>(), "");

				static_assert(isSame<List<int, float, char>::PopFront, List<float, char>>(), "");
				static_assert(isSame<List<float, char>::PopFront, List<char>>(), "");
				static_assert(isSame<List<char>::PopFront, List<>>(), "");
				static_assert(isSame<List<>::PopFront, List<>>(), "");

				//						  0    1      2     3      4
				static_assert(isSame<List<int, float, char, float, int>::SubList<0, 0>, List<>>(), "");
				static_assert(isSame<List<int, float, char, float, int>::SubList<0, 1>, List<int>>(), "");
				static_assert(isSame<List<int, float, char, float, int>::SubList<0, 3>, List<int, float, char>>(), "");
				static_assert(isSame<List<int, float, char, float, int>::SubList<2, 4>, List<char, float>>(), "");
				static_assert(isSame<List<int, float, char, float, int>::SubList<0, 999>, List<int, float, char, float, int>>(), "");
			}
		}
	}
}


int main()
{
SSVU_TEST_RUN_ALL();
	return 0;

}

#endif

#ifdef TESTING

#include <map>
#include <unordered_map>
#include <SSVUtils/SSVUtils.hpp>

int main()
{
	SSVU_TEST_RUN_ALL();

	std::map<int, std::string> m{{1, "aa"}, {2, "bb"}, {3, "cc"}};
	std::unordered_map<int, std::string> um{{1, "aa"}, {2, "bb"}, {3, "cc"}};
	ssvu::Bimap<int, std::string> bm{{1, "aa"}, {2, "bb"}, {3, "cc"}};

	ssvu::lo() << std::vector<int>{1, 2, 3} << std::endl;
	ssvu::lo() << m << std::endl;
	ssvu::lo() << um << std::endl;
	ssvu::lo() << bm << std::endl;

	return 0;
}

#endif

#ifdef TESTING2

#include "SSVBloodshed/OBCommon.hpp"

/*
#define RECURSION_DISPATCHER_IMPL(mFunc, ...) SSVPP_TKNCAT_3(mFunc, _, SSVPP_EXPAND(SSVPP_BOOL(SSVPP_DECREMENT(SSVPP_ARGCOUNT(__VA_ARGS__)))))
#define RECURSION_DISPATCHER(mFunc, ...) RECURSION_DISPATCHER_IMPL(mFunc, __VA_ARGS__)

#define CALL_RECURSION_DISPATCHED_FUNC(mFunc, ...) RECURSION_DISPATCHER(mFunc, __VA_ARGS__) ( __VA_ARGS__ )

#define EXAMPLE_IMPL_0(mArg)	WELP
#define EXAMPLE_IMPL_1(mArg0, mArg1, ...) EXAMPLE_IMPL_0(mArg0) CALL_RECURSION_DISPATCHED_FUNC(EXAMPLE_IMPL, mArg1, __VA_ARGS__)

#define SSVPP_IMPL_VA_ARGS_0(...)
#define SSVPP_IMPL_VA_ARGS_1(...) __VA_ARGS__
#define SSVPP_VA_ARGS(...) SSVPP_EXPAND(SSVPP_TKNCAT_2(SSVPP_IMPL_VA_ARGS_, SSVPP_IMPL_HAS_ZERO_OR_ONE_ARGS(__VA_ARGS__))) ( __VA_ARGS__ )
*/
int main()
{
/*
	std::cout << SSVPP_TOSTR(CALL_RECURSION_DISPATCHED_FUNC(EXAMPLE_IMPL, 1, 2, 3)) << std::endl;
	std::cout << SSVPP_TOSTR(CALL_RECURSION_DISPATCHED_FUNC(EXAMPLE_IMPL, 1, 2)) << std::endl;
	std::cout << SSVPP_TOSTR(CALL_RECURSION_DISPATCHED_FUNC(EXAMPLE_IMPL, 1)) << std::endl;

	std::cout << SSVPP_TOSTR(SSVPP_IMPL_HAS_ZERO_OR_ONE_ARGS(1, 1, 1)) << std::endl;
	std::cout << SSVPP_TOSTR(SSVPP_IMPL_HAS_ZERO_OR_ONE_ARGS(1, 1)) << std::endl;
	std::cout << SSVPP_TOSTR(SSVPP_IMPL_HAS_ZERO_OR_ONE_ARGS(1)) << std::endl;
	std::cout << SSVPP_TOSTR(SSVPP_IMPL_HAS_ZERO_OR_ONE_ARGS()) << std::endl;

	std::cout << SSVPP_TOSTR_SEP(", ", SSVPP_VA_ARGS(1, 1, 1)) << std::endl;
	std::cout << SSVPP_TOSTR_SEP(", ", SSVPP_VA_ARGS(1, 1)) << std::endl;
	std::cout << SSVPP_TOSTR_SEP(", ", SSVPP_VA_ARGS(1)) << std::endl;
	std::cout << SSVPP_TOSTR(SSVPP_VA_ARGS()) << std::endl;
*/

	//std::cout << SSVPP_TOSTR(SSVPP_ARGCOUNT()) << std::endl;
	return 0;
}

#endif

#ifdef TESTCES

#include <chrono>
#include "SSVBloodshed/CESystem/CES.hpp"
#include "SSVBloodshed/OBCommon.hpp"

using namespace ssvces;
using FT = ssvu::FT;

struct CPosition : Component		{ float x, y; CPosition(float mX, float mY) : x{mX}, y{mY} { } };
struct CVelocity : Component		{ float x, y; CVelocity(float mX, float mY) : x{mX}, y{mY} { } };
struct CAcceleration : Component	{ float x, y; CAcceleration(float mX, float mY) : x{mX}, y{mY} { } };
struct CLife : Component			{ float life; CLife(float mLife) : life{mLife} { } };
struct CSprite : Component			{ sf::RectangleShape sprite; CSprite() : sprite{ssvs::Vec2f(1, 1)} { } };
struct CColorInhibitor : Component	{ float life; CColorInhibitor(float mLife) : life{mLife} { } };

struct SMovement : System<SMovement, Req<CPosition, CVelocity, CAcceleration>>
{
	inline void update(FT mFT) { processAll(mFT); }
	inline void process(Entity&, CPosition& cPosition, CVelocity& cVelocity, CAcceleration& cAcceleration, FT mFT)
	{
		cVelocity.x += cAcceleration.x * mFT;
		cVelocity.y += cAcceleration.y * mFT;
		cPosition.x += cVelocity.x * mFT;
		cPosition.y += cVelocity.y * mFT;
	}
};

struct SDeath : System<SDeath, Req<CLife>>
{
	inline void update(FT mFT) { processAll(mFT); }
	inline void process(Entity& entity, CLife& cLife, FT mFT)
	{
		cLife.life -= mFT;
		if(cLife.life < 0) entity.destroy();
	}
};

struct SNonColorInhibitor : System<SNonColorInhibitor, Req<CSprite>, Not<CColorInhibitor>>
{
	inline void draw() { processAll(); }
	inline void process(Entity&, CSprite& cSprite)
	{
		cSprite.sprite.setFillColor(sf::Color::Red);
	}
};

struct SDraw : System<SDraw, Req<CPosition, CSprite>>
{
	sf::RenderTarget& renderTarget;
	inline SDraw(sf::RenderTarget& mRenderTarget) : renderTarget(mRenderTarget) { }

	inline void draw() { processAll(); }
	inline void added(Entity&, CPosition&, CSprite&)
	{

	}
	inline void removed(Entity&, CPosition&, CSprite&)
	{

	}
	inline void process(Entity&, CPosition& cPosition, CSprite& cSprite)
	{
		cSprite.sprite.setPosition(cPosition.x, cPosition.y);
		renderTarget.draw(cSprite.sprite);
	}
};

struct SColorInhibitor : System<SColorInhibitor, Req<CSprite, CColorInhibitor>>
{
	inline void update(FT mFT) { processAll(mFT); }
	inline void draw() { processAll(); }

	inline void process(Entity& entity, CSprite&, CColorInhibitor& cColorInhibitor, FT mFT)
	{
		cColorInhibitor.life -= mFT;
		if(cColorInhibitor.life < 0) entity.removeComponent<CColorInhibitor>();
	}
	inline void process(Entity&, CSprite& cSprite, CColorInhibitor&)
	{
		cSprite.sprite.setFillColor(sf::Color::Blue);
	}

	inline void added(Entity&, CSprite& cSprite, CColorInhibitor&)
	{
		cSprite.sprite.scale(2.f, 2.f);
	}

	inline void removed(Entity&, CSprite& cSprite, CColorInhibitor&)
	{
		cSprite.sprite.scale(2.f, 2.f);
	}
};

int main()
{
	using namespace std;
	using namespace ssvu;
	using namespace sf;

	ssvs::GameWindow gameWindow;
	gameWindow.setTitle("component tests");
	gameWindow.setTimer<ssvs::TimerStatic>(0.5f, 0.5f);
	gameWindow.setSize(1024, 768);
	gameWindow.setFullscreen(false);
	gameWindow.setFPSLimited(true);
	gameWindow.setMaxFPS(200);

	Manager manager;
	SMovement sMovement;
	SDeath sDeath;
	SDraw sDraw{gameWindow};
	SColorInhibitor sColorInhibitor;
	SNonColorInhibitor sNonColorInhibitor;

	manager.registerSystem(sMovement);
	manager.registerSystem(sDeath);
	manager.registerSystem(sNonColorInhibitor);
	manager.registerSystem(sDraw);
	manager.registerSystem(sColorInhibitor);

	/*ssvu::startBenchmark();
	{
		for(int k = 0; k < 5; ++k)
		{
			for(int i = 0; i < 20000; ++i)
			{
				auto e = manager.createEntity();
				e.createComponent<CPosition>(ssvu::getRnd(512 - 100, 512 + 100), ssvu::getRnd(384 - 100, 384 + 100));
				e.createComponent<CVelocity>(ssvu::getRndR(-1.f, 1.f), ssvu::getRndR(-1.f, 1.f));
				e.createComponent<CAcceleration>(ssvu::getRndR(-1.f, 1.f), ssvu::getRndR(-1.f, 1.f));
				e.createComponent<CLife>(1);
			}

			manager.refresh();
			sMovement.update(5);
			sDeath.update(5);

			manager.refresh();
			sMovement.update(5);
			sDeath.update(5);

			manager.refresh();
			sMovement.update(5);
			sDeath.update(5);
		}

		for(int k = 0; k < 5; ++k)
		{
			for(int i = 0; i < 20000; ++i)
			{
				auto e = manager.createEntity();
				e.createComponent<CPosition>(ssvu::getRnd(512 - 100, 512 + 100), ssvu::getRnd(384 - 100, 384 + 100));
				e.createComponent<CVelocity>(ssvu::getRndR(-1.f, 1.f), ssvu::getRndR(-1.f, 1.f));
				e.createComponent<CAcceleration>(ssvu::getRndR(-1.f, 1.f), ssvu::getRndR(-1.f, 1.f));
				e.createComponent<CLife>(25);
			}

			manager.refresh();
			sMovement.update(5);
			sDeath.update(5);

			manager.refresh();
			sMovement.update(5);
			sDeath.update(5);

			manager.refresh();
			sMovement.update(5);
			sDeath.update(5);
		}
	} ssvu::lo("benchmark") << ssvu::endBenchmark();*/

	float counter{99};
	ssvs::GameState gameState;
	gameState.onUpdate += [&, counter](FT mFT) mutable
	{
		counter += mFT;
		if(counter > 100.f)
		{
			counter = 0;
			for(int i = 0; i < 20000; ++i)
			{
				auto e = manager.createEntity();
				e.createComponent<CPosition>(ssvu::getRnd(512 - 100, 512 + 100), ssvu::getRnd(384 - 100, 384 + 100));
				e.createComponent<CVelocity>(ssvu::getRndR(-1.f, 1.f), ssvu::getRndR(-1.f, 1.f));
				e.createComponent<CAcceleration>(ssvu::getRndR(-0.5f, 0.5f), ssvu::getRndR(-0.5f, 0.5f));
				e.createComponent<CSprite>();
				e.createComponent<CLife>(ssvu::getRnd(50, 100));
				e.createComponent<CColorInhibitor>(ssvu::getRnd(5, 85));
				e.addGroups(0);
			}
		}

		manager.refresh();
		sMovement.update(mFT);
		sDeath.update(mFT);
		sColorInhibitor.update(mFT);

		gameWindow.setTitle("up: " + toStr(gameWindow.getMsUpdate()) + "\t dw: " + toStr(gameWindow.getMsDraw()) + "\t ent: " + toStr(manager.getEntityCount()) + "\t cmp: " + toStr(manager.getComponentCount()));

		//if(gameWindow.getFPS() < 60) ssvu::lo()<<gameWindow.getFPS()<<std::endl;
		//ssvu::lo()<<manager.getEntityCount(0)<<std::endl;
	};
	gameState.onDraw += [&]
	{
		sNonColorInhibitor.draw();
		sDraw.draw();
		sColorInhibitor.draw();
	};


	gameWindow.setGameState(gameState);
	gameWindow.run();

	return 0;
}

#endif

#ifdef BLOODSHED

// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBAssets.hpp"
#include "SSVBloodshed/OBConfig.hpp"
#include "SSVBloodshed/OBGame.hpp"
#include "SSVBloodshed/Weapons/OBWpnTypes.hpp"
#include "SSVBloodshed/LevelEditor/OBLEEditor.hpp"
#include "SSVBloodshed/LevelEditor/OBLEDatabase.hpp"

using namespace ob;
using namespace std;
using namespace sf;
using namespace ssvu;
using namespace ssvu::FileSystem;
using namespace ssvs;
using namespace ssvms;


int main()
{
	SSVU_TEST_RUN_ALL();

	OBConfig::setSoundEnabled(false);

	unsigned int width{VideoMode::getDesktopMode().width}, height{VideoMode::getDesktopMode().height};
	width = 640; height = 480;

	OBAssets assets;

	GameWindow gameWindow;
	gameWindow.setTitle("operation bloodshed");
	gameWindow.setTimer<TimerStatic>(0.5f, 0.5f);
	gameWindow.setSize(width, height);
	gameWindow.setFullscreen(false);
	gameWindow.setFPSLimited(true);
	gameWindow.setMaxFPS(200);
	gameWindow.setPixelMult(1);

	OBGame game{gameWindow, assets};
	OBLEEditor editor{gameWindow, assets};
	OBLEDatabase database{assets};

	game.setEditor(editor);
	game.setDatabase(database);
	editor.setGame(game);
	editor.setDatabase(database);

	editor.newPack();

	gameWindow.setGameState(editor.getGameState());
	gameWindow.run();

	return 0;
}

#endif

#ifdef TOKENS

#include <array>
#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/SSVVM/SSVVM.hpp"

std::string source{
	R"(
	//!ssvasm

	$require_registers(4);

	$define(R0, 		0);
	$define(R1, 		1);
	$define(R2, 		2);
	$define(ROutput,	3);




	// _______________________________
	// 	FN_MAIN function
	//		* entrypoint
	//		* returns in ROutput
	// _______________________________

	$label(FN_MAIN);

		// Compute the 6th fibonacci number

		// Load constants
		loadIntCVToR(R0, 6);

		// Save registers
		pushRVToS(R0);
		pushRVToS(R1);

		// Push args
		pushRVToS(R0);

		// Call func
		callPI(FN_FIB);

		// Get return value
		moveRVToR(ROutput, R0);

		// Pop args
		popSV();

		// Restore registers
		popSVToR(R1);
		popSVToR(R0);



		// Push output to stack
		pushRVToS(ROutput);

		halt();




	// _______________________________
	// 	FN_FIB function
	//		* needs 1 int argument
	//		* uses R0, R1
	//		* returns in R0
	// _______________________________

	$label(FN_FIB);

		// Get arg from stack
		moveSBOVToR(R0, 2);

		// Check if arg is < 2 (put compare result in R1)
		compareIntRVIntCVToR(R1, R0, 2);

		// Return arg if arg < 2
		goToPIIfCompareRVSmaller(FN_FIB_RET_ARG, R1);




		// Else return fib(arg - 1) + fib(arg - 2)

		// Calculate fib(arg - 1)

			// Save registers
			pushRVToS(R0);

			// Push args
			pushIntCVToS(1);
			pushRVToS(R0);
			subtractInt2SVs();

			// Call func
			callPI(FN_FIB);

			// Get return value
			// Return value is in R0, move it to R2
			moveRVToR(R2, R0);

			// Pop args
			popSV();

			// Restore registers
			popSVToR(R0);

			// Push fib(arg - 1) on stack
			pushRVToS(R2);

		// Calculate fib(arg - 2)
			// Save registers
			pushRVToS(R0);

			// Push args
			pushIntCVToS(2);
			pushRVToS(R0);
			subtractInt2SVs();

			// Call func
			callPI(FN_FIB);

			// Get return value
			// Return value is in R0, move it to R2
			moveRVToR(R2, R0);

			// Pop args
			popSV();

			// Restore registers
			popSVToR(R0);

			// Push fib(arg - 2) on stack
			pushRVToS(R2);

		// Return fib(arg - 1) + fib(arg + 1)
			addInt2SVs();
			popSVToR(R0);
			returnPI();



		$label(FN_FIB_RET_ARG);
			returnPI();
	)"};



int main()
{
	SSVU_TEST_RUN_ALL();

	auto src(ssvvm::SourceVeeAsm::fromStringRaw(source));
	ssvvm::preprocessSourceRaw<true>(src);
	auto program(ssvvm::getAssembledProgram<true>(src));

	ssvvm::VirtualMachine vm;
	vm.setProgram(program);
	vm.run();

	return 0;
}

#endif

// TODO: generic macro recursion dispatcher
// TODO: static tokenizer macro generator with tokeninterface ctrp base class
