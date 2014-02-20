// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVVM_ASMLEXICALANALYZER
#define SSVVM_ASMLEXICALANALYZER

#include "SSVBloodshed/SSVVM/Common.hpp"
#include "SSVBloodshed/SSVVM/Value.hpp"
#include "SSVBloodshed/SSVVM/Params.hpp"
#include "SSVBloodshed/Other/Graph.hpp"
#include "SSVBloodshed/Other/FSM.hpp"
#include "SSVBloodshed/Other/LexicalFSM.hpp"
#include "SSVBloodshed/Other/LexicalAnalyzer.hpp"

namespace ssvvm
{
	template<bool TStart> inline bool isIdnfChar(char mChar) noexcept
	{
		static constexpr char validChars[]{'_'};
		return (TStart && ssvu::isAlphabetical(mChar)) || (!TStart && ssvu::isAlphanumeric(mChar)) || ssvu::contains(validChars, mChar);
	}

	enum class VMToken : int
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
	struct VMTokenData { bool toDel{false}; };

	using ASMLexicalAnalyzer = ssvut::LexicalAnalyzer<VMToken, VMTokenData>;
	using ASMLAToken = ASMLexicalAnalyzer::Token;
	using ASMLAFSM = ASMLexicalAnalyzer::LAFSM;

	inline ASMLexicalAnalyzer& getASMLA() noexcept
	{
		using FSMNT = ssvut::FSM::NodeType;

		static bool initialized{false};
		static ASMLexicalAnalyzer la;

		if(!initialized)
		{
			la.createMatchFSM(VMToken::PreprocessorStart).exactly("$");
			la.createMatchFSM(VMToken::Semicolon).exactly(";");
			la.createMatchFSM(VMToken::Comma).exactly(",");
			la.createMatchFSM(VMToken::ParenthesisRoundOpen).exactly("(");
			la.createMatchFSM(VMToken::ParenthesisRoundClose).exactly(")");
			la.createMatchFSM(VMToken::Float).matchRepeat(&ssvu::isDigit, FSMNT::NonTerminal).once(".f", FSMNT::Terminal);
			la.createMatchFSM(VMToken::Integer).matchRepeat(&ssvu::isDigit, FSMNT::Terminal);
			la.createMatchFSM(VMToken::Identifier).matchRepeat(&isIdnfChar<true>, FSMNT::Terminal).matchRepeat(&isIdnfChar<false>, FSMNT::Terminal);
			la.createMatchFSM(VMToken::Comment).once("//", FSMNT::Terminal).matchAnythingUntilOnce(FSMNT::NonTerminal, "\n", FSMNT::Terminal);
			la.createMatchFSM(VMToken::WhiteSpace).matchRepeat(&ssvu::isSpace, FSMNT::Terminal);

			initialized = true;
		}

		return la;
	}
}

#endif

// TODO: Create BNF grammar and parsers
