#define TOKENS

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

// TODO: prealloc iteration tests
// TODO: tutorailsc

namespace Internal
{
	template<typename TGraph> class GraphLink : public TGraph::StorageLinkBase
	{
		private:
			using NodePtr = typename TGraph::NodePtr;
			NodePtr node{TGraph::getNodeNull()};

		public:
			inline GraphLink(const NodePtr& mNode) noexcept : node{mNode} { assert(TGraph::isNodeValid(node)); }
			inline const NodePtr& getNode() const noexcept { assert(TGraph::isNodeValid(node)); return node; }
	};

	template<typename TGraph> class GraphNode : public TGraph::StorageNodeBase
	{
		private:
			using NodePtr = typename TGraph::NodePtr;
			using NodeDerived = typename TGraph::NodeDerived;

		public:
			template<typename... TArgs> inline void linkToSelf(TArgs&&... mArgs)
			{
				TGraph::StorageNodeBase::emplaceLink(TGraph::StorageNodeBase::getNodePtr(this), std::forward<TArgs>(mArgs)...);
			}
			template<typename... TArgs> inline void linkTo(const NodePtr& mNode, TArgs&&... mArgs)
			{
				assert(TGraph::isNodeValid(mNode));
				TGraph::StorageNodeBase::emplaceLink(mNode, std::forward<TArgs>(mArgs)...);
			}

			inline const decltype(TGraph::StorageNodeBase::links)& getLinks() const	{ return TGraph::StorageNodeBase::links; }
			inline bool isIsolated() const noexcept	{ return TGraph::StorageNodeBase::links.empty(); }
	};

	template<typename TGraph> struct GraphStorageFreeStore
	{
		using NodeDerived = typename TGraph::NodeDerived;
		using LinkDerived = typename TGraph::LinkDerived;
		using NodePtr = NodeDerived*;

		struct NodeBase
		{
			std::vector<LinkDerived> links;
			template<typename... TArgs> inline void emplaceLink(TArgs&&... mArgs) { links.emplace_back(std::forward<TArgs>(mArgs)...); }
			inline static NodePtr getNodePtr(GraphNode<TGraph>* mNode) noexcept { return reinterpret_cast<NodeDerived*>(mNode); }
		};
		struct LinkBase { };

		std::vector<ssvu::Uptr<NodeDerived>> nodes;

		inline static const NodePtr& getNodeNull() noexcept { static NodePtr result{nullptr}; return result; }
		inline static constexpr bool isNodeValid(const NodePtr& mNode) noexcept { return mNode != getNodeNull(); }

		template<typename... TArgs> inline NodePtr createNode(TArgs&&... mArgs)
		{
			static_assert(ssvu::isBaseOf<GraphNode<TGraph>, NodeDerived>(), "TNode must be derived from Graph::Node");

			auto result(new NodeDerived(std::forward<TArgs>(mArgs)...));
			nodes.emplace_back(result);
			return result;
		}
	};
}

template<typename TNode, typename TLink, template<typename> class TStorage = Internal::GraphStorageFreeStore> class Graph
{
	public:
		using Node = Internal::GraphNode<Graph>;
		using Link = Internal::GraphLink<Graph>;
		using NodeDerived = TNode;
		using LinkDerived = TLink;
		using Storage = TStorage<Graph>;
		using StorageNodeBase = typename Storage::NodeBase;
		using StorageLinkBase = typename Storage::LinkBase;
		using NodePtr = typename Storage::NodePtr;
		friend Storage;
		friend Node;
		friend Link;

	private:
		Storage storage;
		std::vector<NodePtr> nodes;

	protected:
		template<typename... TArgs> inline NodePtr createNode(TArgs&&... mArgs)
		{
			auto result(storage.createNode(std::forward<TArgs>(mArgs)...));
			nodes.push_back(result); return result;
		}

	public:
		inline const decltype(nodes)& getNodes() const noexcept				{ return nodes; }
		inline decltype(nodes)& getNodes() noexcept							{ return nodes; }
		inline static const NodePtr& getNodeNull() noexcept					{ return Storage::getNodeNull(); }
		inline static constexpr bool isNodeValid(NodePtr mNode) noexcept	{ return Storage::isNodeValid(mNode); }
		inline const NodePtr& getLastAddedNode() noexcept					{ assert(!nodes.empty()); return nodes.back(); }
};

