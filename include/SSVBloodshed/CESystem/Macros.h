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

#define SYSTEM_LOOP_1(nameEntity) for(const auto& tpl : getTuples()) { auto& nameEntity(getEntity(tpl));
#define SYSTEM_LOOP_NOENTITY_0() for(const auto& tpl : getTuples()) {
#define SYSTEM_LOOP_2(nameEntity, nameC0) for(const auto& tpl : getTuples()) { auto& nameEntity(getEntity(tpl)); auto& nameC0(getComponent<0>(tpl));
#define SYSTEM_LOOP_NOENTITY_1(nameC0) for(const auto& tpl : getTuples()) { auto& nameC0(getComponent<0>(tpl));
#define SYSTEM_LOOP_3(nameEntity, nameC0, nameC1) for(const auto& tpl : getTuples()) { auto& nameEntity(getEntity(tpl)); auto& nameC0(getComponent<0>(tpl)); auto& nameC1(getComponent<1>(tpl));
#define SYSTEM_LOOP_NOENTITY_2(nameC0, nameC1) for(const auto& tpl : getTuples()) { auto& nameC0(getComponent<0>(tpl)); auto& nameC1(getComponent<1>(tpl));
#define SYSTEM_LOOP_4(nameEntity, nameC0, nameC1, nameC2) for(const auto& tpl : getTuples()) { auto& nameEntity(getEntity(tpl)); auto& nameC0(getComponent<0>(tpl)); auto& nameC1(getComponent<1>(tpl)); auto& nameC2(getComponent<2>(tpl));
#define SYSTEM_LOOP_NOENTITY_3(nameC0, nameC1, nameC2) for(const auto& tpl : getTuples()) { auto& nameC0(getComponent<0>(tpl)); auto& nameC1(getComponent<1>(tpl)); auto& nameC2(getComponent<2>(tpl));
#define SYSTEM_LOOP_5(nameEntity, nameC0, nameC1, nameC2, nameC3) for(const auto& tpl : getTuples()) { auto& nameEntity(getEntity(tpl)); auto& nameC0(getComponent<0>(tpl)); auto& nameC1(getComponent<1>(tpl)); auto& nameC2(getComponent<2>(tpl)); auto& nameC3(getComponent<3>(tpl));
#define SYSTEM_LOOP_NOENTITY_4(nameC0, nameC1, nameC2, nameC3) for(const auto& tpl : getTuples()) { auto& nameC0(getComponent<0>(tpl)); auto& nameC1(getComponent<1>(tpl)); auto& nameC2(getComponent<2>(tpl)); auto& nameC3(getComponent<3>(tpl));
#define SYSTEM_LOOP_6(nameEntity, nameC0, nameC1, nameC2, nameC3, nameC4) for(const auto& tpl : getTuples()) { auto& nameEntity(getEntity(tpl)); auto& nameC0(getComponent<0>(tpl)); auto& nameC1(getComponent<1>(tpl)); auto& nameC2(getComponent<2>(tpl)); auto& nameC3(getComponent<3>(tpl)); auto& nameC4(getComponent<4>(tpl));
#define SYSTEM_LOOP_NOENTITY_5(nameC0, nameC1, nameC2, nameC3, nameC4) for(const auto& tpl : getTuples()) { auto& nameC0(getComponent<0>(tpl)); auto& nameC1(getComponent<1>(tpl)); auto& nameC2(getComponent<2>(tpl)); auto& nameC3(getComponent<3>(tpl)); auto& nameC4(getComponent<4>(tpl));
#define SYSTEM_LOOP_7(nameEntity, nameC0, nameC1, nameC2, nameC3, nameC4, nameC5) for(const auto& tpl : getTuples()) { auto& nameEntity(getEntity(tpl)); auto& nameC0(getComponent<0>(tpl)); auto& nameC1(getComponent<1>(tpl)); auto& nameC2(getComponent<2>(tpl)); auto& nameC3(getComponent<3>(tpl)); auto& nameC4(getComponent<4>(tpl)); auto& nameC5(getComponent<5>(tpl));
#define SYSTEM_LOOP_NOENTITY_6(nameC0, nameC1, nameC2, nameC3, nameC4, nameC5) for(const auto& tpl : getTuples()) { auto& nameC0(getComponent<0>(tpl)); auto& nameC1(getComponent<1>(tpl)); auto& nameC2(getComponent<2>(tpl)); auto& nameC3(getComponent<3>(tpl)); auto& nameC4(getComponent<4>(tpl)); auto& nameC5(getComponent<5>(tpl));
#define SYSTEM_LOOP_8(nameEntity, nameC0, nameC1, nameC2, nameC3, nameC4, nameC5, nameC6) for(const auto& tpl : getTuples()) { auto& nameEntity(getEntity(tpl)); auto& nameC0(getComponent<0>(tpl)); auto& nameC1(getComponent<1>(tpl)); auto& nameC2(getComponent<2>(tpl)); auto& nameC3(getComponent<3>(tpl)); auto& nameC4(getComponent<4>(tpl)); auto& nameC5(getComponent<5>(tpl)); auto& nameC6(getComponent<6>(tpl));
#define SYSTEM_LOOP_NOENTITY_7(nameC0, nameC1, nameC2, nameC3, nameC4, nameC5, nameC6) for(const auto& tpl : getTuples()) { auto& nameC0(getComponent<0>(tpl)); auto& nameC1(getComponent<1>(tpl)); auto& nameC2(getComponent<2>(tpl)); auto& nameC3(getComponent<3>(tpl)); auto& nameC4(getComponent<4>(tpl)); auto& nameC5(getComponent<5>(tpl)); auto& nameC6(getComponent<6>(tpl));
#define SYSTEM_LOOP_9(nameEntity, nameC0, nameC1, nameC2, nameC3, nameC4, nameC5, nameC6, nameC7) for(const auto& tpl : getTuples()) { auto& nameEntity(getEntity(tpl)); auto& nameC0(getComponent<0>(tpl)); auto& nameC1(getComponent<1>(tpl)); auto& nameC2(getComponent<2>(tpl)); auto& nameC3(getComponent<3>(tpl)); auto& nameC4(getComponent<4>(tpl)); auto& nameC5(getComponent<5>(tpl)); auto& nameC6(getComponent<6>(tpl)); auto& nameC7(getComponent<7>(tpl));
#define SYSTEM_LOOP_NOENTITY_8(nameC0, nameC1, nameC2, nameC3, nameC4, nameC5, nameC6, nameC7) for(const auto& tpl : getTuples()) { auto& nameC0(getComponent<0>(tpl)); auto& nameC1(getComponent<1>(tpl)); auto& nameC2(getComponent<2>(tpl)); auto& nameC3(getComponent<3>(tpl)); auto& nameC4(getComponent<4>(tpl)); auto& nameC5(getComponent<5>(tpl)); auto& nameC6(getComponent<6>(tpl)); auto& nameC7(getComponent<7>(tpl));
#define SYSTEM_LOOP_10(nameEntity, nameC0, nameC1, nameC2, nameC3, nameC4, nameC5, nameC6, nameC7, nameC8) for(const auto& tpl : getTuples()) { auto& nameEntity(getEntity(tpl)); auto& nameC0(getComponent<0>(tpl)); auto& nameC1(getComponent<1>(tpl)); auto& nameC2(getComponent<2>(tpl)); auto& nameC3(getComponent<3>(tpl)); auto& nameC4(getComponent<4>(tpl)); auto& nameC5(getComponent<5>(tpl)); auto& nameC6(getComponent<6>(tpl)); auto& nameC7(getComponent<7>(tpl)); auto& nameC8(getComponent<8>(tpl));
#define SYSTEM_LOOP_NOENTITY_9(nameC0, nameC1, nameC2, nameC3, nameC4, nameC5, nameC6, nameC7, nameC8) for(const auto& tpl : getTuples()) { auto& nameC0(getComponent<0>(tpl)); auto& nameC1(getComponent<1>(tpl)); auto& nameC2(getComponent<2>(tpl)); auto& nameC3(getComponent<3>(tpl)); auto& nameC4(getComponent<4>(tpl)); auto& nameC5(getComponent<5>(tpl)); auto& nameC6(getComponent<6>(tpl)); auto& nameC7(getComponent<7>(tpl)); auto& nameC8(getComponent<8>(tpl));
#define SYSTEM_LOOP_11(nameEntity, nameC0, nameC1, nameC2, nameC3, nameC4, nameC5, nameC6, nameC7, nameC8, nameC9) for(const auto& tpl : getTuples()) { auto& nameEntity(getEntity(tpl)); auto& nameC0(getComponent<0>(tpl)); auto& nameC1(getComponent<1>(tpl)); auto& nameC2(getComponent<2>(tpl)); auto& nameC3(getComponent<3>(tpl)); auto& nameC4(getComponent<4>(tpl)); auto& nameC5(getComponent<5>(tpl)); auto& nameC6(getComponent<6>(tpl)); auto& nameC7(getComponent<7>(tpl)); auto& nameC8(getComponent<8>(tpl)); auto& nameC9(getComponent<9>(tpl));
#define SYSTEM_LOOP_NOENTITY_10(nameC0, nameC1, nameC2, nameC3, nameC4, nameC5, nameC6, nameC7, nameC8, nameC9) for(const auto& tpl : getTuples()) { auto& nameC0(getComponent<0>(tpl)); auto& nameC1(getComponent<1>(tpl)); auto& nameC2(getComponent<2>(tpl)); auto& nameC3(getComponent<3>(tpl)); auto& nameC4(getComponent<4>(tpl)); auto& nameC5(getComponent<5>(tpl)); auto& nameC6(getComponent<6>(tpl)); auto& nameC7(getComponent<7>(tpl)); auto& nameC8(getComponent<8>(tpl)); auto& nameC9(getComponent<9>(tpl));

