#pragma once

#include "entity.h"

namespace Tmpl8 {

	/* Parent class for coal enemies */
	class Game;
	class Coal : public Entity
	{
	public:
		void DrawShadow( Surface* screen ) override;

	protected:

		/* Constructor */
		Coal( std::shared_ptr<Sprite> sprite, vec2 centerPos,
			float hitBoxRadius, float speed );

		vec2 ApplySeparation( float deltaTime, const Game& game );
		void CheckWallCollision();

		/* Variables */
		unsigned int frame{ 0 };
		static constexpr float separationPower{ 500.0f };
		static constexpr float separationRadius{ 50.0f };
		static constexpr float maxSeparationDist{ 50.0f };
	};
}