struct FSMState;
struct FSMTransition;

enum class FSMNodeType{NonTerminal, Terminal};

using FSMType = Graph<FSMState, FSMTransition>;
using FSMRule = ssvu::Func<bool()>;

class FSMTransition : public FSMType::Link
{
	private:
		FSMRule rule;

	public:
		inline FSMTransition(const FSMType::NodePtr& mState, const FSMRule& mRule) noexcept : FSMType::Link{mState}, rule{mRule} { }
		inline bool matchesRule() const noexcept { return rule(); }
};

struct FSMState : public FSMType::Node
{
	private:
		FSMNodeType terminal;

	public:
		inline FSMState(FSMNodeType mTerminal) noexcept : terminal{mTerminal} { }

		inline const FSMType::NodePtr& getFirstMatchingTransition() const noexcept
		{
			for(const auto& c : getLinks()) if(c.matchesRule()) return c.getNode();
			return FSMType::getNodeNull();
		}

		inline bool isTerminal() const noexcept { return terminal == FSMNodeType::Terminal; }
};

class FSM : public FSMType
{
	public:
		using NodeType = FSMNodeType;

	private:
		NodePtr startState{getNodeNull()}, currentState{getNodeNull()};

	public:
		inline FSM() : startState{createNode(NodeType::NonTerminal)} { }

		inline FSM& continueOnce(const FSMRule& mRule, NodeType mTerminal)
		{
			auto last(getLastAddedNode());
			auto state(createNode(mTerminal));

			last->linkTo(state, mRule);

			return *this;
		}
		inline FSM& continueRepeat(const FSMRule& mRule, NodeType mTerminal)
		{
			auto last(getLastAddedNode());
			auto state(createNode(mTerminal));

			state->linkToSelf(mRule);
			last->linkTo(state, mRule);

			return *this;
		}

		inline FSM& continueRepeatUntilOnce(const FSMRule& mLoop, NodeType mLoopTerminal, const FSMRule& mEnd, NodeType mEndTerminal)
		{
			auto last(getLastAddedNode());
			auto stateEnd(createNode(mEndTerminal));
			auto stateLoop(createNode(mLoopTerminal));

			stateLoop->linkTo(stateEnd, mEnd);
			stateLoop->linkToSelf(mLoop);
			last->linkTo(stateEnd, mEnd);
			last->linkTo(stateLoop, mLoop);

			return *this;
		}

		inline void reset() noexcept							{ assert(startState != getNodeNull()); currentState = startState; }
		inline void setCurrentState(NodePtr mState) noexcept	{ currentState = mState; }
		inline NodePtr getCurrentState() const noexcept			{ assert(currentState != getNodeNull()); return currentState; }
};

template<typename T> class LexicalAnalyzerFSM : public FSM
{
	private:
		T* lexicalAnalyzer;

	public:
		inline LexicalAnalyzerFSM(T& mLexicalAnalyzer) noexcept : lexicalAnalyzer{&mLexicalAnalyzer} { }
		inline LexicalAnalyzerFSM& matchOnly(const std::string& mStr)
		{
			continueOnce([this, mStr]{ return lexicalAnalyzer->match(mStr); }, NodeType::Terminal);
			return *this;
		}
		inline LexicalAnalyzerFSM& matchOnce(const std::string& mStr, NodeType mType)
		{
			continueOnce([this, mStr]{ return lexicalAnalyzer->match(mStr); }, mType);
			return *this;
		}
		inline LexicalAnalyzerFSM& matchRepeat(const std::string& mStr, NodeType mType)
		{
			continueRepeat([this, mStr]{ return lexicalAnalyzer->match(mStr); }, mType);
			return *this;
		}
		template<typename TFunc> inline LexicalAnalyzerFSM& matchCharOnce(const TFunc& mFunc, NodeType mType)
		{
			continueOnce([this, mFunc]{ return mFunc(lexicalAnalyzer->getMatchChar()); }, mType);
			return *this;
		}
		template<typename TFunc> inline LexicalAnalyzerFSM& matchCharRepeat(const TFunc& mFunc, NodeType mType)
		{
			continueRepeat([this, mFunc]{ return mFunc(lexicalAnalyzer->getMatchChar()); }, mType);
			return *this;
		}
		inline LexicalAnalyzerFSM& matchRepeatAnythingUntilMatchOnce(NodeType mLoopType, const std::string& mEndStr, NodeType mEndType)
		{
			continueRepeatUntilOnce([this]{ return lexicalAnalyzer->matchAnything(); }, mLoopType, [this, mEndStr]{ return lexicalAnalyzer->match(mEndStr); }, mEndType);
			return *this;
		}
};

