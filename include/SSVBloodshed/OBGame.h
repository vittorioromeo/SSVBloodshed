// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_GAME
#define SSVOB_GAME

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBAssets.h"
#include "SSVBloodshed/OBConfig.h"
#include "SSVBloodshed/OBFactory.h"

namespace ob
{
	class Particle : public sf::Transformable, public sf::Drawable
	{
		private:
			sf::VertexArray vertices{sf::PrimitiveType::Quads};
			Vec2f velocity;
			float acceleration{0.f};
			sf::Color color;
			float size{1.f}, life{100.f}, lifeMax{100.f};
			float opacityMult{1.f};

			inline void refreshColor() noexcept { for(auto i(0u); i < vertices.getVertexCount(); ++i) vertices[i].color = color; }

		public:
			inline Particle()
			{
				vertices.append({Vec2f(-size, -size), sf::Color::White});
				vertices.append({Vec2f(size, -size), sf::Color::White});
				vertices.append({Vec2f(size, size), sf::Color::White});
				vertices.append({Vec2f(-size, size), sf::Color::White});
			}

			inline void update(float mFrameTime)
			{
				if(life > 0) life -= mFrameTime;
				color.a = static_cast<unsigned char>(ssvu::getClamped(life * (255 / lifeMax) * opacityMult, 0.f, 255.f));
				refreshColor();
				velocity.x = ssvu::getClampedMin(std::abs(velocity.x) + acceleration * mFrameTime, 0.f) * ssvu::getSign(velocity.x);
				velocity.y = ssvu::getClampedMin(std::abs(velocity.y) + acceleration * mFrameTime, 0.f) * ssvu::getSign(velocity.y);
				setPosition(getPosition() + velocity * mFrameTime);
			}

			inline void draw(sf::RenderTarget& mRenderTarget, sf::RenderStates mRenderStates) const override
			{
				mRenderStates.transform *= getTransform();
				mRenderTarget.draw(vertices, mRenderStates);
			}

			inline void setColor(const sf::Color& mColor) noexcept		{ color = mColor; }
			inline void setVelocity(const Vec2f& mVelocity) noexcept	{ velocity = mVelocity; }
			inline void setAcceleration(float mAcceleration) noexcept	{ acceleration = mAcceleration; }
			inline void setLife(float mLife) noexcept					{ life = lifeMax = mLife; }
			inline void setOpacityMult(float mOpacityMult) noexcept		{ opacityMult = mOpacityMult; }
			inline void setSize(float mSize) noexcept
			{
				size = mSize;
				vertices[0].position = Vec2f(-size, -size);
				vertices[1].position = Vec2f(size, -size);
				vertices[2].position = Vec2f(size, size);
				vertices[3].position = Vec2f(-size, size);
			}

			inline float getLife() const noexcept { return life; }
	};

	class ParticleSystem : public sf::Drawable
	{
		private:
			std::vector<Particle> particles;

		public:
			inline void create(const Vec2f& mPosition, const Vec2f& mVelocity, float mAcceleration, const sf::Color& mColor, float mSize, float mLife)
			{
				Particle p;
				p.setPosition(mPosition);
				p.setVelocity(mVelocity);
				p.setAcceleration(mAcceleration);
				p.setColor(mColor);
				p.setSize(mSize);
				p.setLife(mLife);

				particles.push_back(p);
			}
			inline void createBlood(const Vec2f& mPosition)
			{
				Particle p;
				p.setPosition(mPosition);
				p.setVelocity(ssvs::getVecFromDegrees(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(0.f, 15.f)));
				p.setAcceleration(ssvu::getRndR<float>(-11.f, -9.f) * 0.09f);
				p.setColor(sf::Color{ssvu::getRnd<unsigned char>(185, 255), 0, 0, 255});
				p.setSize(1.f + ssvu::getRndR<float>(-0.3, 0.3));
				p.setLife(100 + ssvu::getRnd(-50, 50));
				p.setOpacityMult(0.5f);

				particles.push_back(p);
			}
			inline void createGib(const Vec2f& mPosition)
			{
				Particle p;
				p.setPosition(mPosition);
				p.setVelocity(ssvs::getVecFromDegrees(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(5.f, 15.f)));
				p.setAcceleration(ssvu::getRndR<float>(-11.f, -9.f) * 0.05f);
				p.setColor(sf::Color{ssvu::getRnd<unsigned char>(95, 170), 15, 15, 255});
				p.setSize(1.1f + ssvu::getRndR<float>(-0.3, 0.3));
				p.setLife(150 + ssvu::getRnd(-50, 50));
				p.setOpacityMult(1.5f);

				particles.push_back(p);
			}
			inline void createDebris(const Vec2f& mPosition)
			{
				Particle p;
				p.setPosition(mPosition);
				p.setVelocity(ssvs::getVecFromDegrees(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(0.f, 9.f)));
				p.setAcceleration(ssvu::getRndR<float>(-11.f, -9.f) * 0.09f);
				p.setColor(sf::Color::Black);
				p.setSize(1.f + ssvu::getRndR<float>(-0.3, 0.3));
				p.setLife(65 + ssvu::getRnd(-50, 50));

				particles.push_back(p);
			}

