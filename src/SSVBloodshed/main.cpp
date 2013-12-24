#ifdef HGJSIOPHSJH

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

#elseif XXX

// Copyright (c) 2013 Vittorio Romeo
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

#else

#include <array>
#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/SSVVM/SSVVM.hpp"



/*
int main()
{
	using IT = ssvvm::OpCode;
	ssvvm::VirtualMachine vm;

	// 2.f * 15

	vm.program += {IT::loadFloatCVToR, 0, 2.f};
	vm.program += {IT::loadFloatCVToR, 2, 12.f};
	vm.program += {IT::loadIntCVToR, 1, 14};

	vm.program += {IT::pushRVToS, 0};
	vm.program += {IT::pushRVToS, 2};

	vm.program += {IT::MultiplyFloat2SVs};
	vm.program += {IT::popSVToR, 0};


	vm.program += {IT::pushRVToS, 0};
	vm.program += {IT::pushRVToS, 2};

	vm.program += {IT::addFloat2SVs};
	vm.program += {IT::popSVToR, 0};

	vm.program += {IT::decrementIntRV, 1};
	vm.program += {IT::goToPIIfIntRV, 3, 1};


	vm.run();

	ssvu::lo() << vm.registry.getValue(0).get<float>() << "\n";

	return 0;
}
*/


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

		// Compute the 10th fibonacci number

		// Load constants
		loadIntCVToR(R0, 10);

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

template<typename TNode> class Graph
{
	public:
		class Node
		{
			private:
				std::vector<TNode*> connections;

			public:
				inline void connectToSelf() { connections.push_back(reinterpret_cast<TNode*>(this)); }
				inline void connectTo(TNode& mVertex)
				{
					assert(this != &mVertex);
					connections.push_back(&mVertex);
				}

				inline const decltype(connections)& getConnections() const { return connections; }
				inline bool isIsolated() const noexcept { return connections.empty(); }
		};


	private:
		std::vector<ssvu::Uptr<TNode>> vertices;

	protected:
		template<typename... TArgs> inline TNode& createNode(TArgs&&... mArgs)
		{
			static_assert(ssvu::isBaseOf<Node, TNode>(), "TNode must be derived from Graph<TNode>::Vertex");

			auto result(new TNode(std::forward<TArgs>(mArgs)...));
			vertices.emplace_back(result);
			return *result;
		}

	public:
};

struct FSMState;

using FSMType = Graph<FSMState>;
using FSMRule = ssvu::Func<bool()>;

struct FSMState : public FSMType::Node
{
	private:
		FSMRule rule;
		bool terminal;

	public:
		inline FSMState(const FSMRule& mRule, bool mTerminal = true) : rule{mRule}, terminal{mTerminal} { }

		inline FSMState* getFirstMatchingConnection() const noexcept
		{
			for(const auto& c : getConnections()) if(c->matchesRule()) return c;
			return nullptr;
		}

		inline bool isTerminal() const noexcept		{ return terminal; }
		inline bool matchesRule() const noexcept	{ return rule(); }

};

class FSM : public FSMType
{
	private:
		FSMState* startState{nullptr};
		FSMState* currentState{nullptr};
		std::vector<FSMState*> history;

	public:
		template<typename... TArgs> inline FSM& startOnce(TArgs&&... mArgs)
		{
			auto& state(createNode(std::forward<TArgs>(mArgs)...));
			startState = currentState = &state;
			history.push_back(&state);
			return *this;
		}
		template<typename... TArgs> inline FSM& startRepeat(TArgs&&... mArgs)
		{
			auto& state(createNode(std::forward<TArgs>(mArgs)...));
			state.connectToSelf();
			startState = currentState = &state;
			history.push_back(&state);
			return *this;
		}

		template<typename... TArgs> inline FSM& continueOnce(TArgs&&... mArgs)
		{
			auto& state(createNode(std::forward<TArgs>(mArgs)...));
			history.back()->connectTo(state);
			history.push_back(&state);
			return *this;
		}
		template<typename... TArgs> inline FSM& continueRepeat(TArgs&&... mArgs)
		{
			auto& state(createNode(std::forward<TArgs>(mArgs)...));
			state.connectToSelf();
			history.back()->connectTo(state);
			history.push_back(&state);
			return *this;
		}

