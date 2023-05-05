#include "coalGold.h"

#include "template.h"

namespace Tmpl8 {

	CoalGold::CoalGold( std::shared_ptr<Sprite> _sprite, vec2 _centerPos,
					float _hitBoxRadius, float _speed )
		: Coal( std::move( _sprite ), _centerPos, _hitBoxRadius, _speed )
	{
		speed *= speedMultiplier;

		/* Make sure the x and y coordinate is within an allowed range */
		centerPos.x = Clamp( centerPos.x, halfWidth + backgroundOffset, 
							ScreenWidth - 1 - halfWidth - backgroundOffset );
		centerPos.y = Clamp( centerPos.y, halfHeight + backgroundOffset, 
							ScreenHeight - 1 - halfHeight - backgroundOffset );

		/* Temporary */
		const float x = _centerPos.x;
		const float y = _centerPos.y;

		/* Either horizontal or vertical movement is predetermined based
		 * on the distance to the edge of the screen */
		 /* It is fine to use x and y, they are the same as centerPos anyways */
		float distToEdge_x = ((ScreenWidth - x) > x) ? x : (ScreenWidth - x);
		float distToEdge_y = ((ScreenHeight - y) > y) ? y : (ScreenHeight - y);

		/* Decide the original direction */
		if (distToEdge_x > distToEdge_y) { moveDir = Movement::HORIZONTAL; dir = { 1.0f, 0.0f }; }
		else { moveDir = Movement::VERTICAL; dir = { 0.0f, 1.0f }; }
	}

	void CoalGold::Update( float deltaTime )
	{
		/* Update the timer */
		activeTimer += deltaTime;

		if (activeTimer <= 0.8f)
		{
			if (activeTimer <= 0.2f)		{ frame = 0; }
			else if (activeTimer <= 0.4f)	{ frame = 1; }
			else if (activeTimer <= 0.6f)	{ frame = 2; }
			else							{ frame = 3; }
		}

		switch (moveDir)
		{
		case Movement::HORIZONTAL:

			/* Change the direction if the gold coal has collided with an edge of the screen */
			if (centerPos.x < halfWidth + backgroundOffset)
			{
				dir = { 1.0f, 0.0f };
			}
			else if (centerPos.x >= (ScreenWidth - halfWidth - backgroundOffset))
			{
				dir = { -1.0f, 0.0f };
			}
			break;

		case Movement::VERTICAL:

			/* Change the direction if the gold coal has collided with an edge of the screen */
			if (centerPos.y < halfHeight + backgroundOffset)
			{
				dir = { 0.0f, 1.0f };
			}
			else if (centerPos.y >= (ScreenHeight - halfHeight - backgroundOffset))
			{
				dir = { 0.0f, -1.0f };
			}
			break;

		/* moveDir should never be NONE when the Move function is called */
		case Movement::NONE:
			break;
		}
		centerPos += dir * speed * deltaTime;
	}

	void CoalGold::Draw(Surface* screen)
	{
		sprite->DrawWithShadow( screen,
			static_cast<int>(centerPos.x - halfWidth),
			static_cast<int>(centerPos.y - halfHeight),
			shadowOffset, shadowOffset, shadowFadeLength,
			0x000000, 0.5f );
	}

	void CoalGold::Replace(vec2 _centerPos, vec2 _dir)
	{
		active = true;
		centerPos = _centerPos;
		activeTimer = 0.0f;

		/* Temporary */
		const float x = _centerPos.x;
		const float y = _centerPos.y;

		/* Either horizontal or vertical movement is predetermined based
		 * on the distance to the edge of the screen */
		 /* It is fine to use x and y, they are the same as centerPos anyways */
		float distToEdge_x = ((ScreenWidth - x) > x) ? x : (ScreenWidth - x);
		float distToEdge_y = ((ScreenHeight - y) > y) ? y : (ScreenHeight - y);

		/* Decide the original direction */
		if (distToEdge_x > distToEdge_y) { moveDir = Movement::HORIZONTAL; dir = { 1.0f, 0.0f }; }
		else { moveDir = Movement::VERTICAL; dir = { 0.0f, 1.0f }; }
	}
}