#pragma once

#include "surface.h"
#include "template.h"

#include <memory>

namespace Tmpl8
{
	/* Consists of a position and a radius */
	struct Circle
	{
		Circle(vec2 _centerPos, float _hitBoxRadius)
			: pos(_centerPos)
			, r(_hitBoxRadius)
		{}
		/* (Center) position */
		vec2 pos;
		/* Radius */
		float r;
	};

	/* Base class for all entities */
	class Entity
	{
	public:

		Entity( Entity&& ) = default;
		Entity& operator=( Entity&& ) = default;

		virtual void Update( float deltaTime ) = 0;
		virtual void Draw( Surface* screen );
		virtual void DrawShadow( Surface* screen );
		virtual void DrawHitBox( Surface* screen ) const;

		[[nodiscard]] unsigned int GetId() const { return id; }
		[[nodiscard]] vec2 GetPos() const { return centerPos; }
		[[nodiscard]] float GetHitBoxRadius() const { return hitBoxRadius; }
		/* Returns the centerPos and hitBoxRadius as a circle */
		/* To be used for collision */
		[[nodiscard]] virtual Circle GetCircle() const { return { centerPos, hitBoxRadius }; }

	protected:

		/* Constructor */
		Entity( std::shared_ptr<Sprite> sprite, vec2 centerPos,
				float hitBoxRadius, float speed );

		/* Sprite */
		std::shared_ptr<Sprite> sprite{ nullptr };

		/* Variables */
		unsigned int id{ 0 };
		inline static unsigned int next_id{ 0 };
		Pixel groundColor{ 0x745146 };

		vec2 dir{ 0.0f, 0.0f };
		vec2 centerPos{ 0.0f, 0.0f };
		float hitBoxRadius{ 0.0f };
		float dirLineLength{ 0.0f };
		float speed{ 0.0f };
		float halfWidth{ 0.0f }, halfHeight{ 0.0f };

		/* Sets the "walls" of the game */
		float backgroundOffset{ 64.0f };
		/* The furthest x & y coordinate where a shadow is present */
		int shadowOffset{ 113 };
		int shadowFadeLength{ 15 };
	};
}