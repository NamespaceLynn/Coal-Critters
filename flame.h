#pragma once

#include "entity.h"

namespace Tmpl8 {

	/* Projectile created on player-input (space bar) */
	class Flame : public Entity
	{
	public:
		/* Constructor */
		Flame( std::shared_ptr<Sprite> sprite, vec2 centerPos,
				float hitBoxRadius, float speed );

		Flame( Flame&& ) = default;
		Flame& operator=( Flame&& ) = default;

		void Update( float deltaTime ) override;
		void Draw( Surface* screen ) override;
		void DrawHitBox( Surface* screen ) const override;

		/* Updates the direction if the current frame was updated */
		void UpdateDirection();

		void SetActive( bool state ) { active = state; }
		[[nodiscard]] bool IsActive() const { return active; }
		[[nodiscard]] vec2 GetDir() const { return dir; }
		[[nodiscard]] vec2 GetHitBoxPos() const { return centerPos + (dir * 7); }
		[[nodiscard]] Circle GetCircle() const override { return { hitBoxPos, hitBoxRadius }; }

	private:

		bool active{ true };
		vec2 hitBoxPos;

		unsigned int frame;
		/* Should the frame be updated */
		bool updateFrame{ true };
		
		/* How many times faster the flame should be relative */
		/* to coal enemies and the player */
		static constexpr float speedMultiplier{ 3.5f };
	};
}