// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_CESYSTEM_MACROS
#define SSVOB_CESYSTEM_MACROS

#define VA_NUM_ARGS(...) VA_NUM_ARGS_IMPL(__VA_ARGS__, 11,10,9,8,7,6,5,4,3,2,1)
#define VA_NUM_ARGS_IMPL(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,N,...) N

#define MACRO_DISPATCH__(func, nargs) func ## nargs
#define MACRO_DISPATCH_(func, nargs) MACRO_DISPATCH__(func, nargs)
#define MACRO_DISPATCH(func, ...) MACRO_DISPATCH_(func, VA_NUM_ARGS(__VA_ARGS__))

#define SYSTEM_LOOP_1(nameEntity) for(const auto& __sysTpl : this->getTuples()) { auto& nameEntity(this->getEntity(__sysTpl));
#define SYSTEM_LOOP_2(nameEntity, nameC0) for(const auto& __sysTpl : this->getTuples()) { auto& nameEntity(this->getEntity(__sysTpl)); auto& nameC0(this->getComponent<0>(__sysTpl));
#define SYSTEM_LOOP_3(nameEntity, nameC0, nameC1) for(const auto& __sysTpl : this->getTuples()) { auto& nameEntity(this->getEntity(__sysTpl)); auto& nameC0(this->getComponent<0>(__sysTpl)); auto& nameC1(this->getComponent<1>(__sysTpl));
#define SYSTEM_LOOP_4(nameEntity, nameC0, nameC1, nameC2) for(const auto& __sysTpl : this->getTuples()) { auto& nameEntity(this->getEntity(__sysTpl)); auto& nameC0(this->getComponent<0>(__sysTpl)); auto& nameC1(this->getComponent<1>(__sysTpl)); auto& nameC2(this->getComponent<2>(__sysTpl));
#define SYSTEM_LOOP_5(nameEntity, nameC0, nameC1, nameC2, nameC3) for(const auto& __sysTpl : this->getTuples()) { auto& nameEntity(this->getEntity(__sysTpl)); auto& nameC0(this->getComponent<0>(__sysTpl)); auto& nameC1(this->getComponent<1>(__sysTpl)); auto& nameC2(this->getComponent<2>(__sysTpl)); auto& nameC3(this->getComponent<3>(__sysTpl));
#define SYSTEM_LOOP_6(nameEntity, nameC0, nameC1, nameC2, nameC3, nameC4) for(const auto& __sysTpl : this->getTuples()) { auto& nameEntity(this->getEntity(__sysTpl)); auto& nameC0(this->getComponent<0>(__sysTpl)); auto& nameC1(this->getComponent<1>(__sysTpl)); auto& nameC2(this->getComponent<2>(__sysTpl)); auto& nameC3(this->getComponent<3>(__sysTpl)); auto& nameC4(this->getComponent<4>(__sysTpl));
#define SYSTEM_LOOP_7(nameEntity, nameC0, nameC1, nameC2, nameC3, nameC4, nameC5) for(const auto& __sysTpl : this->getTuples()) { auto& nameEntity(this->getEntity(__sysTpl)); auto& nameC0(this->getComponent<0>(__sysTpl)); auto& nameC1(this->getComponent<1>(__sysTpl)); auto& nameC2(this->getComponent<2>(__sysTpl)); auto& nameC3(this->getComponent<3>(__sysTpl)); auto& nameC4(this->getComponent<4>(__sysTpl)); auto& nameC5(this->getComponent<5>(__sysTpl));
#define SYSTEM_LOOP_8(nameEntity, nameC0, nameC1, nameC2, nameC3, nameC4, nameC5, nameC6) for(const auto& __sysTpl : this->getTuples()) { auto& nameEntity(this->getEntity(__sysTpl)); auto& nameC0(this->getComponent<0>(__sysTpl)); auto& nameC1(this->getComponent<1>(__sysTpl)); auto& nameC2(this->getComponent<2>(__sysTpl)); auto& nameC3(this->getComponent<3>(__sysTpl)); auto& nameC4(this->getComponent<4>(__sysTpl)); auto& nameC5(this->getComponent<5>(__sysTpl)); auto& nameC6(this->getComponent<6>(__sysTpl));
#define SYSTEM_LOOP_9(nameEntity, nameC0, nameC1, nameC2, nameC3, nameC4, nameC5, nameC6, nameC7) for(const auto& __sysTpl : this->getTuples()) { auto& nameEntity(this->getEntity(__sysTpl)); auto& nameC0(this->getComponent<0>(__sysTpl)); auto& nameC1(this->getComponent<1>(__sysTpl)); auto& nameC2(this->getComponent<2>(__sysTpl)); auto& nameC3(this->getComponent<3>(__sysTpl)); auto& nameC4(this->getComponent<4>(__sysTpl)); auto& nameC5(this->getComponent<5>(__sysTpl)); auto& nameC6(this->getComponent<6>(__sysTpl)); auto& nameC7(this->getComponent<7>(__sysTpl));
#define SYSTEM_LOOP_10(nameEntity, nameC0, nameC1, nameC2, nameC3, nameC4, nameC5, nameC6, nameC7, nameC8) for(const auto& __sysTpl : this->getTuples()) { auto& nameEntity(this->getEntity(__sysTpl)); auto& nameC0(this->getComponent<0>(__sysTpl)); auto& nameC1(this->getComponent<1>(__sysTpl)); auto& nameC2(this->getComponent<2>(__sysTpl)); auto& nameC3(this->getComponent<3>(__sysTpl)); auto& nameC4(this->getComponent<4>(__sysTpl)); auto& nameC5(this->getComponent<5>(__sysTpl)); auto& nameC6(this->getComponent<6>(__sysTpl)); auto& nameC7(this->getComponent<7>(__sysTpl)); auto& nameC8(this->getComponent<8>(__sysTpl));
#define SYSTEM_LOOP_11(nameEntity, nameC0, nameC1, nameC2, nameC3, nameC4, nameC5, nameC6, nameC7, nameC8, nameC9) for(const auto& __sysTpl : this->getTuples()) { auto& nameEntity(this->getEntity(__sysTpl)); auto& nameC0(this->getComponent<0>(__sysTpl)); auto& nameC1(this->getComponent<1>(__sysTpl)); auto& nameC2(this->getComponent<2>(__sysTpl)); auto& nameC3(this->getComponent<3>(__sysTpl)); auto& nameC4(this->getComponent<4>(__sysTpl)); auto& nameC5(this->getComponent<5>(__sysTpl)); auto& nameC6(this->getComponent<6>(__sysTpl)); auto& nameC7(this->getComponent<7>(__sysTpl)); auto& nameC8(this->getComponent<8>(__sysTpl)); auto& nameC9(this->getComponent<9>(__sysTpl));
#define SYSTEM_LOOP_NOENTITY_0() for(const auto& __sysTpl : this->getTuples()) {
#define SYSTEM_LOOP_NOENTITY_1(nameC0) for(const auto& __sysTpl : this->getTuples()) { auto& nameC0(this->getComponent<0>(__sysTpl));
#define SYSTEM_LOOP_NOENTITY_2(nameC0, nameC1) for(const auto& __sysTpl : this->getTuples()) { auto& nameC0(this->getComponent<0>(__sysTpl)); auto& nameC1(this->getComponent<1>(__sysTpl));
#define SYSTEM_LOOP_NOENTITY_3(nameC0, nameC1, nameC2) for(const auto& __sysTpl : this->getTuples()) { auto& nameC0(this->getComponent<0>(__sysTpl)); auto& nameC1(this->getComponent<1>(__sysTpl)); auto& nameC2(this->getComponent<2>(__sysTpl));
#define SYSTEM_LOOP_NOENTITY_4(nameC0, nameC1, nameC2, nameC3) for(const auto& __sysTpl : this->getTuples()) { auto& nameC0(this->getComponent<0>(__sysTpl)); auto& nameC1(this->getComponent<1>(__sysTpl)); auto& nameC2(this->getComponent<2>(__sysTpl)); auto& nameC3(this->getComponent<3>(__sysTpl));
#define SYSTEM_LOOP_NOENTITY_5(nameC0, nameC1, nameC2, nameC3, nameC4) for(const auto& __sysTpl : this->getTuples()) { auto& nameC0(this->getComponent<0>(__sysTpl)); auto& nameC1(this->getComponent<1>(__sysTpl)); auto& nameC2(this->getComponent<2>(__sysTpl)); auto& nameC3(this->getComponent<3>(__sysTpl)); auto& nameC4(this->getComponent<4>(__sysTpl));
#define SYSTEM_LOOP_NOENTITY_6(nameC0, nameC1, nameC2, nameC3, nameC4, nameC5) for(const auto& __sysTpl : this->getTuples()) { auto& nameC0(this->getComponent<0>(__sysTpl)); auto& nameC1(this->getComponent<1>(__sysTpl)); auto& nameC2(this->getComponent<2>(__sysTpl)); auto& nameC3(this->getComponent<3>(__sysTpl)); auto& nameC4(this->getComponent<4>(__sysTpl)); auto& nameC5(this->getComponent<5>(__sysTpl));
#define SYSTEM_LOOP_NOENTITY_7(nameC0, nameC1, nameC2, nameC3, nameC4, nameC5, nameC6) for(const auto& __sysTpl : this->getTuples()) { auto& nameC0(this->getComponent<0>(__sysTpl)); auto& nameC1(this->getComponent<1>(__sysTpl)); auto& nameC2(this->getComponent<2>(__sysTpl)); auto& nameC3(this->getComponent<3>(__sysTpl)); auto& nameC4(this->getComponent<4>(__sysTpl)); auto& nameC5(this->getComponent<5>(__sysTpl)); auto& nameC6(this->getComponent<6>(__sysTpl));
#define SYSTEM_LOOP_NOENTITY_8(nameC0, nameC1, nameC2, nameC3, nameC4, nameC5, nameC6, nameC7) for(const auto& __sysTpl : this->getTuples()) { auto& nameC0(this->getComponent<0>(__sysTpl)); auto& nameC1(this->getComponent<1>(__sysTpl)); auto& nameC2(this->getComponent<2>(__sysTpl)); auto& nameC3(this->getComponent<3>(__sysTpl)); auto& nameC4(this->getComponent<4>(__sysTpl)); auto& nameC5(this->getComponent<5>(__sysTpl)); auto& nameC6(this->getComponent<6>(__sysTpl)); auto& nameC7(this->getComponent<7>(__sysTpl));
#define SYSTEM_LOOP_NOENTITY_9(nameC0, nameC1, nameC2, nameC3, nameC4, nameC5, nameC6, nameC7, nameC8) for(const auto& __sysTpl : this->getTuples()) { auto& nameC0(this->getComponent<0>(__sysTpl)); auto& nameC1(this->getComponent<1>(__sysTpl)); auto& nameC2(this->getComponent<2>(__sysTpl)); auto& nameC3(this->getComponent<3>(__sysTpl)); auto& nameC4(this->getComponent<4>(__sysTpl)); auto& nameC5(this->getComponent<5>(__sysTpl)); auto& nameC6(this->getComponent<6>(__sysTpl)); auto& nameC7(this->getComponent<7>(__sysTpl)); auto& nameC8(this->getComponent<8>(__sysTpl));
#define SYSTEM_LOOP_NOENTITY_10(nameC0, nameC1, nameC2, nameC3, nameC4, nameC5, nameC6, nameC7, nameC8, nameC9) for(const auto& __sysTpl : this->getTuples()) { auto& nameC0(this->getComponent<0>(__sysTpl)); auto& nameC1(this->getComponent<1>(__sysTpl)); auto& nameC2(this->getComponent<2>(__sysTpl)); auto& nameC3(this->getComponent<3>(__sysTpl)); auto& nameC4(this->getComponent<4>(__sysTpl)); auto& nameC5(this->getComponent<5>(__sysTpl)); auto& nameC6(this->getComponent<6>(__sysTpl)); auto& nameC7(this->getComponent<7>(__sysTpl)); auto& nameC8(this->getComponent<8>(__sysTpl)); auto& nameC9(this->getComponent<9>(__sysTpl));