		inline FSM& connectWithPrevious()
		{
			history.back()->connectTo(**(std::end(history) - 2));
			return *this;
		}

		inline FSM& continueRepeatUntilOnce(const FSMRule& mLoop, const FSMRule& mEnd)
		{
			auto& stateEnd(createNode(mEnd));

			auto& stateLoop(createNode(mLoop));
			stateLoop.connectTo(stateEnd);
			stateLoop.connectToSelf();

			history.back()->connectTo(stateEnd);
			history.back()->connectTo(stateLoop);

			return *this;
		}


		inline void reset() noexcept { currentState = startState; }
		inline void setCurrentState(FSMState& mState) noexcept	{ currentState = &mState; }
		inline const FSMState& getCurrentState() const noexcept	{ assert(currentState != nullptr); return *currentState; }
};

template<typename TTokenType, typename TTokenAttribute> class LexicalAnalyzer
{
	public:
		struct Token
		{
			TTokenType type;
			TTokenAttribute attribute;
			std::string contents;

			inline Token(TTokenType mType, std::string mContents) : type{mType}, contents{std::move(mContents)} { }
		};

	private:
		std::map<TTokenType, FSM*> matches;
		std::vector<Token> tokens;
		std::string source;
		std::size_t markerBegin, markerEnd, nextEnd;

		inline void advance() noexcept { markerEnd = nextEnd; }
		inline void consume(TTokenType mType)
		{
			const auto& tokenContents(source.substr(markerBegin, markerEnd - markerBegin));
			//ssvu::lo() << tokenContents << std::endl;

			tokens.emplace_back(mType, tokenContents);
			markerBegin = markerEnd;
		}

	public:
		inline void setSource(std::string mSource)				{ source = std::move(mSource); }
		inline void addMatch(TTokenType mTokenType, FSM& mFSM)	{ matches[mTokenType] = &mFSM; }

		inline char getMatchChar() { nextEnd = markerEnd + 1; return source[markerEnd]; }
		inline bool match(const std::string& mMatch, bool mConsume = true)
		{
			for(auto i(0u); i < mMatch.size(); ++i)
			{
				auto idxToCheck(markerEnd + i);
				if(idxToCheck >= source.size() || source[idxToCheck] != mMatch[i]) return false;
			}

			nextEnd = markerEnd + mMatch.size();
			return true;
		}
		inline bool matchAnything() { nextEnd = markerEnd + 1; return true; }

		inline void tokenize()
		{
			tokens.clear();
			markerBegin = markerEnd = nextEnd = 0;

			while(markerEnd < source.size())
			{
				bool canConsume{false};
				TTokenType foundType;

				for(const auto& p : matches)
				{
					const auto& tokenType(p.first);
					auto& fsm(*p.second);

					fsm.reset();

					markerEnd = markerBegin;

					if(fsm.getCurrentState().matchesRule())
					{
					//	ssvu::lo() << "FT: " << source.substr(markerBegin, nextEnd - markerBegin) << std::endl;

						advance();

						while(true)
						{
							//ssvu::lo() << source.substr(markerBegin, nextEnd - markerBegin) << std::endl;

							if(fsm.getCurrentState().isTerminal())
							{
								canConsume = true;
								foundType = tokenType;
							}
							else
							{
								//ssvu::lo() << "NONTERMINAL" << std::endl;
							}

							auto next(fsm.getCurrentState().getFirstMatchingConnection());
							if(next == nullptr) break;

							advance();
							fsm.setCurrentState(*next);
						}

						if(canConsume)
						{
							//ssvu::lo() << "CONSUMED" << std::endl;
							consume(foundType);
							break;
						}
					}
				}

				if(canConsume) continue;

				ssvu::lo() << "didn't find any match" << std::endl;
				ssvu::lo() << source.substr(markerBegin, nextEnd - markerBegin) << std::endl;
				throw;
			}
		}

		inline const decltype(tokens)& getTokens() const noexcept { return tokens; }
};

