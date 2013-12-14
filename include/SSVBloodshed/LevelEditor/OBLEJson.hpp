// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_LEVELEDITOR_JSON
#define SSVOB_LEVELEDITOR_JSON

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/LevelEditor/OBLEPack.hpp"
#include "SSVBloodshed/LevelEditor/OBLESector.hpp"
#include "SSVBloodshed/LevelEditor/OBLELevel.hpp"
#include "SSVBloodshed/LevelEditor/OBLETile.hpp"

namespace ssvuj
{
	template<> SSVUJ_CNV_SIMPLE(ob::OBLETType, mObj, mV)	{ ssvuj::convertEnum<ob::OBLETType, int>(mObj, mV); }								SSVUJ_CNV_SIMPLE_END();
	template<> SSVUJ_CNV_SIMPLE(ob::OBLETile, mObj, mV)		{ ssvuj::convertArray(mObj, mV.x, mV.y, mV.z, mV.type, mV.params); }				SSVUJ_CNV_SIMPLE_END();
	template<> SSVUJ_CNV_SIMPLE(ob::OBLELevel, mObj, mV)	{ ssvuj::convertArray(mObj, mV.x, mV.y, mV.cols, mV.rows, mV.depth, mV.tiles); }	SSVUJ_CNV_SIMPLE_END();
	template<> SSVUJ_CNV_SIMPLE(ob::OBLESector, mObj, mV)	{ ssvuj::convert(mObj, mV.levels); }												SSVUJ_CNV_SIMPLE_END();
	template<> SSVUJ_CNV_SIMPLE(ob::OBLEPack, mObj, mV)		{ ssvuj::convertArray(mObj, mV.name, mV.sectors); }									SSVUJ_CNV_SIMPLE_END();

}

#endif
