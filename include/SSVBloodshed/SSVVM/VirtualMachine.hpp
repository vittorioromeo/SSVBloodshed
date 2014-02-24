// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVVM_VIRTUALMACHINE
#define SSVVM_VIRTUALMACHINE

#include "SSVBloodshed/SSVVM/Common.hpp"
#include "SSVBloodshed/SSVVM/Value.hpp"
#include "SSVBloodshed/SSVVM/Params.hpp"
#include "SSVBloodshed/SSVVM/Registry.hpp"
#include "SSVBloodshed/SSVVM/Stack.hpp"
#include "SSVBloodshed/SSVVM/Instruction.hpp"
#include "SSVBloodshed/SSVVM/Program.hpp"

namespace ssvvm
{
	namespace Internal
	{
		template<std::size_t TRegistrySize, bool TDebug> class VMImpl
		{
			public:
				Registry<TRegistrySize> registry;
				Stack stack;

				Instruction::Idx programCounter{0};
				Program program;

				Instruction programInstruction;
				VMFnPtr<VMImpl> fnPtr;
				Params params;

				bool running{false};

				// Helper functions
				inline Value& getRV(const Value& mValueIdx) noexcept	{ return registry.getValue(mValueIdx.get<Register::Idx>()); }
				template<typename T> inline Value execOnStack2(const T& mFn) noexcept
				{
					Value a{stack.getPop()}, b{stack.getPop()};

					if(TDebug)
					{
						ssvu::lo("execOnStack2") << "Value A\t" << a << "\n";
						ssvu::lo("execOnStack2") << "Value B\t" << b << "\n";
					}

					return mFn(a, b);
				}
				template<typename T> inline T getFromValue(const Value& mValue) const noexcept { return mValue.template get<T>(); }

				// Instructions
				inline void halt() noexcept
				{
					running = false;
					if(TDebug) ssvu::lo("halt") << "Execution halted" << "\n";
				}

				inline void loadIntCVToR() noexcept
				{
					SSVU_ASSERT(params[1].getType() == VMVal::Int);

					auto& regValue(getRV(params[0]));
					regValue = params[1];

					if(TDebug)
					{
						const auto& dbgIdxReg(getFromValue<Register::Idx>(params[0]));
						ssvu::lo("loadIntCVToR") << "Loaded " << params[1] << " into register " << dbgIdxReg << "\n";
					}
				}
				inline void loadFloatCVToR() noexcept
				{
					SSVU_ASSERT(params[1].getType() == VMVal::Float);

					auto& regValue(getRV(params[0]));
					regValue = params[1];

					if(TDebug)
					{
						const auto& dbgIdxReg(getFromValue<Register::Idx>(params[0]));
						ssvu::lo("loadFloatCVToR") << "Loaded " << params[1] << " into register " << dbgIdxReg << "\n";
					}
				}

				inline void moveRVToR() noexcept
				{
					const auto& idxDst(getFromValue<Register::Idx>(params[0]));
					const auto& idxSrc(params[1].template get<Register::Idx>());

					registry.get(idxDst) = registry.get(idxSrc);

					if(TDebug)
					{
						ssvu::lo("moveRVToR") << "Moved register " << idxSrc << " into register " << idxDst << "\n";
						ssvu::lo("moveRVToR") << "Both registers' value is now " << registry.get(idxSrc).value << "\n";
					}
				}

				inline void pushRVToS() noexcept
				{
					const auto& toPush(getRV(params[0]));
					stack.push(toPush);

					if(TDebug)
					{
						const auto& dbgIdxReg(getFromValue<Register::Idx>(params[0]));
						ssvu::lo("pushRVToS") << "Pushed register " << dbgIdxReg << " value " << toPush << " onto stack" << "\n";
					}
				}
				inline void popSVToR() noexcept
				{
					auto& popDst(getRV(params[0]));
					popDst = stack.getPop();

					if(TDebug)
					{
						const auto& dbgIdxReg(getFromValue<Register::Idx>(params[0]));
						ssvu::lo("popSVToR") << "Popped value " << popDst << " in register " << dbgIdxReg << " from stack" << "\n";
					}
				}
				inline void moveSBOVToR() noexcept
				{
					const auto& sbOffset(stack.getFromBase(params[1].template get<int>()));

					if(TDebug)
					{
						const auto& dbgIdxReg(getFromValue<Register::Idx>(params[0]));
						ssvu::lo("moveSBOVToR") << "Moved SBO value " << sbOffset << " into register " << dbgIdxReg << " from stack base offset" << "\n";
					}

					getRV(params[0]) = sbOffset;
				}

				inline void pushIntCVToS() noexcept
				{
					if(TDebug)
					{
						ssvu::lo("pushIntCVToS") << "Pushing constant int value " << params[0] << " on stack" << "\n";
					}

					SSVU_ASSERT(params[0].getType() == VMVal::Int);
					stack.push(params[0]);
				}

				inline void pushFloatCVToS() noexcept
				{
					if(TDebug)
					{
						ssvu::lo("pushFloatCVToS") << "Pushing constant float value " << params[0] << " on stack" << "\n";
					}

					SSVU_ASSERT(params[0].getType() == VMVal::Float);
					stack.push(params[0]);
				}

				inline void pushSVToS() noexcept
				{
					if(TDebug)
					{
						ssvu::lo("pushSVToS") << "Pushing stack top (duplicating) value " << stack.getTop() << "\n";
					}

					stack.push(stack.getTop());
				}

				inline void popSV() noexcept
				{
					if(TDebug)
					{
						ssvu::lo("popSV") << "Popping stack top (removing) value " << stack.getTop() << "\n";
					}

					stack.pop();
				}

				inline void goToPI() noexcept
				{
					const auto& jmpDst(getFromValue<Instruction::Idx>(params[0]));

					if(TDebug)
					{
						ssvu::lo("goToPI") << "Unconditional jump to instruction " << jmpDst << "\n";
					}

					programCounter = jmpDst;
				}
				inline void goToPIIfIntRV() noexcept
				{
					const auto& jmpDst(getFromValue<Instruction::Idx>(params[0]));
					const auto& cndVal(getRV(params[1]));

					if(TDebug)
					{
						const auto& dbgIdxReg(getFromValue<Register::Idx>(params[1]));
						ssvu::lo("goToPIIfIntRV") << "Conditional jump to instruction " << jmpDst << "\n";
						ssvu::lo("goToPIIfIntRV") << "Condition: register " << dbgIdxReg << " value " << cndVal << " != 0\n";
					}

					if(cndVal.template get<int>() != 0)
					{
						programCounter = jmpDst;
						if(TDebug) ssvu::lo("goToPIIfIntRV") << "Conditional jump SUCCESS" << "\n";
					}
					else if(TDebug) ssvu::lo("goToPIIfIntRV") << "Conditional jump FAILURE" << "\n";
				}

				inline void goToPIIfCompareRVGreater() noexcept
				{
					const auto& jmpDst(getFromValue<Instruction::Idx>(params[0]));
					const auto& cndVal(getRV(params[1]));

					if(TDebug)
					{
						const auto& dbgIdxReg(getFromValue<Register::Idx>(params[1]));
						ssvu::lo("goToPIIfCompareRVGreater") << "Conditional jump to instruction " << jmpDst << "\n";
						ssvu::lo("goToPIIfCompareRVGreater") << "Condition GREATER: register " << dbgIdxReg << " compare value " << cndVal << " > 0\n";
					}

					if(cndVal.template get<int>() > 0)
					{
						programCounter = jmpDst;
						if(TDebug) ssvu::lo("goToPIIfCompareRVGreater") << "Conditional jump SUCCESS" << "\n";
					}
					else if(TDebug) ssvu::lo("goToPIIfCompareRVGreater") << "Conditional jump FAILURE" << "\n";
				}
				inline void goToPIIfCompareRVSmaller() noexcept
				{
					const auto& jmpDst(getFromValue<Instruction::Idx>(params[0]));
					const auto& cndVal(getRV(params[1]));

					if(TDebug)
					{
						const auto& dbgIdxReg(getFromValue<Register::Idx>(params[1]));
						ssvu::lo("goToPIIfCompareRVSmaller") << "Conditional jump to instruction " << jmpDst << "\n";
						ssvu::lo("goToPIIfCompareRVSmaller") << "Condition SMALLER: register " << dbgIdxReg << " compare value " << cndVal << " < 0\n";
					}

					if(cndVal.template get<int>() < 0)
					{
						programCounter = jmpDst;
						if(TDebug) ssvu::lo("goToPIIfCompareRVSmaller") << "Conditional jump SUCCESS" << "\n";
					}
					else if(TDebug) ssvu::lo("goToPIIfCompareRVSmaller") << "Conditional jump FAILURE" << "\n";
				}
				inline void goToPIIfCompareRVEqual() noexcept
				{
					const auto& jmpDst(getFromValue<Instruction::Idx>(params[0]));
					const auto& cndVal(getRV(params[1]));

					if(TDebug)
					{
						const auto& dbgIdxReg(getFromValue<Register::Idx>(params[1]));
						ssvu::lo("goToPIIfCompareRVEqual") << "Conditional jump to instruction " << jmpDst << "\n";
						ssvu::lo("goToPIIfCompareRVEqual") << "Condition EQUAL: register " << dbgIdxReg << " compare value " << cndVal << " == 0\n";
					}

					if(cndVal.template get<int>() == 0)
					{
						programCounter = jmpDst;
						if(TDebug) ssvu::lo("goToPIIfCompareRVEqual") << "Conditional jump SUCCESS" << "\n";
					}
					else if(TDebug) ssvu::lo("goToPIIfCompareRVEqual") << "Conditional jump FAILURE" << "\n";
				}


				inline void callPI() noexcept
				{
					const auto& callDst(getFromValue<Instruction::Idx>(params[0]));

					if(TDebug) ssvu::lo("callPI") << "Preparing to call function at instruction " << callDst << "\n";

					if(TDebug) ssvu::lo("callPI") << "Push current instruction (" << programCounter << ") on stack (for return)\n";
					stack.push(Value::create<Instruction::Idx>(programCounter));

					if(TDebug) ssvu::lo("callPI") << "Push current stack base (" << stack.getBaseOffset() << ") and reset it\n";
					stack.pushBaseOffset();

					if(TDebug) ssvu::lo("callPI") << "Calling function (jumping) at instruction " << callDst << "\n";
					programCounter = callDst;
				}
				inline void returnPI() noexcept
				{
					if(TDebug)
					{
						ssvu::lo("returnPI") << "Returning from a function - restoring stack base\n";
						ssvu::lo("returnPI") << "\tBefore: " << stack.getBaseOffset() << "\n";
					}

					stack.popBaseOffset();
					const auto& returnDst(getFromValue<Instruction::Idx>(stack.getPop()));

					if(TDebug)
					{
						ssvu::lo("returnPI") << "\tAfter: " << stack.getBaseOffset() << "\n";
						ssvu::lo("callPI") << "Returning (jumping) to instruction at the top of the stack (" << returnDst << ")" << "\n";
					}

					programCounter =  returnDst;
				}

				inline void incrementIntRV() noexcept
				{
					auto& regVal(getRV(params[0]));

					if(TDebug)
					{
						const auto& dbgIdxReg(getFromValue<Register::Idx>(params[0]));
						ssvu::lo("incrementIntRV") << "Incrementing value " << regVal << " in register " << dbgIdxReg << "\n";
					}

					regVal.template set<int>(regVal.template get<int>() + 1);
				}
				inline void decrementIntRV() noexcept
				{
					auto& regVal(getRV(params[0]));

					if(TDebug)
					{
						const auto& dbgIdxReg(getFromValue<Register::Idx>(params[0]));
						ssvu::lo("decrementIntRV") << "Decrementing value " << regVal << " in register " << dbgIdxReg << "\n";
					}

					regVal.template set<int>(regVal.template get<int>() - 1);
				}

				inline void addInt2SVs() noexcept
				{
					if(TDebug) ssvu::lo("addInt2SVs") << "Adding 2 ints" << "\n";
					stack.push(execOnStack2(VMOperations::getAddition<int>));
				}
				inline void addFloat2SVs() noexcept
				{
					if(TDebug) ssvu::lo("addFloat2SVs") << "Adding 2 floats" << "\n";
					stack.push(execOnStack2(VMOperations::getAddition<float>));
				}

				inline void subtractInt2SVs() noexcept
				{
					if(TDebug) ssvu::lo("subtractInt2SVs") << "Subtracting 2 ints" << "\n";
					stack.push(execOnStack2(VMOperations::getSubtraction<int>));
				}
				inline void subtractFloat2SVs() noexcept
				{
					if(TDebug) ssvu::lo("subtractFloat2SVs") << "Subtracting 2 floats" << "\n";
					stack.push(execOnStack2(VMOperations::getSubtraction<float>));
				}

				inline void multiplyInt2SVs() noexcept
				{
					if(TDebug) ssvu::lo("multiplyInt2SVs") << "Multiplying 2 ints" << "\n";
					stack.push(execOnStack2(VMOperations::getMultiplication<int>));
				}
				inline void multiplyFloat2SVs() noexcept
				{
					if(TDebug) ssvu::lo("multiplyFloat2SVs") << "Multiplying 2 floats" << "\n";
					stack.push(execOnStack2(VMOperations::getMultiplication<float>));
				}

				inline void divideInt2SVs() noexcept
				{
					if(TDebug) ssvu::lo("divideInt2SVs") << "Dividing 2 ints" << "\n";
					stack.push(execOnStack2(VMOperations::getDivision<int>));
				}
				inline void divideFloat2SVs() noexcept
				{
					if(TDebug) ssvu::lo("divideFloat2SVs") << "Dividing 2 floats" << "\n";
					stack.push(execOnStack2(VMOperations::getDivision<float>));
				}


				inline void compareIntRVIntRVToR() noexcept
				{
					const auto& idxDst(getFromValue<Register::Idx>(params[0]));
					const auto& idxA(getFromValue<Register::Idx>(params[1]));
					const auto& idxB(getFromValue<Register::Idx>(params[2]));
					const auto& valA(getFromValue<int>(registry.get(idxA).value));
					const auto& valB(getFromValue<int>(registry.get(idxB).value));
					const auto& result(VMOperations::getIntComparison(valA, valB));

					registry.get(idxDst).value = result;

					if(TDebug)
					{
						ssvu::lo("compareIntRVIntRVToR") << "Comparing register value " << valA << " with register value " << valB << " into register " << idxDst << "\n";
						ssvu::lo("compareIntRVIntRVToR") << "Register value is now " << result << "\n";
					}
				}
				inline void compareIntRVIntSVToR() noexcept
				{
					const auto& idxDst(getFromValue<Register::Idx>(params[0]));
					const auto& idxA(getFromValue<Register::Idx>(params[1]));
					const auto& valA(getFromValue<int>(registry.get(idxA).value));
					const auto& valB(stack.getTop());
					const auto& result(VMOperations::getIntComparison(valA, valB));

					registry.get(idxDst).value = result;

					if(TDebug)
					{
						ssvu::lo("compareIntRVIntSVToR") << "Comparing register value " << valA << " with stack value " << valB << " into register " << idxDst << "\n";
						ssvu::lo("compareIntRVIntSVToR") << "Register value is now " << result << "\n";
					}
				}
				inline void compareIntSVIntSVToR() noexcept
				{
					const auto& idxDst(getFromValue<Register::Idx>(params[0]));
					const auto& valA(stack.getTop());
					const auto& valB(stack.getTop(1));
					const auto& result(VMOperations::getIntComparison(valA, valB));

					registry.get(idxDst).value = result;

					if(TDebug)
					{
						ssvu::lo("compareIntRVIntSVToR") << "Comparing stack value " << valA << " with stack value " << valB << " into register " << idxDst << "\n";
						ssvu::lo("compareIntRVIntSVToR") << "Register value is now " << result << "\n";
					}
				}
				inline void compareIntRVIntCVToR() noexcept
				{
					const auto& idxDst(getFromValue<Register::Idx>(params[0]));
					const auto& idxA(getFromValue<Register::Idx>(params[1]));
					const auto& valA(getFromValue<int>(registry.get(idxA).value));
					const auto& valB(getFromValue<int>(params[2]));
					const auto& result(VMOperations::getIntComparison(valA, valB));

					registry.get(idxDst).value = result;

					if(TDebug)
					{
						ssvu::lo("compareIntRVIntCVToR") << "Comparing register value " << valA << " with constant int " << valB << " into register " << idxDst << "\n";
						ssvu::lo("compareIntRVIntCVToR") << "Register value is now " << result << "\n";
					}
				}
				inline void compareIntSVIntCVToR() noexcept
				{
					const auto& idxDst(getFromValue<Register::Idx>(params[0]));
					const auto& valA(stack.getTop());
					const auto& valB(getFromValue<int>(params[1]));
					const auto& result(VMOperations::getIntComparison(valA, valB));

					registry.get(idxDst).value = result;

					if(TDebug)
					{
						ssvu::lo("compareIntSVIntCVToR") << "Comparing stack value " << valA << " with constant int " << valB << " into register " << idxDst << "\n";
						ssvu::lo("compareIntSVIntCVToR") << "Register value is now " << result << "\n";
					}
				}

				// Execution impl
				inline void fetch() noexcept
				{
					if(TDebug) ssvu::lo("fetch") << "Fetching instruction at " << programCounter << "\n";
					programInstruction = program[programCounter++];
				}
				inline void decode() noexcept
				{
					if(TDebug) ssvu::lo("decode") << "Decoding instruction: OPCODE(" << int(programInstruction.opCode) << ")" << "\n";
					fnPtr = getVMFnPtr<VMImpl>(programInstruction.opCode); params = programInstruction.params;
				}
				inline void eval() noexcept	{ (this->*fnPtr)(); }

				// Execution interface
				inline void run() noexcept
				{
					running = true;
					while(running)
					{
						fetch(); decode(); eval();

						if(TDebug)
						{
							ssvu::lo() << "\n";
							ssvu::lo("run()") << "Printing VM state...\n\n";
							const auto& st(stack.getStack());

							for(int i{0}; i < int(std::max(st.size(), registry.getSize())); ++i)
							{
								std::size_t sIdx(st.size() - i - 1);

								ssvu::lo() << ((sIdx < st.size()) ? "\t|--------------------|" : "\t                      ");

								if(i < int(registry.getSize())) ssvu::lo() << "\t\tRegister " << i << ": " << registry.get(i).value;

								ssvu::lo() << "\n";

								if(sIdx < st.size())
								{
									ssvu::lo() << ((i == int(stack.getBaseOffset())) ? "--->\t" : "\t") << i << "(" << -(int(stack.getBaseOffset()) - i) << ")" << "\t" << st.at(sIdx) << "\n";
								}

								if(sIdx == 0) ssvu::lo() << "\t|--------------------|\n";
							}

							ssvu::lo() << "\n";
						}
					}

					ssvu::lo().flush();
				}

				inline void setProgram(Program mProgram) noexcept { program = std::move(mProgram); }



		};
	}

	class VirtualMachine : public Internal::VMImpl<6, true> { };
}

#endif