			inline void update(float mFrameTime)
			{
				ssvu::eraseRemoveIf(particles, [](const Particle& mParticle){ return mParticle.getLife() <= 0; });
				for(auto& p : particles) p.update(mFrameTime);
			}

			inline void draw(sf::RenderTarget& mRenderTarget, sf::RenderStates mRenderStates) const override
			{
				for(const auto& p : particles) mRenderTarget.draw(p, mRenderStates);
			}

			inline void clear() { particles.clear(); }
	};

	struct ParticleTextureComponent : public sses::Component
	{
		private:
			sf::RenderTexture& renderTexture;
			sf::Sprite sprite;
			sf::RenderTarget& renderTarget;
			bool clearOnDraw{false};
			unsigned char opacity{255};

		public:
			ParticleTextureComponent(sf::RenderTexture& mRenderTexture, sf::RenderTarget& mRenderTarget, bool mClearOnDraw = false, unsigned char mOpacity = 255)
				: renderTexture(mRenderTexture), renderTarget(mRenderTarget), clearOnDraw{mClearOnDraw}, opacity{mOpacity} { }

			inline void init() override { renderTexture.clear(sf::Color::Transparent); }
			inline void draw() override
			{
				renderTexture.display();
				sprite.setTexture(renderTexture.getTexture());
				sprite.setColor({255, 255, 255, opacity});
				renderTarget.draw(sprite);

				if(clearOnDraw) renderTexture.clear(sf::Color::Transparent);
			}

			inline void render(const sf::Drawable& mDrawable) { renderTexture.draw(mDrawable); }
	};





	class OBGame
	{
		private:
			ssvs::GameWindow& gameWindow;
			OBAssets& assets;
			ssvs::Camera camera{gameWindow, 2.f};
			OBFactory factory;
			ssvs::GameState gameState;
			ssvsc::World world;
			sses::Manager manager;
			ssvs::BitmapText debugText;
			//ssvu::TimelineManager timelineManager;

			bool inputShoot{false}, inputSwitch{false};
			int inputX{0}, inputY{0};

		public:
			sf::RenderTexture permanentParticles, tempParticles;
			ParticleSystem bloodParticleSystem, debrisParticleSystem, gibParticleSystem;
			sses::Entity* bloodParticles;
			sses::Entity* debrisParticles;
			sses::Entity* gibParticles;

			OBGame(ssvs::GameWindow& mGameWindow, OBAssets& mAssets);

			void newGame();

			void update(float mFrameTime);
			void updateDebugText(float mFrameTime);
			void draw();

			inline void render(const sf::Drawable& mDrawable) { gameWindow.draw(mDrawable); }

			inline Vec2i getMousePosition() const		{ return toCoords(camera.getMousePosition()); }
			inline ssvs::GameWindow& getGameWindow()	{ return gameWindow; }
			inline OBAssets& getAssets()				{ return assets; }
			inline OBFactory& getFactory()				{ return factory; }
			inline ssvs::GameState& getGameState()		{ return gameState; }
			inline ssvsc::World& getWorld()				{ return world; }
			inline sses::Manager& getManager()			{ return manager; }

			inline bool getIShoot() const noexcept		{ return inputShoot; }
			inline bool getISwitch() const noexcept		{ return inputSwitch; }
			inline int getIX() const noexcept			{ return inputX; }
			inline int getIY() const noexcept			{ return inputY; }
	};
}

#endif
