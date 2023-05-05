#pragma once

#include "coal.h"

namespace Tmpl8 {

	enum class Movement
	{
		NONE = 0,
		HORIZONTAL = 1,
		VERTICAL = 2,
	};

	/* If touched by the player, all enemies self destruct */
	/* Nothing happens if it gets destroyed by a flame, fireball or explosion */
	class CoalGold : public Coal
	{
	public:
		/* Constructor*/
		CoalGold( std::shared_ptr<Sprite> sprite, vec2 centerPos,
				float hitBoxRadius, float speed );

		void Update( float deltaTime ) override;
		void Draw(Surface* screen) override;
		/* "Replace the previous' position and direction */
		/* Used when a "new" (same object) gold coal is created */
		void Replace(vec2 centerPos, vec2 dir);

		void SetActive( bool state ) { active = state; }
		[[nodiscard]] bool IsActive() const { return active; }

	private:

		bool active{ false };
		float activeTimer{ 0.0f };

		Movement moveDir{ Movement::NONE };
		
		/* How many times faster the gold coal should be relative */
		/* to other coal enemies */
		const float speedMultiplier{ 4.0f };
	};
}