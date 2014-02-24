// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVVM_STACK
#define SSVVM_STACK

#include "SSVBloodshed/SSVVM/Common.hpp"
#include "SSVBloodshed/SSVVM/Value.hpp"

namespace ssvvm
{
	class Stack
	{
		private:
			std::vector<Value> stack;
			int baseOffset{0}; // Distance between top and base

		public:
			inline void pushBaseOffset() noexcept	{ push(Value::create<int>(baseOffset)); baseOffset = 0; }
			inline void popBaseOffset() noexcept	{ baseOffset = getPop().get<int>(); }

			inline void push(Value mValue) noexcept	{ stack.emplace_back(mValue); ++baseOffset; }
			inline Value getPop() noexcept			{ SSVU_ASSERT(!stack.empty()); auto result(stack.back()); stack.pop_back(); --baseOffset; return result; }

			inline const Value& getTop() const noexcept				{ return stack.back(); }
			inline Value& getTop() noexcept							{ return stack.back(); }
			inline const Value& getTop(int mOffset) const noexcept	{ return *(std::end(stack) - mOffset - 1); }
			inline Value& getTop(int mOffset) noexcept				{ return *(std::end(stack) - mOffset - 1); }

			inline void pop() noexcept	{ SSVU_ASSERT(!stack.empty()); stack.pop_back(); }

			inline Value getFromBase(int mOffset) noexcept	{ return *(std::end(stack) - baseOffset - mOffset - 1); }
			inline int getBaseOffset() const				{ return baseOffset; }

			inline const decltype(stack)& getStack() const noexcept	{ return stack; }
	};

}

#endif
