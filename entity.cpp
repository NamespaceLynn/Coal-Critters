#include "entity.h"

#include "mathFunctions.h"

namespace Tmpl8	{

	Entity::Entity( std::shared_ptr<Sprite> _sprite, vec2 _centerPos, 
					float _hitBoxRadius, float _speed )
		: sprite( std::move(_sprite) )
		, centerPos( _centerPos )
		, hitBoxRadius( _hitBoxRadius )
		, speed( _speed )
	{
		halfWidth = static_cast<float>(sprite->GetWidth()) * 0.5f;
		halfHeight = static_cast<float>(sprite->GetHeight()) * 0.5f;

		/* Sets a unique ID for each entity */
		id = next_id;
		next_id++;
		dirLineLength = hitBoxRadius * 2;
	}

	void Entity::Draw( Surface* screen )
	{
		sprite->Draw( screen,
			static_cast<int>(centerPos.x - halfWidth),
			static_cast<int>(centerPos.y - halfHeight) );
	}

	void Entity::DrawShadow(Surface* screen)
	{
		/* The shadow is offset towards the bottom right */
		sprite->DrawInColor( screen, 
			static_cast<int>(centerPos.x - halfWidth) + 5, 
			static_cast<int>(centerPos.y - halfHeight) + 5, 
			0x745146 );
	}

	void Entity::DrawHitBox( Surface* screen ) const
	{
		Pixel* address = screen->GetBuffer();

		for (float i = 0.0f; i < (2 * PI);)
		{
			/* Get a point on the hit box's circle outline */
			const vec2 point = CalcPointOnCircle( GetCircle(), i );

			/* If the point is valid, draw it */
			if (point.x < (ScreenWidth - 1) && point.x > 0 && 
				point.y < (ScreenHeight - 1) && point.y > 0)
			{
				address[static_cast<unsigned int>(point.x) +
					static_cast<unsigned int>(point.y) *
					ScreenWidth] = 0x00ff00;
			}

			/* Increment */
			i += (PI / 60.0f);
		}

		vec2 tempDir = dir;
		tempDir.normalize();
		const vec2 point1 = centerPos + (tempDir * hitBoxRadius);
		const vec2 point2 = centerPos + (tempDir * (hitBoxRadius + dirLineLength));
		screen->Line(point1.x, point1.y, point2.x, point2.y, 0x00ff00);
	}
}