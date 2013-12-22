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


struct FSM
{
	using Rule = ssvu::Func<bool()>;

	struct State
	{
		Rule rule;
		std::vector<State*> transitions;
		bool terminal{true};
	};

	std::vector<std::unique_ptr<State>> states;
	State* currentState;
	std::vector<State*> history;

	inline State& createState()
	{
		auto result(new State);
		states.emplace_back(result);
		return *result;
	}

	inline FSM& startOnce(const Rule& mRule)
	{
		auto& state(createState());
		state.rule = mRule;
		currentState = &state;
		history.push_back(&state);
		return *this;
	}
	inline FSM& startRepeat(const Rule& mRule)
	{
		auto& state(createState());
		state.rule = mRule;
		state.transitions.push_back(&state);
		currentState = &state;
		history.push_back(&state);
		return *this;
	}

	inline FSM& continueOnce(const Rule& mRule)
	{
		auto& state(createState());
		state.rule = mRule;
		history.back()->transitions.push_back(&state);
		history.push_back(&state);
		return *this;
	}
	inline FSM& continueRepeat(const Rule& mRule)
	{
		auto& state(createState());
		state.rule = mRule;
		state.transitions.push_back(&state);
		history.back()->transitions.push_back(&state);
		history.push_back(&state);
		return *this;
	}

	inline FSM& connectWithPrevious()
	{
		history.back()->transitions.push_back(*(std::end(history) - 2));
		return *this;
	}

	inline FSM& continueRepeatUntilOnce(const Rule& mLoop, const Rule& mEnd)
	{
		auto& stateEnd(createState());
		stateEnd.rule = mEnd;

		auto& stateLoop(createState());
		stateLoop.rule = mLoop;
		stateLoop.transitions.push_back(&stateEnd);
		stateLoop.transitions.push_back(&stateLoop);

		history.back()->transitions.push_back(&stateEnd);
		history.back()->transitions.push_back(&stateLoop);

		return *this;
	}
};

template<typename TTokenType, typename TTokenAttribute> class LexicalAnalyzer
{
	public:
		struct Token
		{
			TTokenType type;
			TTokenAttribute attribute;
		};

		std::map<TTokenType, FSM*> matches;

	private:
		std::string source;
		std::size_t markerBegin, markerEnd, nextStep;

	public:
		inline void setSource(std::string mSource) { source = std::move(mSource); }
		inline void addMatch(TTokenType mTokenType, FSM& mFSM)
		{
			matches[mTokenType] = &mFSM;
		}
		inline char getCurrentChar() { ++nextStep; return source[markerEnd]; }
		inline bool match(const std::string& mMatch, bool mConsume = true)
		{
			for(auto i(0u); i < mMatch.size(); ++i)
			{
				auto idxToCheck(markerEnd + i);
				if(idxToCheck >= source.size())
				{
					return false;
				}
				if(source[idxToCheck] != mMatch[i])
				{
					return false;
				}
			}

			nextStep = mMatch.size();

			return true;
		}
		inline bool matchAnything() { ++nextStep; return true; }

		inline void tokenize()
		{
			markerBegin = markerEnd = 0;

			while(markerEnd < source.size())
			{
				bool foundAnyMatch{false};

				for(const auto& p : matches)
				{

					const auto& tokenType(p.first);
					auto& fsm(*p.second);
					auto current(fsm.currentState);

					markerEnd = markerBegin;
					nextStep = 0;

					if(current->rule())
					{
						while(true)
						{
							markerEnd += nextStep;
							nextStep = 0;

							if(current->terminal) foundAnyMatch = true;
							if(current->transitions.empty()) break;

							bool found{false};
							for(const auto& t : current->transitions)
							{
								if(t->rule())
								{
									current = t;
									found = true;
									break;
								}
							}

							if(!found) break;
						}

					}

					if(foundAnyMatch) break;
				}

				if(!foundAnyMatch)
				{
					ssvu::lo() << "didn't find any match" << std::endl;
					return;
				}

				//ssvu::lo() << "found match" << std::endl;
				ssvu::lo() << source.substr(markerBegin, markerEnd - markerBegin) << std::endl;
				markerBegin = markerEnd;
			}
		}
};

int main()
{
	enum class TokenType
	{
		PreprocessorStart,		// '$'
		Semicolon,				// ';'
		ParenthesisRoundOpen,	// '('
		ParenthesisRoundClose,	// ')'
		Number,
		Identifier,
		Comment
	};

	struct TokenAttribute { };
	LexicalAnalyzer<TokenType, TokenAttribute> la;

	FSM fsmPreprocessorStart;
	FSM fsmSemicolon;
	FSM fsmParenthesisRoundOpen;
	FSM fsmParenthesisRoundClose;
	FSM fsmNumber;
	FSM fsmIdentifier;
	FSM fsmComment;

	fsmPreprocessorStart.startOnce([&la]{ return la.match("$"); });
	fsmSemicolon.startOnce([&la]{ return la.match(";"); });
	fsmParenthesisRoundOpen.startOnce([&la]{ return la.match("("); });
	fsmParenthesisRoundClose.startOnce([&la]{ return la.match(")"); });
	fsmNumber.startRepeat([&la]{ return std::isdigit(la.getCurrentChar()); }).continueOnce([&la]{ return la.match(".f"); });
	fsmIdentifier.startRepeat([&la]{ return std::isalpha(la.getCurrentChar()); }).continueRepeat([&la]{ return std::isdigit(la.getCurrentChar()); }).connectWithPrevious();
	fsmComment.startOnce([&la]{ return la.match("//"); }).continueRepeatUntilOnce([&la]{ return la.matchAnything(); }, [&la]{ return la.match("\n", false); });

	la.addMatch(TokenType::PreprocessorStart, fsmPreprocessorStart);
	la.addMatch(TokenType::Semicolon, fsmSemicolon);
	la.addMatch(TokenType::ParenthesisRoundOpen, fsmParenthesisRoundOpen);
	la.addMatch(TokenType::ParenthesisRoundClose, fsmParenthesisRoundClose);
	la.addMatch(TokenType::Number, fsmNumber);
	la.addMatch(TokenType::Identifier, fsmIdentifier);
	la.addMatch(TokenType::Comment, fsmComment);

	la.setSource("(2s2our22ce)1234//cocks\n");
	//la.setSource("abcdef");
	la.tokenize();

	ssvu::lo() << "end" << std::endl;

	return 0;
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



