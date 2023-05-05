#include "coalBomb.h"

#include "game.h"
#include "mathFunctions.h"

namespace Tmpl8 {

	CoalBomb::CoalBomb( std::shared_ptr<Sprite> _sprite, vec2 _centerPos,
					float _hitBoxRadius, float _speed )
		: Coal( std::move( _sprite ), _centerPos, _hitBoxRadius, _speed )
	{}

	void CoalBomb::Update( float deltaTime, const Game& game )
	{
		/* Handle despawn mechanic if necessary */
		if (despawn)
		{
			/* Apply logic to the frame system */
			timeSinceFrameUpdate += deltaTime;

			if (frame == 0 && (timeSinceFrameUpdate >= timeBetweenFrameUpdates))
			{
				despawned = true;
			}
			else if (timeSinceFrameUpdate >= timeBetweenFrameUpdates)
			{
				--frame;
				timeSinceFrameUpdate = 0.0f;
			}

		}
		/* Only start applying movement logic once */
		/* the spawning animation has ended */
		else if (finishedSpawning)
		{
			vec2 move{ 0.0f, 0.0f };

			/* Calculate delta X and delta Y */
			const float dX = game.GetPlayerPos().x - centerPos.x;
			const float dY = game.GetPlayerPos().y - centerPos.y;

			if (dX != 0.0f || dY != 0.0f)
			{
				/* Directional speed is adjusted to move the same amount per tick/second/etc.
				 * regardless of direction */
				const float moveX = (speed * dX) / sqrtf( dX * dX + dY * dY );
				const float moveY = (speed * dY) / sqrtf( dX * dX + dY * dY );

				move.x += moveX;
				move.y += moveY;
			}
			// Setting the direction, only used for drawing the direction hit box line
			dir = move;
			/* Adjust position based on separation (steering behavior) */
			move += ApplySeparation( deltaTime, game );

			centerPos += (move * deltaTime);
		}
		else
		{
			/* Apply logic to the frame system */
			timeSinceFrameUpdate += deltaTime;
			if (frame == (sprite->Frames() - 1))
			{
				updateFrame = false;
				finishedSpawning = true;
				invincible = false;
			}
			else if (timeSinceFrameUpdate >= timeBetweenFrameUpdates)
			{
				updateFrame = true;
				timeSinceFrameUpdate = 0.0f;
			}
		}

		/* Always check collision with the wall */
		CheckWallCollision();
	}

	void CoalBomb::Draw( Surface* screen )
	{
		/* Update the frame when necessary */
		if (updateFrame)
		{
			if (++frame >= sprite->Frames())
			{
				frame = sprite->Frames();
			}
			updateFrame = false;
		}
		
		/* Draw the sprite */
		sprite->SetFrame( frame );
		if (frame > sprite->Frames() - 3)
		{
			sprite->DrawWithShadow( screen,
				static_cast<int>(centerPos.x - halfWidth),
				static_cast<int>(centerPos.y - halfHeight),
				shadowOffset, shadowOffset, shadowFadeLength,
				0x000000, 0.5f );
		}
		else /* Draw the coal a bit darker while spawning (as it comes from underground) */
		{
			sprite->DrawInBlendedColor( screen,
				static_cast<int>(centerPos.x - halfWidth),
				static_cast<int>(centerPos.y - halfHeight),
				0x000000,
				0.1f,
				shadowOffset, shadowOffset, shadowFadeLength, 0x000000, 0.5f );
		}
	}

	void CoalBomb::DrawHitBox(Surface* screen) const
	{
		Entity::DrawHitBox( screen );

		Pixel* address = screen->GetBuffer();

		for (float i = 0.0f; i < (2 * PI);)
		{
			/* Get a point on the hit box's circle outline */
			vec2 point = CalcPointOnCircle( {centerPos, explodeDist}, i );

			/* If the point is valid, draw it */
			if (point.x < (ScreenWidth - 1) && point.x > 0 &&
				point.y < (ScreenHeight - 1) && point.y > 0)
			{
				address[static_cast<unsigned int>(point.x) +
					static_cast<unsigned int>(point.y) *
					ScreenWidth] = 0xffff00;
			}

			/* Increment */
			i += (PI / 30.0f);
		}
	}
}