#define SYSTEM_LOOP(...) MACRO_DISPATCH(SYSTEM_LOOP_, __VA_ARGS__)(__VA_ARGS__)
#define SYSTEM_LOOP_NOENTITY(...) MACRO_DISPATCH(SYSTEM_LOOP_NOENTITY_, __VA_ARGS__)(__VA_ARGS__)

#endif

/* Macro generator:

int main()
{
	constexpr int count{11};
	constexpr const char* macroName1{"SYSTEM_LOOP_"};
	constexpr const char* macroName2{"SYSTEM_LOOP_NOENTITY_"};
	constexpr const char* tplIdentifier{"__sysTpl"};

	std::string result;
	for(int i{0}; i < count; ++i)
	{
		// Macro name
		result += "#define " + macroName1 + ssvu::toStr(i + 1);

		// Parameters
		result += "(nameEntity";
		for(int p{0}; p < i; ++p) result += ", nameC" + ssvu::toStr(p);
		result += ") ";

		// Macro body
		result += "for(const auto& " + tplIdentifier + " : this->getTuples()) { ";
		result += "auto& nameEntity(this->getEntity(" + tplIdentifier + ")); ";
		for(int p{0}; p < i; ++p) result += "auto& nameC" + ssvu::toStr(p) + "(this->getComponent<" + ssvu::toStr(p) + ">(" + tplIdentifier + ")); ";

		result += "\n";
	}
	for(int i{0}; i < count; ++i)
	{
		// Macro name
		result += "#define " + macroName2 + ssvu::toStr(i);

		// Parameters
		result += "(";
		for(int p{0}; p < i; ++p)
		{
			result += "nameC" + ssvu::toStr(p);
			if(p < i - 1) result += ", ";
		}
		result += ") ";

		// Macro body
		result += "for(const auto& " + tplIdentifier + " : this->getTuples()) { ";
		for(int p{0}; p < i; ++p) result += "auto& nameC" + ssvu::toStr(p) + "(this->getComponent<" + ssvu::toStr(p) + ">(" + tplIdentifier + ")); ";

		result += "\n";
	}

	std::cout << result << std::endl;
	return 0;
}

*/
