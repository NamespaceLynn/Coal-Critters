#include "coalBasic.h"

#include "game.h"
#include "mathFunctions.h"

namespace Tmpl8 {

	CoalBasic::CoalBasic( std::shared_ptr<Sprite> _sprite, vec2 _centerPos,
						float _hitBoxRadius, float _speed )
		: Coal( std::move( _sprite ), _centerPos, _hitBoxRadius, _speed )
	{}

	void CoalBasic::Update(float deltaTime, const Game& game)
	{
		/* Handle "death" mechanic if necessary */
		if (dead)
		{
			timeDead += deltaTime;
			if (timeDead > 0.4f)
			{
				/* Setting despawned to true will have the coal be deleted in Game.cpp */
				despawned = true;
			}
		}
		/* Handle despawn mechanic if necessary */
		else if (despawn)
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

			/* Makes sure not to divide by 0 */
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
			if (frame == (sprite->Frames() - 1 - deathFrames))
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

	void CoalBasic::Draw(Surface* screen)
	{
		if (dead)
		{
			if (timeDead < 0.01f)
			{
				sprite->SetFrame( 5 );

				sprite->DrawInColor( screen,
					static_cast<int>(centerPos.x - halfWidth),
					static_cast<int>(centerPos.y - halfHeight),
					hitColor );

				return;
			}
			else if (timeDead < 0.2f)
			{
				frame = 6;
			}
			else if (timeDead < 0.4f)
			{
				frame = 7;
			}
		}
		/* Update the frame when necessary */
		else if (updateFrame)
		{
			if (++frame >= sprite->Frames())
			{
				frame = sprite->Frames();
			}
			updateFrame = false;
		}

		/* Draw the sprite */
		sprite->SetFrame( frame );
		if (frame > sprite->Frames() - deathFrames - 3)
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
}