template<bool TStart> inline bool isIdnfChar(char mChar)
{
	static constexpr char validChars[]{'_'};
	return (TStart && std::isalpha(mChar)) || (!TStart && std::isalnum(mChar)) || ssvu::contains(validChars, mChar);
}

template<typename T, bool TDebug = true> ssvvm::Program makeProgram(T mTokens);

enum class TokenType : int
{
	Anything = -1,

	PreprocessorStart = 0,	// '$'
	Semicolon,				// ';'
	Comma,					// ','
	ParenthesisRoundOpen,	// '('
	ParenthesisRoundClose,	// ')'
	Float,					// "1234.f"
	Integer,				// "1234"
	Identifier,				// "hello1234_test"
	Comment,				// "// fgjisofg"
	WhiteSpace				// "  \n\t "
};

struct TokenAttribute { bool toDel{false}; };

using VAToken = LexicalAnalyzer<TokenType, TokenAttribute>::Token;

int main()
{
	LexicalAnalyzer<TokenType, TokenAttribute> la;

	FSM fsmPreprocessorStart;
	FSM fsmSemicolon;
	FSM fsmComma;
	FSM fsmParenthesisRoundOpen;
	FSM fsmParenthesisRoundClose;
	FSM fsmFloat;
	FSM fsmInteger;
	FSM fsmIdentifier;
	FSM fsmComment;
	FSM fsmWhiteSpace;

	fsmPreprocessorStart.startOnce([&la]{ return la.match("$"); });
	fsmSemicolon.startOnce([&la]{ return la.match(";"); });
	fsmComma.startOnce([&la]{ return la.match(","); });
	fsmParenthesisRoundOpen.startOnce([&la]{ return la.match("("); });
	fsmParenthesisRoundClose.startOnce([&la]{ return la.match(")"); });
	fsmFloat.startRepeat([&la]{ return std::isdigit(la.getMatchChar()); }, false).continueOnce([&la]{ return la.match(".f"); });
	fsmInteger.startRepeat([&la]{ return std::isdigit(la.getMatchChar()); });
	fsmIdentifier.startRepeat([&la]{ return isIdnfChar<true>(la.getMatchChar()); }).continueRepeat([&la]{ return isIdnfChar<false>(la.getMatchChar()); });
	fsmComment.startOnce([&la]{ return la.match("//"); }).continueRepeatUntilOnce([&la]{ return la.matchAnything(); }, [&la]{ return la.match("\n", false); });
	fsmWhiteSpace.startRepeat([&la]{ return std::isspace(la.getMatchChar()); });

	la.addMatch(TokenType::PreprocessorStart,		fsmPreprocessorStart);
	la.addMatch(TokenType::Semicolon,				fsmSemicolon);
	la.addMatch(TokenType::Comma,					fsmComma);
	la.addMatch(TokenType::ParenthesisRoundOpen,	fsmParenthesisRoundOpen);
	la.addMatch(TokenType::ParenthesisRoundClose,	fsmParenthesisRoundClose);
	la.addMatch(TokenType::Float,					fsmFloat);
	la.addMatch(TokenType::Integer,					fsmInteger);
	la.addMatch(TokenType::Identifier,				fsmIdentifier);
	la.addMatch(TokenType::Comment,					fsmComment);
	la.addMatch(TokenType::WhiteSpace,				fsmWhiteSpace);

	la.setSource(source);
	la.tokenize();

	makeProgram(la.getTokens());

	return 0;
}

