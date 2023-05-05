#pragma once

#include "entity.h"

namespace Tmpl8 {

	/* Spawned when certain entities "die" */
	/* Damages most entities */
	class Explosion : public Entity
	{
	public:
		Explosion( std::shared_ptr<Sprite> sprite, vec2 centerPos,
				float hitBoxRadius, float speed );

		Explosion( Explosion&& ) = default;
		Explosion& operator=( Explosion&& ) = default;

		void Update(float deltaTime) override;
		void Draw(Surface* screen) override;
		void DrawHitBox(Surface* screen) const override;

		void SetActive( bool state ) { active = state; }
		[[nodiscard]] bool IsActive() const { return active; }

	private:

		bool active{ true };

		unsigned int currentFrame{ 0 };

		float timeSinceLastFrame{ 0.0f };
		static constexpr float timeBetweenFrames{ 0.1f };
	};
}