#define SYSTEM_LOOP(...) MACRO_DISPATCH(SYSTEM_LOOP_, __VA_ARGS__)(__VA_ARGS__)
#define SYSTEM_LOOP_NOENTITY(...) MACRO_DISPATCH(SYSTEM_LOOP_NOENTITY_, __VA_ARGS__)(__VA_ARGS__)

#endif

/* Macro generator:

int main()
{
	std::string result;
	for(int i{0}; i < 11; ++i)
	{
		// Macro name
		result += "#define SYSTEM_LOOP_" + ssvu::toStr(i + 1);

		// Parameters
		result += "(nameEntity";
		for(int p{0}; p < i; ++p) result += ", nameC" + ssvu::toStr(p);
		result += ") ";

		// Macro body
		result += "for(const auto& tpl : getTuples()) { ";
		result += "auto& nameEntity(getEntity(tpl)); ";
		for(int p{0}; p < i; ++p) result += "auto& nameC" + ssvu::toStr(p) + "(getComponent<" + ssvu::toStr(p) + ">(tpl)); ";

		result += "\n";

		// Macro name
		result += "#define SYSTEM_LOOP_NOENTITY_" + ssvu::toStr(i + 1);

		// Parameters
		result += "(";
		for(int p{0}; p < i; ++p)
		{
			result += "nameC" + ssvu::toStr(p);
			if(p < i - 1) result += ", ";
		}
		result += ") ";

		// Macro body
		result += "for(const auto& tpl : getTuples()) { ";
		for(int p{0}; p < i; ++p) result += "auto& nameC" + ssvu::toStr(p) + "(getComponent<" + ssvu::toStr(p) + ">(tpl)); ";

		result += "\n";
	}


	std::cout << result << std::endl;

	return 0;
}

*/
