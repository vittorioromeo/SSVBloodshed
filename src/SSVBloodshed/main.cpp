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

	vm.program += {IT::LoadFloatCVToR, 0, 2.f};
	vm.program += {IT::LoadFloatCVToR, 2, 12.f};
	vm.program += {IT::LoadIntCVToR, 1, 14};

	vm.program += {IT::PushRVToS, 0};
	vm.program += {IT::PushRVToS, 2};

	vm.program += {IT::MultiplyFloat2SVs};
	vm.program += {IT::PopSVToR, 0};


	vm.program += {IT::PushRVToS, 0};
	vm.program += {IT::PushRVToS, 2};

	vm.program += {IT::AddFloat2SVs};
	vm.program += {IT::PopSVToR, 0};

	vm.program += {IT::DecrementIntRV, 1};
	vm.program += {IT::GoToPIIfIntRV, 3, 1};


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

ssvvm::Program makeProgram(std::string mSource)
{
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
	for(const auto& x : ssvu::getSplit<ssvu::Split::KeepSeparatorAsToken>(mSource, splitsKeep)) tokens.emplace_back(x);

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

	ssvvm::Program result;

	int idx{0};
	for(auto& s : instructions)
	{
		ssvu::lo() << idx++ << ":\t" << s.idnf << " " << s.args << "\n";
		if(s.idnf == "halt")					{ if(s.args.size() == 0) result += {ssvvm::OpCode::Halt};																								else throw; }
		else if(s.idnf == "loadIntCVToR")		{ if(s.args.size() == 2) result += {ssvvm::OpCode::LoadIntCVToR,		std::atoi(s.args[0].c_str()),			std::atoi(s.args[1].c_str())};			else throw; }
		else if(s.idnf == "loadFloatCVToR")		{ if(s.args.size() == 2) result += {ssvvm::OpCode::LoadFloatCVToR,		std::atoi(s.args[0].c_str()),			(float)std::atof(s.args[1].c_str())};	else throw; }
		else if(s.idnf == "moveRVToR")			{ if(s.args.size() == 2) result += {ssvvm::OpCode::MoveRVToR,			std::atoi(s.args[0].c_str()),			std::atoi(s.args[1].c_str())};			else throw; }
		else if(s.idnf == "pushRVToS")			{ if(s.args.size() == 1) result += {ssvvm::OpCode::PushRVToS,			std::atoi(s.args[0].c_str())};													else throw; }
		else if(s.idnf == "popSVToR")			{ if(s.args.size() == 1) result += {ssvvm::OpCode::PopSVToR,			std::atoi(s.args[0].c_str())};													else throw; }
		else if(s.idnf == "moveSBOVToR")		{ if(s.args.size() == 2) result += {ssvvm::OpCode::MoveSBOVToR,			std::atoi(s.args[0].c_str()),			std::atoi(s.args[1].c_str())};			else throw; }
		else if(s.idnf == "pushIntCVToS")		{ if(s.args.size() == 1) result += {ssvvm::OpCode::PushIntCVToS,		std::atoi(s.args[0].c_str())};													else throw; }
		else if(s.idnf == "pushFloatCVToS")		{ if(s.args.size() == 1) result += {ssvvm::OpCode::PushFloatCVToS,		(float)std::atof(s.args[0].c_str())};											else throw; }
		else if(s.idnf == "pushSVToS")			{ if(s.args.size() == 0) result += {ssvvm::OpCode::PushSVToS};																							else throw; }
		else if(s.idnf == "popSV")				{ if(s.args.size() == 0) result += {ssvvm::OpCode::PopSV};																								else throw; }
		else if(s.idnf == "callPI")				{ if(s.args.size() == 1) result += {ssvvm::OpCode::CallPI,				std::atoi(s.args[0].c_str())};													else throw; }
		else if(s.idnf == "returnPI")			{ if(s.args.size() == 0) result += {ssvvm::OpCode::ReturnPI};																							else throw; }
		else if(s.idnf == "goToPI")				{ if(s.args.size() == 1) result += {ssvvm::OpCode::GoToPI,				std::atoi(s.args[0].c_str())};													else throw; }
		else if(s.idnf == "goToPIIfIntRV")		{ if(s.args.size() == 2) result += {ssvvm::OpCode::GoToPIIfIntRV,		std::atoi(s.args[0].c_str()),			std::atoi(s.args[1].c_str())};			else throw; }
		else if(s.idnf == "goToPIIfCompareRVGreater")	{ if(s.args.size() == 2) result += {ssvvm::OpCode::GoToPIIfCompareRVGreater,		std::atoi(s.args[0].c_str()),			std::atoi(s.args[1].c_str())};			else throw; }
		else if(s.idnf == "goToPIIfCompareRVSmaller")	{ if(s.args.size() == 2) result += {ssvvm::OpCode::GoToPIIfCompareRVSmaller,		std::atoi(s.args[0].c_str()),			std::atoi(s.args[1].c_str())};			else throw; }
		else if(s.idnf == "goToPIIfCompareRVEqual")		{ if(s.args.size() == 2) result += {ssvvm::OpCode::GoToPIIfCompareRVEqual,			std::atoi(s.args[0].c_str()),			std::atoi(s.args[1].c_str())};			else throw; }
		else if(s.idnf == "incrementIntRV")		{ if(s.args.size() == 1) result += {ssvvm::OpCode::IncrementIntRV,		std::atoi(s.args[0].c_str())};													else throw; }
		else if(s.idnf == "decrementIntRV")		{ if(s.args.size() == 1) result += {ssvvm::OpCode::DecrementIntRV,		std::atoi(s.args[0].c_str())};													else throw; }
		else if(s.idnf == "addInt2SVs")			{ if(s.args.size() == 0) result += {ssvvm::OpCode::AddInt2SVs};																							else throw; }
		else if(s.idnf == "addFloat2SVs")		{ if(s.args.size() == 0) result += {ssvvm::OpCode::AddFloat2SVs};																						else throw; }
		else if(s.idnf == "subtractInt2SVs")	{ if(s.args.size() == 0) result += {ssvvm::OpCode::SubtractInt2SVs};																					else throw; }
		else if(s.idnf == "subtractFloat2SVs")	{ if(s.args.size() == 0) result += {ssvvm::OpCode::SubtractFloat2SVs};																					else throw; }
		else if(s.idnf == "multiplyInt2SVs")	{ if(s.args.size() == 0) result += {ssvvm::OpCode::MultiplyInt2SVs};																					else throw; }
		else if(s.idnf == "multiplyFloat2SVs")	{ if(s.args.size() == 0) result += {ssvvm::OpCode::MultiplyFloat2SVs};																					else throw; }
		else if(s.idnf == "divideInt2SVs")		{ if(s.args.size() == 0) result += {ssvvm::OpCode::DivideInt2SVs};																						else throw; }
		else if(s.idnf == "divideFloat2SVs")	{ if(s.args.size() == 0) result += {ssvvm::OpCode::DivideFloat2SVs};																					else throw; }

		else if(s.idnf == "compareIntRVIntRVToR")	{ if(s.args.size() == 3) result += {ssvvm::OpCode::CompareIntRVIntRVToR,	std::atoi(s.args[0].c_str()),	std::atoi(s.args[1].c_str()),	std::atoi(s.args[2].c_str())}; else throw; }
		else if(s.idnf == "compareIntRVIntSVToR")	{ if(s.args.size() == 2) result += {ssvvm::OpCode::CompareIntRVIntSVToR,	std::atoi(s.args[0].c_str()),	std::atoi(s.args[1].c_str())}; else throw; }
		else if(s.idnf == "compareIntSVIntSVToR")	{ if(s.args.size() == 1) result += {ssvvm::OpCode::CompareIntSVIntSVToR,	std::atoi(s.args[0].c_str())}; else throw; }
		else if(s.idnf == "compareIntRVIntCVToR")	{ if(s.args.size() == 3) result += {ssvvm::OpCode::CompareIntRVIntCVToR,	std::atoi(s.args[0].c_str()),	std::atoi(s.args[1].c_str()),	std::atoi(s.args[2].c_str())}; else throw; }
		else if(s.idnf == "compareIntSVIntCVToR")	{ if(s.args.size() == 2) result += {ssvvm::OpCode::CompareIntSVIntCVToR,	std::atoi(s.args[0].c_str()),	std::atoi(s.args[1].c_str())}; else throw; }

		else throw;
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
/*
ssvvm::VirtualMachine::BoundFunction bf = vm.bindCFunction(&cfunc);

	ssvvm::Params callParams{21};
	ssvvm::Value returnValue = bf.function->call(callParams);

	ssvu::lo() << returnValue << std::endl;

return 0;*/
	vm.setProgram(program);
	vm.run();

	//ssvu::lo() << vm.registry.getValue(0).get<float>() << "\n";
	//ssvu::lo() << vm.registry.getValue(1).get<float>() << "\n";

	return 0;
}

#endif


