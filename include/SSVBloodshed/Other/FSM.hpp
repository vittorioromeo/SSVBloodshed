// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef OB_TESTING_OTHER_FSM
#define OB_TESTING_OTHER_FSM

#include <vector>
#include <memory>
#include <SSVUtils/SSVUtils.hpp>
#include "SSVBloodshed/Other/Graph.hpp"

namespace ssvut
{
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
}

#endif
