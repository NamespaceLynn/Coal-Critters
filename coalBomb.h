#pragma once

#include "coal.h"

namespace Tmpl8 {

	/* Moves towards player */
	/* Explodes when close to the player */
	/* Explodes when hit by fireball or flame */
	class CoalBomb : public Coal
	{
	public:
		/* Constructor */
		CoalBomb( std::shared_ptr<Sprite> sprite, vec2 centerPos,
				float hitBoxRadius, float speed );

		void Update( float deltaTime ) override { /* UNIMPLEMENTED */ }
		void Update( float deltaTime, const Game& game );
		void Draw( Surface* screen ) override;
		void DrawHitBox(Surface* screen) const override;

		void Despawn() { despawn = true; updateFrame = false; }
		[[nodiscard]] bool FinishedSpawning() const { return finishedSpawning; }
		[[nodiscard]] bool IsDespawning() const { return despawn; }
		[[nodiscard]] bool IsInvincible() const { return invincible; }
		[[nodiscard]] bool IsDespawned() const { return despawned; }

	private:
		/* The coal is invincible until it's spawning animation finishes */
		bool invincible{ true };
		/* True if the coal should go back into the ground again */
		bool despawn{ false };
		/* True when the coal is done despawning */
		bool despawned{ false };
		bool updateFrame{ false };
		bool finishedSpawning{ false };
		float timeSinceFrameUpdate{ 0.0f };
		static constexpr  float timeBetweenFrameUpdates{ 0.2f };
		
		static constexpr float explodeDist{ 32.0f };
	};
}