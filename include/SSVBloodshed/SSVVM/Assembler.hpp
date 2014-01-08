// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVVM_ASMASSEMBLER
#define SSVVM_ASMASSEMBLER

#include "SSVBloodshed/SSVVM/Common.hpp"
#include "SSVBloodshed/SSVVM/ASMLexicalAnalyzer.hpp"
#include "SSVBloodshed/SSVVM/Program.hpp"

namespace ssvvm
{
	namespace Internal
	{
		struct InstructionTemplate
		{
			std::size_t requiredArgs{0u};
			OpCode opCode;
			VMVal argTypes[3];

			inline InstructionTemplate() = default;
			inline InstructionTemplate(OpCode mOpCode, VMVal mT0 = VMVal::Void, VMVal mT1 = VMVal::Void, VMVal mT2 = VMVal::Void) : opCode{mOpCode}
			{
				argTypes[0] = mT0;	if(argTypes[0] != VMVal::Void) ++requiredArgs;
				argTypes[1] = mT1;	if(argTypes[1] != VMVal::Void) ++requiredArgs;
				argTypes[2] = mT2;	if(argTypes[2] != VMVal::Void) ++requiredArgs;
			}

			inline void addToProgram(Program& mProgram, std::vector<Value>& mArgs) const
			{
				Params params;
				for(auto i(0u); i < requiredArgs; ++i) params[i] = mArgs[i];

				Instruction instruction;
				instruction.opCode = opCode;
				instruction.params = params;

				mProgram += instruction;
			}
		};

		using InstructionTemplateMap = std::unordered_map<std::string, InstructionTemplate>;

		inline void addInstructionTemplate(InstructionTemplateMap& mMap, OpCode mOpCode, VMVal mArgType0 = VMVal::Void, VMVal mArgType1 = VMVal::Void, VMVal mArgType2 = VMVal::Void)
		{
			mMap[getOpCodeStr(mOpCode)] = {mOpCode, mArgType0, mArgType1, mArgType2};
		}

		inline const InstructionTemplateMap& getInstructionTemplates()
		{
			static bool initialized{false};
			static InstructionTemplateMap instructionTemplates;

			if(!initialized)
			{
				addInstructionTemplate(instructionTemplates,	OpCode::halt																			);
				addInstructionTemplate(instructionTemplates,	OpCode::loadIntCVToR,				VMVal::Int,			VMVal::Int						);
				addInstructionTemplate(instructionTemplates,	OpCode::loadFloatCVToR,				VMVal::Int,			VMVal::Float					);
				addInstructionTemplate(instructionTemplates,	OpCode::moveRVToR,					VMVal::Int,			VMVal::Int						);
				addInstructionTemplate(instructionTemplates,	OpCode::pushRVToS,					VMVal::Int											);
				addInstructionTemplate(instructionTemplates,	OpCode::popSVToR,					VMVal::Int											);
				addInstructionTemplate(instructionTemplates,	OpCode::moveSBOVToR,				VMVal::Int,			VMVal::Int						);
				addInstructionTemplate(instructionTemplates,	OpCode::pushIntCVToS,				VMVal::Int											);
				addInstructionTemplate(instructionTemplates,	OpCode::pushFloatCVToS,				VMVal::Float										);
				addInstructionTemplate(instructionTemplates,	OpCode::pushSVToS																		);
				addInstructionTemplate(instructionTemplates,	OpCode::popSV																			);
				addInstructionTemplate(instructionTemplates,	OpCode::callPI,						VMVal::Int											);
				addInstructionTemplate(instructionTemplates,	OpCode::returnPI																		);
				addInstructionTemplate(instructionTemplates,	OpCode::goToPI,						VMVal::Int											);
				addInstructionTemplate(instructionTemplates,	OpCode::goToPIIfIntRV,				VMVal::Int,			VMVal::Int						);
				addInstructionTemplate(instructionTemplates,	OpCode::goToPIIfCompareRVGreater,	VMVal::Int,			VMVal::Int						);
				addInstructionTemplate(instructionTemplates,	OpCode::goToPIIfCompareRVSmaller,	VMVal::Int,			VMVal::Int						);
				addInstructionTemplate(instructionTemplates,	OpCode::goToPIIfCompareRVEqual,		VMVal::Int,			VMVal::Int						);
				addInstructionTemplate(instructionTemplates,	OpCode::incrementIntRV,				VMVal::Int											);
				addInstructionTemplate(instructionTemplates,	OpCode::decrementIntRV,				VMVal::Int											);
				addInstructionTemplate(instructionTemplates,	OpCode::addInt2SVs																		);
				addInstructionTemplate(instructionTemplates,	OpCode::addFloat2SVs																	);
				addInstructionTemplate(instructionTemplates,	OpCode::subtractInt2SVs																	);
				addInstructionTemplate(instructionTemplates,	OpCode::subtractFloat2SVs																);
				addInstructionTemplate(instructionTemplates,	OpCode::multiplyInt2SVs																	);
				addInstructionTemplate(instructionTemplates,	OpCode::multiplyFloat2SVs																);
				addInstructionTemplate(instructionTemplates,	OpCode::divideInt2SVs																	);
				addInstructionTemplate(instructionTemplates,	OpCode::divideFloat2SVs																	);
				addInstructionTemplate(instructionTemplates,	OpCode::compareIntRVIntRVToR,		VMVal::Int,			VMVal::Int,			VMVal::Int	);
				addInstructionTemplate(instructionTemplates,	OpCode::compareIntRVIntSVToR,		VMVal::Int,			VMVal::Int						);
				addInstructionTemplate(instructionTemplates,	OpCode::compareIntSVIntSVToR,		VMVal::Int											);
				addInstructionTemplate(instructionTemplates,	OpCode::compareIntRVIntCVToR,		VMVal::Int,			VMVal::Int,			VMVal::Int	);
				addInstructionTemplate(instructionTemplates,	OpCode::compareIntSVIntCVToR,		VMVal::Int,			VMVal::Int						);

				initialized = true;
			}

			return instructionTemplates;
		}

