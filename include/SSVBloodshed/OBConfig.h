// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_CONFIG
#define SSVOB_CONFIG

#include "SSVBloodshed/OBCommon.h"

namespace ob
{
	class OBConfig
	{
		private:
			bool soundEnabled{true}, musicEnabled{true};
			inline static OBConfig& getInstance() noexcept { static OBConfig instance; return instance; }

		public:
			inline static bool isSoundEnabled() noexcept { return getInstance().soundEnabled; }
			inline static bool isMusicEnabled() noexcept { return getInstance().musicEnabled; }
	};
}

#endif
