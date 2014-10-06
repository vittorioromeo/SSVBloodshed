// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_LEVELEDITOR_JSON
#define SSVOB_LEVELEDITOR_JSON

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/LevelEditor/OBLEPack.hpp"
#include "SSVBloodshed/LevelEditor/OBLESector.hpp"
#include "SSVBloodshed/LevelEditor/OBLELevel.hpp"
#include "SSVBloodshed/LevelEditor/OBLETile.hpp"

// TODO: plox
namespace ssvu
{
	namespace Json
	{
		namespace Internal
		{
			template<> SSVJ_CNV_SIMPLE(ob::OBLETileData, mObj, mV)	{ ssvj::convertArr(mObj, mV.x, mV.y, mV.z, mV.type, mV.params); }				SSVJ_CNV_END();
			template<> SSVJ_CNV_SIMPLE(ob::OBLETile, mObj, mV)		{ ssvj::convert(mObj, mV.data); }												SSVJ_CNV_END();
			template<> SSVJ_CNV_SIMPLE(ob::OBLELevel, mObj, mV)		{ ssvj::convertArr(mObj, mV.x, mV.y, mV.cols, mV.rows, mV.depth, mV.tiles); }	SSVJ_CNV_END();
			template<> SSVJ_CNV_SIMPLE(ob::OBLESector, mObj, mV)	{ ssvj::convert(mObj, mV.levels); }												SSVJ_CNV_END();
			template<> SSVJ_CNV_SIMPLE(ob::OBLEPack, mObj, mV)		{ ssvj::convertArr(mObj, mV.name, mV.sectors); }								SSVJ_CNV_END();
		}
	}
}

#endif