		inline bool hasInstructionTemplate(const std::string& mId) noexcept					{ return getInstructionTemplates().count(mId) > 0; }
		inline const InstructionTemplate& getInstructionTemplate(const std::string& mId)	{ return getInstructionTemplates().at(mId); }
	}

	template<bool TDebug> inline Program getAssembledProgram(SourceVeeAsm& mSource)
	{
		if(!mSource.isPreprocessed()) throw;

		getASMLA().setSource(mSource.getSourceString());
		getASMLA().tokenize();

		auto tokens(getASMLA().getTokens());

		//for(auto& t : tokens) ssvu::lo() << t.contents;

		Program result;

		// Phase 4: separating instructions using semicolons
		struct SrcInstruction { std::string identifier; std::vector<Value> args; };
		std::vector<SrcInstruction> srcInstructions;

		std::size_t idx{0u};
		while(idx < tokens.size())
		{
			SrcInstruction currentSrcInstruction;

			if(tokens[idx].type != VMToken::Identifier)
			{
				if(TDebug) ssvu::lo("makeProgram - phase 3") << "ERROR: expected `" << getTokenContents(tokens, idx) << "` to be an identifier" << "\n";
				throw;
			}

			currentSrcInstruction.identifier = getTokenContents(tokens, idx);

			++idx;

			if(tokens[idx].type != VMToken::ParenthesisRoundOpen)
			{
				if(TDebug) ssvu::lo("makeProgram - phase 3") << "ERROR: expected `" << getTokenContents(tokens, idx) << "` to be an open round parenthesis" << "\n";
				throw;
			}

			++idx;

			if(tokens[idx].type == VMToken::ParenthesisRoundClose)
			{
				++idx;

				if(tokens[idx].type != VMToken::Semicolon)
				{
					if(TDebug) ssvu::lo("makeProgram - phase 3") << "ERROR: expected `" << getTokenContents(tokens, idx) << "` to be a semicolon" << "\n";
					throw;
				}
			}
			else
			{
				while(tokens[idx].type != VMToken::Semicolon)
				{
					if(tokens[idx].type == VMToken::Float)			currentSrcInstruction.args.emplace_back(Value::create<float>(getTokenAsFloat(tokens, idx)));
					else if(tokens[idx].type == VMToken::Integer)	currentSrcInstruction.args.emplace_back(Value::create<int>(getTokenAsInt(tokens, idx)));
					else
					{
						if(TDebug) ssvu::lo("makeProgram - phase 3") << "ERROR: expected `" << getTokenContents(tokens, idx) << "` to be a float or an integer" << "\n";
						throw;
					}

					++idx;

					if(tokens[idx].type != VMToken::Comma && tokens[idx].type != VMToken::ParenthesisRoundClose)
					{
						if(TDebug) ssvu::lo("makeProgram - phase 3") << "ERROR: expected `" << getTokenContents(tokens, idx) << "` to be a comma or a close round parenthesis" << "\n";
						throw;
					}

					++idx;
				}
			}

			srcInstructions.emplace_back(currentSrcInstruction);
			++idx;
		}

		std::size_t instructionIdx{0};
		for(auto& i : srcInstructions)
		{
			ssvu::lo(instructionIdx++) << i.identifier << " " << i.args << "\n";

			if(!Internal::hasInstructionTemplate(i.identifier))
			{
				ssvu::lo("ASSEMBLER ERROR") << "No OpCode with name '" << i.identifier << "'" << std::endl;
				throw;
			}

			auto& it(Internal::getInstructionTemplate(i.identifier));

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
}

#endif
