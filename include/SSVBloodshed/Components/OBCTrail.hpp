// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_TRAIL
#define SSVOB_COMPONENTS_TRAIL

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBGame.hpp"

namespace ob
{
	class OBCTrail : public sses::Component
	{
		private:
			OBGame& game;
			float life{75};
			Vec2f a, b;
			sf::Color color;
			ssvs::VertexVector<sf::PrimitiveType::Lines> vertices;

		public:
			OBCTrail(OBGame& mGame, const Vec2i& mA, const Vec2i& mB, sf::Color mColor) : game(mGame), a{toPixels(mA)}, b{toPixels(mB)}, color{std::move(mColor)}, vertices{2} { }

			inline void update(FT mFT) override
			{
				life -= mFT;
				if(life <= 0) getEntity().destroy();
				color.a = life * (255 / 100);
				vertices[0].color = vertices[1].color = color;
				vertices[0].position = a + Vec2f(ssvu::getRnd(-1, 1), ssvu::getRnd(-1, 1));
				vertices[1].position = b + Vec2f(ssvu::getRnd(-1, 1), ssvu::getRnd(-1, 1));
			}
			inline void draw() override { game.render(vertices); }
	};
}

#endif