template<typename TTokenType, typename TTokenAttribute> class LexicalAnalyzer
{
	public:
		using LAFSM = LexicalAnalyzerFSM<LexicalAnalyzer<TTokenType, TTokenAttribute>>;

		struct Token
		{
			TTokenType type;
			TTokenAttribute attribute;
			std::string contents;

			inline Token(TTokenType mType, std::string mContents) : type{mType}, contents{std::move(mContents)} { }
		};

	private:
		std::map<TTokenType, LAFSM> matches;
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
		inline LAFSM& createMatchFSM(TTokenType mTokenType)
		{
			assert(matches.count(mTokenType) == 0);
			matches.insert(std::make_pair(mTokenType, LAFSM{*this}));
			return matches.at(mTokenType);
		}

		inline void setSource(std::string mSource) { source = std::move(mSource); }

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

				for(auto& p : matches)
				{
					auto& fsm(p.second);

					fsm.reset();
					markerEnd = markerBegin;

					auto nextNode(LAFSM::getNodeNull());

					while((nextNode = fsm.getCurrentState()->getFirstMatchingTransition()) != LAFSM::getNodeNull())
					{
						advance();
						//ssvu::lo() << source.substr(markerBegin, nextEnd - markerBegin) << std::endl;

						fsm.setCurrentState(nextNode);
						if(fsm.getCurrentState()->isTerminal())
						{
							canConsume = true;
							foundType = p.first;
						}
					}

					if(canConsume)
					{
						consume(foundType);
						break;
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

template<typename T, bool TDebug = true> std::string getPreprocessedSource(T mTokens);
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
using LAFSM = LexicalAnalyzer<TokenType, TokenAttribute>::LAFSM;

int main()
{
	LexicalAnalyzer<TokenType, TokenAttribute> la;

	la.createMatchFSM(TokenType::PreprocessorStart).matchOnly("$");
	la.createMatchFSM(TokenType::Semicolon).matchOnly(";");
	la.createMatchFSM(TokenType::Comma).matchOnly(",");
	la.createMatchFSM(TokenType::ParenthesisRoundOpen).matchOnly("(");
	la.createMatchFSM(TokenType::ParenthesisRoundClose).matchOnly(")");
	la.createMatchFSM(TokenType::Float).matchCharRepeat([](char mC){ return std::isdigit(mC); }, FSM::NodeType::NonTerminal).matchOnce(".f", FSM::NodeType::Terminal);
	la.createMatchFSM(TokenType::Integer).matchCharRepeat([](char mC){ return std::isdigit(mC); }, FSM::NodeType::Terminal);
	la.createMatchFSM(TokenType::Identifier).matchCharRepeat([](char mC){ return isIdnfChar<true>(mC); }, FSM::NodeType::Terminal).matchCharRepeat([](char mC){ return isIdnfChar<false>(mC); }, FSM::NodeType::Terminal);
	la.createMatchFSM(TokenType::Comment).matchOnce("//", FSM::NodeType::Terminal).matchRepeatAnythingUntilMatchOnce(FSM::NodeType::NonTerminal, "\n", FSM::NodeType::Terminal);
	la.createMatchFSM(TokenType::WhiteSpace).matchCharRepeat([](char mC){ return std::isspace(mC); }, FSM::NodeType::Terminal);

	la.setSource(source);
	la.tokenize();

	std::string preprocessed{getPreprocessedSource(la.getTokens())};

	la.setSource(preprocessed);
	la.tokenize();

	ssvvm::Program program{makeProgram(la.getTokens())};
	ssvvm::VirtualMachine vm;
	vm.setProgram(program);
	vm.run();

	return 0;
}

template<typename T, bool TDebug> inline std::string getPreprocessedSource(T mTokens)
{
	using namespace ssvvm;
	std::string result;

	// Helper functions
	auto getTokenAsInt =	[&mTokens](std::size_t mIdx) -> int					{ return std::stoi(mTokens[mIdx].contents.c_str()); };
	auto getTokenAsFloat =	[&mTokens](std::size_t mIdx) -> float				{ return std::stof(mTokens[mIdx].contents.substr(0, mTokens[mIdx].contents.size() - 2).c_str()); };
	auto getTokenContents =	[&mTokens](std::size_t mIdx) -> const std::string&	{ return mTokens[mIdx].contents; };
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
			if(getTokenContents(i + 1) == "require_registers")
			{
				requireRegisters = getTokenAsInt(i + 3);
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
			if(getTokenContents(i + 1) != "define") continue;

			const auto& alias(getTokenContents(i + 3));
			const auto& replacement(getTokenContents(i + 5));

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
	std::size_t foundLabels{0u}, currentInstruction{0u};
	std::map<std::string, std::size_t> labels;
	for(auto i(0u); i < mTokens.size(); ++i)
	{
		if(mTokens[i].type == TokenType::Semicolon) ++currentInstruction;

		if(matchTypes(i, {TokenType::PreprocessorStart, TokenType::Identifier, TokenType::ParenthesisRoundOpen, TokenType::Identifier, TokenType::ParenthesisRoundClose, TokenType::Semicolon}))
		{
			if(mTokens[i + 1].contents != "label") continue;

			const auto& name(getTokenContents(i + 3));
			if(TDebug) ssvu::lo("makeProgram - phase 3") << "Found `$label`: " << name << "\n";

			if(defines.count(name) > 0)
			{
				if(TDebug) ssvu::lo("makeProgram - phase 3") << "ERROR: label name `" << name << "` already previously encountered" << "\n";
				throw;
			}

			labels[name] = currentInstruction - foundLabels;
			++foundLabels;

			for(auto k(0u); k < 6; ++k) mTokens[i + k].attribute.toDel = true;
		}
	}

	ssvu::eraseRemoveIf(mTokens, [](const VAToken& mT){ return mT.attribute.toDel; });

	if(TDebug) ssvu::lo("makeProgram - phase 3") << "Applying `$label` directives..." << "\n";
	for(auto& t : mTokens) if(labels.count(t.contents) > 0) t.contents = ssvu::toStr(labels[t.contents]);
	if(TDebug) ssvu::lo("makeProgram - phase 3") << "Done" << "\n";



	for(auto& t : mTokens) result += t.contents;
	return result;
}

template<typename T> inline void addInstructionTemplate(T& mTarget, ssvvm::OpCode mOpCode,
											 ssvvm::ValueType mArgType0 = ssvvm::ValueType::Void,
											 ssvvm::ValueType mArgType1 = ssvvm::ValueType::Void,
											 ssvvm::ValueType mArgType2 = ssvvm::ValueType::Void)
{
	mTarget[getOpCodeStr(mOpCode)] = {mOpCode, mArgType0, mArgType1, mArgType2};
}

template<typename T, bool TDebug> inline ssvvm::Program makeProgram(T mTokens)
{
	//for(auto& t : mTokens) ssvu::lo() << t.contents;

	using namespace ssvvm;
	Program result;

	// Helper functions
	auto getTokenAsInt =	[&mTokens](std::size_t mIdx) -> int					{ return std::stoi(mTokens[mIdx].contents.c_str()); };
	auto getTokenAsFloat =	[&mTokens](std::size_t mIdx) -> float				{ return std::stof(mTokens[mIdx].contents.substr(0, mTokens[mIdx].contents.size() - 2).c_str()); };
	auto getTokenContents =	[&mTokens](std::size_t mIdx) -> const std::string&	{ return mTokens[mIdx].contents; };

	// Phase 4: separating instructions using semicolons
	struct SrcInstruction { std::string identifier; std::vector<Value> args; };
	std::vector<SrcInstruction> srcInstructions;

	std::size_t idx{0u};
	while(idx < mTokens.size())
	{
		SrcInstruction currentSrcInstruction;

		if(mTokens[idx].type != TokenType::Identifier)
		{
			if(TDebug) ssvu::lo("makeProgram - phase 3") << "ERROR: expected `" << getTokenContents(idx) << "` to be an identifier" << "\n";
			throw;
		}

		currentSrcInstruction.identifier = getTokenContents(idx);

		++idx;

		if(mTokens[idx].type != TokenType::ParenthesisRoundOpen)
		{
			if(TDebug) ssvu::lo("makeProgram - phase 3") << "ERROR: expected `" << getTokenContents(idx) << "` to be an open round parenthesis" << "\n";
			throw;
		}

		++idx;

		if(mTokens[idx].type == TokenType::ParenthesisRoundClose)
		{
			++idx;

			if(mTokens[idx].type != TokenType::Semicolon)
			{
				if(TDebug) ssvu::lo("makeProgram - phase 3") << "ERROR: expected `" << getTokenContents(idx) << "` to be a semicolon" << "\n";
				throw;
			}
		}
		else
		{
			while(mTokens[idx].type != TokenType::Semicolon)
			{
				if(mTokens[idx].type == TokenType::Float)			currentSrcInstruction.args.push_back(Value::create<float>(getTokenAsFloat(idx)));
				else if(mTokens[idx].type == TokenType::Integer)	currentSrcInstruction.args.push_back(Value::create<int>(getTokenAsInt(idx)));
				else
				{
					if(TDebug) ssvu::lo("makeProgram - phase 3") << "ERROR: expected `" << getTokenContents(idx) << "` to be a float or an integer" << "\n";
					throw;
				}

				++idx;

				if(mTokens[idx].type != TokenType::Comma && mTokens[idx].type != TokenType::ParenthesisRoundClose)
				{
					if(TDebug) ssvu::lo("makeProgram - phase 3") << "ERROR: expected `" << getTokenContents(idx) << "` to be a comma or a close round parenthesis" << "\n";
					throw;
				}

				++idx;
			}
		}

		srcInstructions.push_back(currentSrcInstruction);
		++idx;
	}



	struct InstructionTemplate
	{
		std::size_t requiredArgs{0u};
		OpCode opCode;
		ValueType argTypes[3];

		inline InstructionTemplate() = default;
		inline InstructionTemplate(OpCode mOpCode, ValueType mT0 = ValueType::Void, ValueType mT1 = ValueType::Void, ValueType mT2 = ValueType::Void) : opCode{mOpCode}
		{
			argTypes[0] = mT0;	if(argTypes[0] != ValueType::Void) ++requiredArgs;
			argTypes[1] = mT1;	if(argTypes[1] != ValueType::Void) ++requiredArgs;
			argTypes[2] = mT2;	if(argTypes[2] != ValueType::Void) ++requiredArgs;
		}

		inline void addToProgram(Program& mProgram, std::vector<Value>& mArgs)
		{
			Params params;
			for(auto i(0u); i < requiredArgs; ++i) params[i] = mArgs[i];

			Instruction instruction;
			instruction.opCode = opCode;
			instruction.params = params;

			mProgram += instruction;
		}
	};

	std::unordered_map<std::string, InstructionTemplate> instructionTemplates;

	addInstructionTemplate(instructionTemplates,	OpCode::halt																						);
	addInstructionTemplate(instructionTemplates,	OpCode::loadIntCVToR,				ValueType::Int,			ValueType::Int							);
	addInstructionTemplate(instructionTemplates,	OpCode::loadFloatCVToR,				ValueType::Int,			ValueType::Float						);
	addInstructionTemplate(instructionTemplates,	OpCode::moveRVToR,					ValueType::Int,			ValueType::Int							);
	addInstructionTemplate(instructionTemplates,	OpCode::pushRVToS,					ValueType::Int													);
	addInstructionTemplate(instructionTemplates,	OpCode::popSVToR,					ValueType::Int													);
	addInstructionTemplate(instructionTemplates,	OpCode::moveSBOVToR,				ValueType::Int,			ValueType::Int							);
	addInstructionTemplate(instructionTemplates,	OpCode::pushIntCVToS,				ValueType::Int													);
	addInstructionTemplate(instructionTemplates,	OpCode::pushFloatCVToS,				ValueType::Float												);
	addInstructionTemplate(instructionTemplates,	OpCode::pushSVToS																					);
	addInstructionTemplate(instructionTemplates,	OpCode::popSV																						);
	addInstructionTemplate(instructionTemplates,	OpCode::callPI,						ValueType::Int													);
	addInstructionTemplate(instructionTemplates,	OpCode::returnPI																					);
	addInstructionTemplate(instructionTemplates,	OpCode::goToPI,						ValueType::Int													);
	addInstructionTemplate(instructionTemplates,	OpCode::goToPIIfIntRV,				ValueType::Int,			ValueType::Int							);
	addInstructionTemplate(instructionTemplates,	OpCode::goToPIIfCompareRVGreater,	ValueType::Int,			ValueType::Int							);
	addInstructionTemplate(instructionTemplates,	OpCode::goToPIIfCompareRVSmaller,	ValueType::Int,			ValueType::Int							);
	addInstructionTemplate(instructionTemplates,	OpCode::goToPIIfCompareRVEqual,		ValueType::Int,			ValueType::Int							);
	addInstructionTemplate(instructionTemplates,	OpCode::incrementIntRV,				ValueType::Int													);
	addInstructionTemplate(instructionTemplates,	OpCode::decrementIntRV,				ValueType::Int													);
	addInstructionTemplate(instructionTemplates,	OpCode::addInt2SVs																					);
	addInstructionTemplate(instructionTemplates,	OpCode::addFloat2SVs																				);
	addInstructionTemplate(instructionTemplates,	OpCode::subtractInt2SVs																				);
	addInstructionTemplate(instructionTemplates,	OpCode::subtractFloat2SVs																			);
	addInstructionTemplate(instructionTemplates,	OpCode::multiplyInt2SVs																				);
	addInstructionTemplate(instructionTemplates,	OpCode::multiplyFloat2SVs																			);
	addInstructionTemplate(instructionTemplates,	OpCode::divideInt2SVs																				);
	addInstructionTemplate(instructionTemplates,	OpCode::divideFloat2SVs																				);
	addInstructionTemplate(instructionTemplates,	OpCode::compareIntRVIntRVToR,		ValueType::Int,			ValueType::Int,			ValueType::Int	);
	addInstructionTemplate(instructionTemplates,	OpCode::compareIntRVIntSVToR,		ValueType::Int,			ValueType::Int							);
	addInstructionTemplate(instructionTemplates,	OpCode::compareIntSVIntSVToR,		ValueType::Int													);
	addInstructionTemplate(instructionTemplates,	OpCode::compareIntRVIntCVToR,		ValueType::Int,			ValueType::Int,			ValueType::Int	);
	addInstructionTemplate(instructionTemplates,	OpCode::compareIntSVIntCVToR,		ValueType::Int,			ValueType::Int							);



	std::size_t instructionIdx{0};
	for(auto& i : srcInstructions)
	{
		ssvu::lo(instructionIdx++) << i.identifier << " " << i.args << "\n";

		if(instructionTemplates.count(i.identifier) == 0)
		{
			ssvu::lo("ASSEMBLER ERROR") << "No OpCode with name '" << i.identifier << "'" << std::endl;
			throw;
		}

		auto& it(instructionTemplates[i.identifier]);

		if(it.requiredArgs != i.args.size())
		{
			ssvu::lo("ASSEMBLER ERROR") << "OpCode '" << i.identifier << "' requires '" << it.requiredArgs << "' arguments" << std::endl;
			throw;
		}

		it.addToProgram(result, i.args);
	}

	ssvu::lo().flush();
	return result;
}

#endif