/*
namespace Internal
{
	template<typename T> struct ExpectHelperDispatcher;
	template<> struct ExpectHelperDispatcher<TokenType>
	{
		template<typename TTokens> inline static bool check(TTokens& mTokens, std::size_t mIdx, TokenType mType) noexcept
		{
			return mTokens[mIdx].type == mType;
		}
	};
	template<> struct ExpectHelperDispatcher<const std::string&>
	{
		template<typename TTokens> inline static bool check(TTokens& mTokens, std::size_t mIdx, const std::string& mContents) noexcept
		{
			return mTokens[mIdx].contents == mContents;
		}
	};

	template<typename TTokens, typename TArg> inline bool expectHelper(TTokens& mTokens, std::size_t mIdx, TArg mArg) { return ExpectHelperDispatcher<TArg>::check(mTokens, mIdx, mArg); }
	template<typename TTokens, typename TArg, typename... TArgs> inline bool expectHelper(TTokens& mTokens, std::size_t mIdx, TArg mArg, TArgs... mArgs)
	{
		if(!expectHelper<TTokens, TArg>(mTokens, mIdx, mArg) || expectHelper<TTokens, TArgs...>(mTokens, mIdx + 1, mArgs...)) return false;
		return true;
	}
}

template<typename TTokens, typename... TArgs> inline bool expect(TTokens& mTokens, std::size_t mIdx, TArgs... mArgs)
{
	std::size_t ub{mIdx + sizeof...(TArgs)}, typeIdx{0u};
	if(ub >= mTokens.size()) return false;

	return Internal::expectHelper<TTokens, TArgs...>(mTokens, mIdx, mArgs...);
}
*/

