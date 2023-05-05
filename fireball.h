#pragma once

#include "entity.h"
#include <array>

using std::shared_ptr;

namespace Tmpl8 {

	/* Instantiated when a CoalBasic object is hit by a Flame */
	class Fireball : public Entity
	{
	public:
		/* Constructor */
		Fireball( std::shared_ptr<Sprite> sprite, vec2 centerPos,
			vec2 dir, float hitBoxRadius, float speed );

		void Update( float deltaTime ) override;
		void Draw(Surface* screen) override;
		/* Checks collision with the edges of the game window */
		void CheckWallCollision();

		void SetErase() { toBeErased = true; }
		[[nodiscard]] bool IsToBeErased() const { return toBeErased; }
		[[nodiscard]] bool IsHittingWall() const { return justHitWall; }

	private:

		static constexpr float speedMultiplier{ 5.0f };

		/* Holds the previous' frames' positions */
		std::array<vec2, 12> previousPos;
		/* DeltaTime used in some calculations */
		float dt{ 0.0f };

		/* Used to erase and delete Fireballs in collisionManager.cpp */
		bool toBeErased{ false };
		bool justHitWall{ false };
	};
}