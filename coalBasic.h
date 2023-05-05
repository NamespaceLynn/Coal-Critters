#pragma once

#include "coal.h"

namespace Tmpl8 {

	/* Moves towards player */
	/* Player takes damage on contact */
	/* "Becomes" a fireball when killed by a player's flame */
	class CoalBasic : public Coal
	{
	public:
		/* Constructor*/
		CoalBasic( std::shared_ptr<Sprite> sprite, vec2 centerPos,
				float hitBoxRadius, float speed );

		void Update(float deltaTime) override { /* UNIMPLEMENTED */ }
		void Update( float deltaTime, const Game& game );
		void Draw(Surface* screen) override;

		void Despawn() { despawn = true; updateFrame = false; }
		void SetDead() { dead = true; timeDead = 0.0f; invincible = true; }
		[[nodiscard]] bool IsDead() const { return dead; }
		[[nodiscard]] bool FinishedSpawning() const { return finishedSpawning; }
		[[nodiscard]] bool IsDespawning() const { return despawn; }
		[[nodiscard]] bool IsInvincible() const { return invincible; }
		[[nodiscard]] bool IsDespawned() const { return despawned; }

	private:
		/* The coal is invincible until it's spawning animation finishes */
		bool invincible{ true };
		/* True if the coal should "die" */
		bool dead{ false };
		float timeDead{ 0.0f };
		unsigned int hitColor{ 0xfd5f44 };
		/* True if the coal should go back into the ground again */
		bool despawn{ false };
		/* True when the coal is done despawning */
		bool despawned{ false };
		bool updateFrame{ false };
		bool finishedSpawning{ false };
		float timeSinceFrameUpdate{ 0.0f };
		static constexpr  float timeBetweenFrameUpdates{ 0.2f };
		/* Amount of frames the death animation has */
		static constexpr int deathFrames{ 2 };
	};
}