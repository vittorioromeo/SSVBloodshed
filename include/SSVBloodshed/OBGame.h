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
	class Particle
	{
		private:
			sf::Color color;
			Vec2f position, velocity;
			float acceleration{0.f}, size{1.f}, life{100.f}, lifeMax{100.f}, opacityMult{1.f};

		public:
			inline void update(float mFrameTime)
			{
				life -= mFrameTime;
				color.a = static_cast<unsigned char>(ssvu::getClamped(life * (255.f / lifeMax) * opacityMult, 0.f, 255.f));
				velocity *= acceleration;
				position += velocity * mFrameTime;
			}

			inline void setPosition(const Vec2f& mPosition) noexcept	{ position = mPosition; }
			inline void setColor(const sf::Color& mColor) noexcept		{ color = mColor; }
			inline void setVelocity(const Vec2f& mVelocity) noexcept	{ velocity = mVelocity; }
			inline void setAcceleration(float mAcceleration) noexcept	{ acceleration = mAcceleration; }
			inline void setLife(float mLife) noexcept					{ life = lifeMax = mLife; }
			inline void setOpacityMult(float mOpacityMult) noexcept		{ opacityMult = mOpacityMult; }
			inline void setSize(float mSize) noexcept					{ size = mSize; }

			inline float getLife() const noexcept						{ return life; }
			inline float getSize() const noexcept						{ return size; }
			inline const Vec2f& getPosition() const noexcept			{ return position; }
			inline const sf::Color& getColor() const noexcept			{ return color; }
	};

	class ParticleSystem : public sf::Drawable
	{
		private:
			sf::VertexArray vertices{sf::PrimitiveType::Quads};
			std::vector<Particle> particles;

		public:
			inline ParticleSystem() { particles.reserve(350); }

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
				p.setAcceleration(0.9f);
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
				p.setAcceleration(0.93f);
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
				p.setAcceleration(0.9f);
				p.setColor(sf::Color::Black);
				p.setSize(1.f + ssvu::getRndR<float>(-0.3, 0.3));
				p.setLife(65 + ssvu::getRnd(-50, 50));
				p.setOpacityMult(0.8f);

				particles.push_back(p);
			}
			inline void createMuzzle(const Vec2f& mPosition)
			{
				Particle p;
				p.setPosition(mPosition);
				p.setVelocity(ssvs::getVecFromDegrees(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(1.f, 4.5f)));
				p.setAcceleration(0.9f);
				p.setColor(sf::Color{255, ssvu::getRnd<unsigned char>(95, 100), 15, 255});
				p.setSize(1.1f + ssvu::getRndR<float>(-0.3, 0.3));
				p.setLife(6 + ssvu::getRnd(-5, 5));

				particles.push_back(p);
			}

			inline void update(float mFrameTime)
			{
				ssvu::eraseRemoveIf(particles, [](const Particle& mParticle){ return mParticle.getLife() <= 0; });
				vertices.clear();
				for(auto& p : particles)
				{
					p.update(mFrameTime);
					vertices.append({Vec2f(p.getPosition().x - p.getSize(), p.getPosition().y - p.getSize()), p.getColor()});
					vertices.append({Vec2f(p.getPosition().x + p.getSize(), p.getPosition().y - p.getSize()), p.getColor()});
					vertices.append({Vec2f(p.getPosition().x + p.getSize(), p.getPosition().y + p.getSize()), p.getColor()});
					vertices.append({Vec2f(p.getPosition().x - p.getSize(), p.getPosition().y + p.getSize()), p.getColor()});
				}
			}

			inline void draw(sf::RenderTarget& mRenderTarget, sf::RenderStates mRenderStates) const override { mRenderTarget.draw(vertices, mRenderStates); }
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

			inline void init() override
			{
				renderTexture.clear(sf::Color::Transparent);
				sprite.setTexture(renderTexture.getTexture());
				sprite.setColor({255, 255, 255, opacity});
			}

			inline void draw() override
			{
				renderTexture.display();
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
			ParticleSystem permanentParticleSystem, tempParticleSystem;
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
