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

SSVJ_CNV_TO_VAL(ob::OBLETile, data)
SSVJ_CNV_TO_VAL(ob::OBLESector, levels)

SSVJ_CNV_TO_ARR(ob::OBLETileData, x, y, z, type, params)
SSVJ_CNV_TO_ARR(ob::OBLELevel, x, y, cols, rows, depth, tiles)
SSVJ_CNV_TO_ARR(ob::OBLEPack, name, sectors)

#endif
