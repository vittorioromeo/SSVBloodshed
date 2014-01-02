// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVVM_ASMPREPROCESSOR
#define SSVVM_ASMPREPROCESSOR

#include "SSVBloodshed/SSVVM/Common.hpp"
#include "SSVBloodshed/SSVVM/ASMLexicalAnalyzer.hpp"

namespace ssvvm
{
	template<bool TDebug> inline void preprocessSourceRaw(SourceVeeAsm& mSource)
	{
		if(mSource.isPreprocessed()) throw;

		getASMLA().setSource(mSource.getSourceString());
		getASMLA().tokenize();

		auto tokens(getASMLA().getTokens());


		std::string result;

		// Helper functions
		auto matchTypes = [&tokens](std::size_t mIdx, const std::vector<VMToken>& mTypes) -> bool
		{
			std::size_t ub{mIdx + mTypes.size()}, typeIdx{0u};
			if(ub >= tokens.size()) return false;

			for(auto i(mIdx); i < ub; ++i)
			{
				const auto& type(mTypes[typeIdx++]);
				if(type != VMToken::Anything && type != tokens[i].type) return false;
			}
			return true;
		};



		// Phase 0: discard Comment/WhiteSpace tokens
		ssvu::eraseRemoveIf(tokens, [](const ASMLAToken& mT){ return mT.type == VMToken::WhiteSpace || mT.type == VMToken::Comment; });



		// Phase 1: `$require_registers` directive
		int requireRegisters{-1};

		for(auto i(0u); i < tokens.size(); ++i)
			if(matchTypes(i, {VMToken::PreprocessorStart, VMToken::Identifier, VMToken::ParenthesisRoundOpen, VMToken::Integer, VMToken::ParenthesisRoundClose, VMToken::Semicolon}))
				if(getTokenContents(tokens, i + 1) == "require_registers")
				{
					requireRegisters = getTokenAsInt(tokens, i + 3);
					for(auto k(0u); k < 6; ++k) tokens[i + k].toDel = true;
					if(TDebug) ssvu::lo("makeProgram - phase 1") << "Found `$require_registers` = " << requireRegisters << "\n";
					break;
				}

		ssvu::eraseRemoveIf(tokens, [](const ASMLAToken& mT){ return mT.toDel; });


		//for(const auto& t : tokens) ssvu::lo()<<t.contents<<std::endl;

		// Phase 2: `$define` directives
		std::map<std::string, std::string> defines;
		for(auto i(0u); i < tokens.size(); ++i)
			if(matchTypes(i, {VMToken::PreprocessorStart, VMToken::Identifier, VMToken::ParenthesisRoundOpen, VMToken::Identifier, VMToken::Comma, VMToken::Anything, VMToken::ParenthesisRoundClose, VMToken::Semicolon}))
			{
				if(getTokenContents(tokens, i + 1) != "define") continue;

				const auto& alias(getTokenContents(tokens, i + 3));
				const auto& replacement(getTokenContents(tokens, i + 5));

				if(TDebug) ssvu::lo("makeProgram - phase 2") << "Found `$define`: " << alias << " -> " << replacement << "\n";

				if(defines.count(alias) > 0)
				{
					if(TDebug) ssvu::lo("makeProgram - phase 2") << "ERROR: alias `" << alias << "` already previously defined" << "\n";
					throw;
				}

				defines[alias] = replacement;
				for(auto k(0u); k < 8; ++k) tokens[i + k].toDel = true;
			}

		ssvu::eraseRemoveIf(tokens, [](const ASMLAToken& mT){ return mT.toDel; });

		if(TDebug) ssvu::lo("makeProgram - phase 2") << "Applying `$define` directives..." << "\n";
		for(auto& t : tokens) if(defines.count(t.contents) > 0) t.contents = defines[t.contents];
		if(TDebug) ssvu::lo("makeProgram - phase 2") << "Done" << "\n";



		// Phase 3: `$label` directives
		std::size_t foundLabels{0u}, currentInstruction{0u};
		std::map<std::string, std::size_t> labels;
		for(auto i(0u); i < tokens.size(); ++i)
		{
			if(tokens[i].type == VMToken::Semicolon) ++currentInstruction;

			if(matchTypes(i, {VMToken::PreprocessorStart, VMToken::Identifier, VMToken::ParenthesisRoundOpen, VMToken::Identifier, VMToken::ParenthesisRoundClose, VMToken::Semicolon}))
			{
				if(tokens[i + 1].contents != "label") continue;

				const auto& name(getTokenContents(tokens, i + 3));
				if(TDebug) ssvu::lo("makeProgram - phase 3") << "Found `$label`: " << name << "\n";

				if(defines.count(name) > 0)
				{
					if(TDebug) ssvu::lo("makeProgram - phase 3") << "ERROR: label name `" << name << "` already previously encountered" << "\n";
					throw;
				}

				labels[name] = currentInstruction - foundLabels;
				++foundLabels;

				for(auto k(0u); k < 6; ++k) tokens[i + k].toDel = true;
			}
		}

		ssvu::eraseRemoveIf(tokens, [](const ASMLAToken& mT){ return mT.toDel; });

		if(TDebug) ssvu::lo("makeProgram - phase 3") << "Applying `$label` directives..." << "\n";
		for(auto& t : tokens) if(labels.count(t.contents) > 0) t.contents = ssvu::toStr(labels[t.contents]);
		if(TDebug) ssvu::lo("makeProgram - phase 3") << "Done" << "\n";

		for(auto& t : tokens) result += t.contents;

		ssvu::lo()<<result<<std::endl;

		mSource.setSourceString(result);
		mSource.setPreprocessed(true);
	}
}

#endif