template<typename T, bool TDebug> inline ssvvm::Program makeProgram(T mTokens)
{
	using namespace ssvvm;
	Program result;

	// Helper functions
	auto getTokenAsInt =	[](const VAToken& mT) -> int { return std::stoi(mT.contents.c_str()); };
	auto getTokenAsFloat =	[](const VAToken& mT) -> float { return std::stof(mT.contents.substr(0, mT.contents.size() - 2).c_str()); };
	auto getTokenContents =	[](const VAToken& mT) -> const std::string& { return mT.contents; };
	auto matchTypes = [&mTokens](std::size_t mIdx, const std::vector<TokenType> mTypes) -> bool
	{
		std::size_t ub{mIdx + mTypes.size()}, typeIdx{0u};
		if(ub >= mTokens.size()) return false;

		for(auto i(mIdx); i < ub; ++i)
		{
			const auto& type(mTypes[typeIdx++]);
			if(type != TokenType::Anything && type != mTokens[i].type) return false;
		}
		return true;
	};



	// Phase 0: discard Comment/WhiteSpace tokens
	ssvu::eraseRemoveIf(mTokens, [](const VAToken& mT){ return mT.type == TokenType::WhiteSpace || mT.type == TokenType::Comment; });



	// Phase 1: `$require_registers` directive
	int requireRegisters{-1};

	for(auto i(0u); i < mTokens.size(); ++i)
		if(matchTypes(i, {TokenType::PreprocessorStart, TokenType::Identifier, TokenType::ParenthesisRoundOpen, TokenType::Integer, TokenType::ParenthesisRoundClose, TokenType::Semicolon}))
			if(mTokens[i + 1].contents == "require_registers")
			{
				requireRegisters = getTokenAsInt(mTokens[i + 3]);
				for(auto k(0u); k < 6; ++k) mTokens[i + k].attribute.toDel = true;
				if(TDebug) ssvu::lo("makeProgram - phase 1") << "Found `$require_registers` = " << requireRegisters << "\n";
				break;
			}

	ssvu::eraseRemoveIf(mTokens, [](const VAToken& mT){ return mT.attribute.toDel; });



	// Phase 2: `$define` directives
	std::map<std::string, std::string> defines;
	for(auto i(0u); i < mTokens.size(); ++i)
		if(matchTypes(i, {TokenType::PreprocessorStart, TokenType::Identifier, TokenType::ParenthesisRoundOpen, TokenType::Identifier, TokenType::Comma, TokenType::Anything, TokenType::ParenthesisRoundClose, TokenType::Semicolon}))
		{
			const auto& alias(getTokenContents(mTokens[i + 3]));
			const auto& replacement(getTokenContents(mTokens[i + 5]));

			if(TDebug) ssvu::lo("makeProgram - phase 2") << "Found `$define`: " << alias << " -> " << replacement << "\n";

			if(defines.count(alias) > 0)
			{
				if(TDebug) ssvu::lo("makeProgram - phase 2") << "ERROR: alias `" << alias << "` already previously defined" << "\n";
				throw;
			}

			defines[alias] = replacement;
			for(auto k(0u); k < 8; ++k) mTokens[i + k].attribute.toDel = true;
		}

	ssvu::eraseRemoveIf(mTokens, [](const VAToken& mT){ return mT.attribute.toDel; });

	if(TDebug) ssvu::lo("makeProgram - phase 2") << "Applying `$define` directives..." << "\n";
	for(auto& t : mTokens) if(defines.count(t.contents) > 0) t.contents = defines[t.contents];
	if(TDebug) ssvu::lo("makeProgram - phase 2") << "Done" << "\n";



	// Phase 3: `$label` directives



	// Phase 4: separating instructions using semicolons
	struct Instruction { std::string identifier; std::vector<Value> args; };
	std::vector<Instruction> instructions;

	std::size_t idx{0u};
	while(idx < mTokens.size())
	{
		Instruction currentInstruction;

		if(mTokens[idx].type != TokenType::Identifier)
		{
			if(TDebug) ssvu::lo("makeProgram - phase 3") << "ERROR: expected `" << getTokenContents(mTokens[idx]) << "` to be an identifier" << "\n";
			throw;
		}

		currentInstruction.identifier = getTokenContents(mTokens[idx]);

		++idx;

		if(mTokens[idx].type != TokenType::ParenthesisRoundOpen)
		{
			if(TDebug) ssvu::lo("makeProgram - phase 3") << "ERROR: expected `" << getTokenContents(mTokens[idx]) << "` to be an open round parenthesis" << "\n";
			throw;
		}

		++idx;

		if(mTokens[idx].type == TokenType::ParenthesisRoundClose)
		{
			++idx;

			if(mTokens[idx].type != TokenType::Semicolon)
			{
				if(TDebug) ssvu::lo("makeProgram - phase 3") << "ERROR: expected `" << getTokenContents(mTokens[idx]) << "` to be a semicolon" << "\n";
				throw;
			}
		}
		else
		{
			while(mTokens[idx].type != TokenType::Semicolon)
			{
				if(mTokens[idx].type == TokenType::Float)			currentInstruction.args.push_back(Value::create<float>(getTokenAsFloat(mTokens[idx])));
				else if(mTokens[idx].type == TokenType::Integer)	currentInstruction.args.push_back(Value::create<int>(getTokenAsInt(mTokens[idx])));
				else
				{
					if(TDebug) ssvu::lo("makeProgram - phase 3") << "ERROR: expected `" << getTokenContents(mTokens[idx]) << "` to be a float or an integer" << "\n";
					throw;
				}

				++idx;

				if(mTokens[idx].type != TokenType::Comma)
				{
					if(TDebug) ssvu::lo("makeProgram - phase 3") << "ERROR: expected `" << getTokenContents(mTokens[idx]) << "` to be a comma" << "\n";
					throw;
				}

				++idx;
			}
		}

		instructions.push_back(currentInstruction);
		++idx;
	}


	//for(auto& t : mTokens) ssvu::lo() << t.contents;

	for(auto& i : instructions) ssvu::lo("Instruction") << i.identifier << ": " << i.args << "\n";

	ssvu::lo().flush();
	return result;
}


			/*
template<typename T> inline void addSrcInstr(T& mTarget, ssvvm::OpCode mOpCode,
											 ssvvm::ValueType mArgType0 = ssvvm::ValueType::Void,
											 ssvvm::ValueType mArgType1 = ssvvm::ValueType::Void,
											 ssvvm::ValueType mArgType2 = ssvvm::ValueType::Void)
{
	mTarget[getOpCodeStr(mOpCode)] = {mOpCode, mArgType0, mArgType1, mArgType2};
}

ssvvm::Program makeProgram(std::string mSource)
{
	using namespace ssvvm;

	struct Tkn
	{
		std::string str;
		bool toDel{false};

		Tkn(std::string mStr) : str(std::move(mStr)) { }
	};

	auto expectTkns = [](std::vector<Tkn>& mTkns, std::size_t mIdxStart, const std::vector<std::string>& mExpect) -> bool
	{
		if(mIdxStart + mExpect.size() >= mTkns.size()) return false;
		std::size_t ei{0u};
		for(auto i(mIdxStart); i < mIdxStart + mExpect.size(); ++i)
		{
			if(mExpect[ei] != "?" && mTkns[i].str != mExpect[ei]) return false;
			++ei;
		}
		return true;
	};

	auto delTkns = [](std::vector<Tkn>& mTkns, std::size_t mIdxStart, const std::vector<std::string>& mExpect)
	{
		if(mIdxStart + mExpect.size() >= mTkns.size()) return;
		std::size_t ei{0u};
		for(auto i(mIdxStart); i < mIdxStart + mExpect.size(); ++i)
		{
			if(mExpect[ei] == "?" || mTkns[i].str == mExpect[ei]) mTkns[i].toDel = true;
			++ei;
		}
	};

	std::vector<std::string> lineByLine;
	ssvu::split(lineByLine, mSource, "\n");

	// Get rid of comments
	for(auto& l : lineByLine)
	{
		std::size_t commentPos;
		while((commentPos = l.find("//")) != std::string::npos) l.erase(commentPos, l.size() - commentPos);
	}
	mSource = ""; for(auto& s : lineByLine) mSource += s;

	// Tokenize
	std::vector<std::string> splits{"\n", "\t", " "}, splitsKeep{"(", ")", ";", ","};
	std::vector<Tkn> tokens;
	for(const auto& x : ssvu::getSplit<ssvu::Split::Normal>(mSource, splits)) tokens.emplace_back(x);
	mSource = ""; for(auto& s : tokens) mSource += s.str; tokens.clear();
	for(const auto& x : ssvu::getSplit<ssvu::Split::TokenizeSeparator>(mSource, splitsKeep)) tokens.emplace_back(x);

	//for(auto& s : tokens) ssvu::lo()<<s.c_str()<<std::endl;

	// Preprocess stage 1: $require directives
	int requireRegisters{-1};
	for(auto i(0u); i < tokens.size(); ++i)
	{
		if(expectTkns(tokens, i, {"$require_registers", "(", "?", ")", ";"}))
		{
			requireRegisters = std::atoi(tokens[i + 2].str.c_str());
			delTkns(tokens, i, {"$require_registers", "(", "?", ")", ";"});
		}
	}

	ssvu::eraseRemoveIf(tokens, [](const Tkn& mTkn){ return mTkn.toDel; });

	// Preprocess stage2: $define directives
	std::map<std::string, std::string> defines;
	//for(auto& s : tokens) ssvu::lo()<<s.str.c_str()<<std::endl;
	for(auto i(0u); i < tokens.size(); ++i)
	{
		if(expectTkns(tokens, i, {"$define", "(", "?", ",", "?", ")", ";"}))
		{
			std::string alias{tokens[i + 2].str};
			if(defines.count(alias) > 0) throw;
			defines[alias] = tokens[i + 4].str;

			delTkns(tokens, i, {"$define", "(", "?", ",", "?", ")", ";"});
		}

	}

	ssvu::eraseRemoveIf(tokens, [](const Tkn& mTkn){ return mTkn.toDel; });

	for(auto& t : tokens) if(defines.count(t.str) > 0) t.str = defines[t.str];
	//for(auto& s : tokens) ssvu::lo()<<s.str.c_str()<<std::endl;

	struct Instr
	{
		std::string idnf{"NULL"};
		std::vector<std::string> args;
	};

	std::vector<Instr> instructions;
	Instr currentInstr;
	std::size_t tknIdx{0u};

	while(tknIdx < tokens.size())
	{
		currentInstr.idnf = tokens[tknIdx].str;
		++tknIdx;

		if(tokens[tknIdx].str == "(")
		{
			if(tokens[tknIdx + 1].str == ")" && tokens[tknIdx + 2].str == ";")
			{
				instructions.push_back(currentInstr); currentInstr = Instr{};
				tknIdx = tknIdx + 3; continue;
			}

			++tknIdx;
			while(tokens[tknIdx].str != ";")
			{
				currentInstr.args.push_back(ssvu::getReplaced(tokens[tknIdx].str, ".f", ""));
				tknIdx += 2;
			}

			++tknIdx;
			instructions.push_back(currentInstr); currentInstr = Instr{};
		}
	}

	//for(auto& s : instructions) ssvu::lo() << s.idnf << " " << s.args << "\n";

	std::map<std::string, std::string> labelIdxs;

	bool found{true};

	while(found)
	{
		found = false;
		for(auto i(0u); i < instructions.size(); ++i)
		{
			if(instructions[i].idnf == "$label")
			{
				found = true;
				labelIdxs[instructions[i].args[0]] = ssvu::toStr(i);
				instructions.erase(std::begin(instructions) + i);
				break;
			}
		}
	}

	for(auto& kk : labelIdxs) for(auto& i : instructions) for(auto& arg : i.args) if(arg == kk.first) arg = kk.second;
	//for(auto& s : instructions) ssvu::lo() << s.idnf << " " << s.args << "\n";

	Program result;

	struct SrcInstr
	{
		std::size_t requiredArgs{0u};
		OpCode opCode;
		ValueType argTypes[3];

		inline SrcInstr() = default;
		inline SrcInstr(OpCode mOpCode,
						ValueType mArgType0 = ValueType::Void,
						ValueType mArgType1 = ValueType::Void,
						ValueType mArgType2 = ValueType::Void)
			: opCode{mOpCode}
		{
			argTypes[0] = mArgType0;	if(argTypes[0] != ValueType::Void) ++requiredArgs;
			argTypes[1] = mArgType1;	if(argTypes[1] != ValueType::Void) ++requiredArgs;
			argTypes[2] = mArgType2;	if(argTypes[2] != ValueType::Void) ++requiredArgs;
		}

		inline void addToProgram(Program& mProgram, std::vector<std::string>& mArgs)
		{
			Params params;
			for(auto i(0u); i < requiredArgs; ++i)
			{
				params[i].setType(argTypes[i]);
				if(argTypes[i] == ValueType::Int)			params[i].set<int>(std::atoi(mArgs[i].c_str()));
				else if(argTypes[i] == ValueType::Float)	params[i].set<float>(float(std::atof(mArgs[i].c_str())));
			}

			Instruction instruction;
			instruction.opCode = opCode;
			instruction.params = params;

			mProgram += instruction;
		}
	};

	std::unordered_map<std::string, SrcInstr> srcInstrs;

	addSrcInstr(srcInstrs,	OpCode::halt																						);
	addSrcInstr(srcInstrs,	OpCode::loadIntCVToR,				ValueType::Int,			ValueType::Int							);
	addSrcInstr(srcInstrs,	OpCode::loadFloatCVToR,				ValueType::Int,			ValueType::Float						);
	addSrcInstr(srcInstrs,	OpCode::moveRVToR,					ValueType::Int,			ValueType::Int							);
	addSrcInstr(srcInstrs,	OpCode::pushRVToS,					ValueType::Int													);
	addSrcInstr(srcInstrs,	OpCode::popSVToR,					ValueType::Int													);
	addSrcInstr(srcInstrs,	OpCode::moveSBOVToR,				ValueType::Int,			ValueType::Int							);
	addSrcInstr(srcInstrs,	OpCode::pushIntCVToS,				ValueType::Int													);
	addSrcInstr(srcInstrs,	OpCode::pushFloatCVToS,				ValueType::Float												);
	addSrcInstr(srcInstrs,	OpCode::pushSVToS																					);
	addSrcInstr(srcInstrs,	OpCode::popSV																						);
	addSrcInstr(srcInstrs,	OpCode::callPI,						ValueType::Int													);
	addSrcInstr(srcInstrs,	OpCode::returnPI																					);
	addSrcInstr(srcInstrs,	OpCode::goToPI,						ValueType::Int													);
	addSrcInstr(srcInstrs,	OpCode::goToPIIfIntRV,				ValueType::Int,			ValueType::Int							);
	addSrcInstr(srcInstrs,	OpCode::goToPIIfCompareRVGreater,	ValueType::Int,			ValueType::Int							);
	addSrcInstr(srcInstrs,	OpCode::goToPIIfCompareRVSmaller,	ValueType::Int,			ValueType::Int							);
	addSrcInstr(srcInstrs,	OpCode::goToPIIfCompareRVEqual,		ValueType::Int,			ValueType::Int							);
	addSrcInstr(srcInstrs,	OpCode::incrementIntRV,				ValueType::Int													);
	addSrcInstr(srcInstrs,	OpCode::decrementIntRV,				ValueType::Int													);
	addSrcInstr(srcInstrs,	OpCode::addInt2SVs																					);
	addSrcInstr(srcInstrs,	OpCode::addFloat2SVs																				);
	addSrcInstr(srcInstrs,	OpCode::subtractInt2SVs																				);
	addSrcInstr(srcInstrs,	OpCode::subtractFloat2SVs																			);
	addSrcInstr(srcInstrs,	OpCode::multiplyInt2SVs																				);
	addSrcInstr(srcInstrs,	OpCode::multiplyFloat2SVs																			);
	addSrcInstr(srcInstrs,	OpCode::divideInt2SVs																				);
	addSrcInstr(srcInstrs,	OpCode::divideFloat2SVs																				);
	addSrcInstr(srcInstrs,	OpCode::compareIntRVIntRVToR,		ValueType::Int,			ValueType::Int,			ValueType::Int	);
	addSrcInstr(srcInstrs,	OpCode::compareIntRVIntSVToR,		ValueType::Int,			ValueType::Int							);
	addSrcInstr(srcInstrs,	OpCode::compareIntSVIntSVToR,		ValueType::Int													);
	addSrcInstr(srcInstrs,	OpCode::compareIntRVIntCVToR,		ValueType::Int,			ValueType::Int,			ValueType::Int	);
	addSrcInstr(srcInstrs,	OpCode::compareIntSVIntCVToR,		ValueType::Int,			ValueType::Int							);

	int idx{0};
	for(auto& s : instructions)
	{
		ssvu::lo() << idx++ << ":\t" << s.idnf << " " << s.args << "\n";

		if(srcInstrs.count(s.idnf) == 0)
		{
			ssvu::lo("ASSEMBLER ERROR") << "No OpCode with name '" << s.idnf << "'" << std::endl;
			throw;
		}

		auto& si(srcInstrs[s.idnf]);

		if(si.requiredArgs != s.args.size())
		{
			ssvu::lo("ASSEMBLER ERROR") << "OpCode '" << s.idnf << "' requires '" << si.requiredArgs << "' arguments" << std::endl;
			throw;
		}

		si.addToProgram(result, s.args);
	}

	return result;
}

void cfunc(int x, int y) { std::cout << x * y << std::endl; }

int main()
{
	SSVU_TEST_RUN_ALL();

	ssvvm::BoundFunction bf{&cfunc};
	ssvu::lo() << bf.call({2, 4}) << std::endl;

	return 0;
	ssvvm::Program program{makeProgram(source)};

	ssvvm::VirtualMachine vm;

ssvvm::VirtualMachine::BoundFunction bf = vm.bindCFunction(&cfunc);

	ssvvm::Params callParams{21};
	ssvvm::Value returnValue = bf.function->call(callParams);

	ssvu::lo() << returnValue << std::endl;

return 0;
	vm.setProgram(program);
	vm.run();

	//ssvu::lo() << vm.registry.getValue(0).get<float>() << "\n";
	//ssvu::lo() << vm.registry.getValue(1).get<float>() << "\n";

	return 0;
}


*/


#endif



