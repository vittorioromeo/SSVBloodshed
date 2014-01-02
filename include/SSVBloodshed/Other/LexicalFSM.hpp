// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef OB_TESTING_OTHER_LEXICALFSM
#define OB_TESTING_OTHER_LEXICALFSM

#include <vector>
#include <memory>
#include <SSVUtils/SSVUtils.hpp>
#include "SSVBloodshed/Other/Graph.hpp"
#include "SSVBloodshed/Other/FSM.hpp"

namespace ssvut
{
	template<typename T> class LexicalFSM : public FSM
	{
		private:
			T* la;

		public:
			inline LexicalFSM(T& mLexicalAnalyzer) noexcept : la{&mLexicalAnalyzer} { }

			inline LexicalFSM& exactly(const std::string& mStr)					{ continueOnce([this, mStr]{ return la->match(mStr); }, NodeType::Terminal); return *this; }
			inline LexicalFSM& once(const std::string& mStr, NodeType mType)	{ continueOnce([this, mStr]{ return la->match(mStr); }, mType); return *this; }
			inline LexicalFSM& repeat(const std::string& mStr, NodeType mType)	{ continueRepeat([this, mStr]{ return la->match(mStr); }, mType); return *this; }

			template<typename TF> inline LexicalFSM& matchOnce(const TF& mFunc, NodeType mType)		{ continueOnce([this, mFunc]{ return mFunc(la->getMatchChar()); }, mType); return *this; }
			template<typename TF> inline LexicalFSM& matchRepeat(const TF& mFunc, NodeType mType)	{ continueRepeat([this, mFunc]{ return mFunc(la->getMatchChar()); }, mType); return *this; }

			inline LexicalFSM& matchAnythingUntilOnce(NodeType mLoopType, const std::string& mEndStr, NodeType mEndType)
			{
				continueRepeatUntilOnce([this]{ return la->matchAnything(); }, mLoopType, [this, mEndStr]{ return la->match(mEndStr); }, mEndType);
				return *this;
			}
	};
}

#endif
