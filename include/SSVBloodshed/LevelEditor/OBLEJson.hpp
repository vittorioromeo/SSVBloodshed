// Copyright (c) 2013-2015 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_LEVELEDITOR_JSON
#define SSVOB_LEVELEDITOR_JSON

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/LevelEditor/OBLEPack.hpp"
#include "SSVBloodshed/LevelEditor/OBLESector.hpp"
#include "SSVBloodshed/LevelEditor/OBLELevel.hpp"
#include "SSVBloodshed/LevelEditor/OBLETile.hpp"

SSVJ_CNV_NAMESPACE()
{
	template<> SSVJ_CNV(ob::OBLETileData, mV, mX)	{ ssvj::cnvArr(mV, mX.x, mX.y, mX.z, mX.type, mX.params); }				SSVJ_CNV_END()
	template<> SSVJ_CNV(ob::OBLETile, mV, mX)		{ ssvj::cnv(mV, mX.data); }												SSVJ_CNV_END()
	template<> SSVJ_CNV(ob::OBLELevel, mV, mX)		{ ssvj::cnvArr(mV, mX.x, mX.y, mX.cols, mX.rows, mX.depth, mX.tiles); }	SSVJ_CNV_END()
	template<> SSVJ_CNV(ob::OBLESector, mV, mX)		{ ssvj::cnv(mV, mX.levels); }											SSVJ_CNV_END()
	template<> SSVJ_CNV(ob::OBLEPack, mV, mX)		{ ssvj::cnvArr(mV, mX.name, mX.sectors); }								SSVJ_CNV_END()
}
SSVJ_CNV_NAMESPACE_END()

#endif
