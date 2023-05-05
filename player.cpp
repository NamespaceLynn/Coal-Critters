#include "player.h"

#include "flame.h"
#include "game.h"
#include "mathFunctions.h"

#include <cmath>
#include <utility>

namespace Tmpl8 {

	Player::Player( shared_ptr<Sprite> _sprite, shared_ptr<Sprite> _mushroom, 
		vec2 _centerPos, float _hitBoxRadius, float _speed )
		: Entity( std::move( _sprite ), _centerPos, _hitBoxRadius, _speed )
		, mushroom_sprite( std::move(_mushroom) )
	{
		/* Puts the player in the middle of the screen */
		centerPos.x = (static_cast<float>(ScreenWidth) / 2.0f);
		centerPos.y = (static_cast<float>(ScreenHeight) / 2.0f);

		/* Set all "previous" positions to (0, 0) */
		for (auto& pos : previousPos)
		{
			pos = { 0.0f, 0.0f };
		}
	}

	void Player::CalcSetFrame( int mousex, int mousey )
	{
		angle = CalcDirInDegrees( centerPos.x, centerPos.y, static_cast<float>(mousex), static_cast<float>(mousey) );

		/* makes sure that frame is a valid value */
		/* lround improves the rounding off */
		frame = static_cast<unsigned int>(lround(fmodf( angle, 360.0f ) / 6.0f));
		frame = (frame < 0) ? 0 : (frame > 59) ? 59 : frame;
		
		sprite->SetFrame( frame );
		mushroom_sprite->SetFrame( frame );
	}

	// Check collision before moving the player.
	void Player::SetMovement( const float dx, const float dy )
	{
		movePos.x += dx;
		movePos.y += dy;
	}

	void Player::reduceHitPoints(int damage)
	{
		if (hitPoints > 0 )
		{
			hitPoints -= damage;
		}
		/* hitPoints should never be negative */
		if (hitPoints < 0 )
		{
			hitPoints = 0;
		}
	}

	void Player::Update(float deltaTime, int mousex, int mousey)
	{
		/* Set the new frame (which direction the player faces) */
		CalcSetFrame( mousex, mousey );

		/* Update previous positions */
		for (int i = static_cast<int>(previousPos.size()) - 1;
			i > 0; --i)
		{
			previousPos[i] = previousPos[i - 1];
		}
		previousPos[0] = centerPos;

		/* Update the boost state variables */
		if (boost)
		{
			++boostFrameCount;
		}
		else if (++afterBoostFrameCount > static_cast<int>(previousPos.size()))
		{
			boostFrameCount = 0;
		}

		/* Normalize movement vector */
		/* Normalize function was changed to take a variable that determines */
		if (movePos.length() > 0)
		{
			if (boost)
			{
				movePos.normalize( boostModifier );
			}
			else
			{
				movePos.normalize();
			}
		}

		/* Apply the movement vector to the player's position*/
		centerPos.x += movePos.x * speed * deltaTime;
		centerPos.y += movePos.y * speed * deltaTime;

		/* Reset the movement vector */
		movePos.x = 0.0f;
		movePos.y = 0.0f;

		CheckWallCollision();
	}

	void Player::CheckWallCollision()
	{
		/* Side wall collision check and correction */
		if (centerPos.x - hitBoxRadius < backgroundOffset)
		{
			centerPos.x = backgroundOffset + hitBoxRadius;
		}
		else if ((centerPos.x + hitBoxRadius - 1) >= (ScreenWidth - backgroundOffset))
		{
			centerPos.x = ScreenWidth - backgroundOffset - hitBoxRadius;
		}

		/* Bottom and top wall collision check and correction */
		if (centerPos.y - hitBoxRadius < backgroundOffset)
		{
			centerPos.y = backgroundOffset + hitBoxRadius;
		}
		else if ((centerPos.y + hitBoxRadius - 1) >= (ScreenHeight - backgroundOffset))
		{
			centerPos.y = ScreenHeight - backgroundOffset - hitBoxRadius;
		}
	}

	void Player::DrawHitBox(Surface* screen) const
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

		// -angle because the y-axis is flipped
		const float angleInRad = -angle * (PI / 180.0f);
		const vec2 tempDir = { cosf( angleInRad ), sinf( angleInRad ) };
		const vec2 point1 = centerPos + (tempDir * hitBoxRadius);
		const vec2 point2 = centerPos + (tempDir * (hitBoxRadius + dirLineLength));
		screen->Line( point1.x, point1.y, point2.x, point2.y, 0x00ff00 );
	}

	void Player::Draw(Surface* screen)
	{
		/* Just became immune, set flashRed to the correct state */
		if (immuneLastTick && immunityTimer > 0.0f)
		{
			flashRed = true;
			immuneSwitchTimer = maxTimeBetweenImmuneSwitch;
		}
		immuneLastTick = (immunityTimer <= 0.0f);

		if (mushroomMan)
		{
			DrawMushroom( screen );
			return;
		}

		if (boost)
		{
			int offset;
			int decrement;
			/* If the deltaTime is too high, don't draw the far away previous positions */
			/* Adjust the offset and decrement to have it look roughly the same, */
			/* Though this is only really for 60 and 144 fps (not a very elegant solution) */
			if (dt >= 0.0118f) { offset = 6; decrement = 1; }
			else { offset = 1; decrement = 2; }

			if (immunityTimer <= 0 || !flashRed) // flashRed = false;
			{
				const int size = static_cast<int>(previousPos.size());
				if (boostFrameCount < size)
				{
					for (int i = boostFrameCount - offset; i >= 0; i -= decrement)
					{
						sprite->DrawBlend( screen,
							static_cast<int>(previousPos[i].x - halfWidth),
							static_cast<int>(previousPos[i].y - halfHeight),
							1.0f / static_cast<float>(i + 2),
							shadowOffset, shadowOffset, shadowFadeLength, 0x000000, 0.8f / static_cast<float>(i + 2) );
					}
				}
				else
				{
					for (int i = size - offset; i >= 0; i -= decrement)
					{
						sprite->DrawBlend( screen,
							static_cast<int>(previousPos[i].x - halfWidth),
							static_cast<int>(previousPos[i].y - halfHeight),
							1.0f / static_cast<float>(i + 2),
							shadowOffset, shadowOffset, shadowFadeLength, 0x000000, 0.8f / static_cast<float>(i + 2) );
					}
				}
			}
			else // flashRed = true
			{
				const int size = static_cast<int>(previousPos.size());
				if (boostFrameCount < size)
				{
					for (int i = boostFrameCount - offset; i >= 0; i -= decrement)
					{
						sprite->DrawInColorAndBlend( screen,
							static_cast<int>(previousPos[i].x - halfWidth),
							static_cast<int>(previousPos[i].y - halfHeight),
							hitColor,
							1.0f / static_cast<float>(i + 2) );
					}
				}
				else
				{
					for (int i = size - offset; i >= 0; i -= decrement)
					{
						sprite->DrawInColorAndBlend( screen,
							static_cast<int>(previousPos[i].x - halfWidth),
							static_cast<int>(previousPos[i].y - halfHeight),
							hitColor,
							1.0f / static_cast<float>(i + 2) );
					}
				}
			}
		}

		if (immunityTimer <= 0)
		{
			sprite->DrawWithShadow( screen,
				static_cast<int>(centerPos.x - halfWidth),
				static_cast<int>(centerPos.y - halfHeight),
				shadowOffset, shadowOffset, shadowFadeLength,
				0x000000, 0.5f );

			collisionWithGoldCoal = false;
			return;
		}
		else if (flashRed)
		{
			if (collisionWithGoldCoal)
			{
				/* Collided with GoldCoal, draw in gold */
				sprite->DrawInColor( screen,
					static_cast<int>(centerPos.x - halfWidth),
					static_cast<int>(centerPos.y - halfHeight),
					goldColor );
			}
			else
			{
				/* Hit by an enemy, draw in red */
				sprite->DrawInColor( screen,
					static_cast<int>(centerPos.x - halfWidth),
					static_cast<int>(centerPos.y - halfHeight),
					hitColor );
			}
		}
		else // !flashRed
		{
			sprite->DrawInBlendedColor( screen,
				static_cast<int>(centerPos.x - halfWidth),
				static_cast<int>(centerPos.y - halfHeight),
				0x000000,
				0.25f,
				shadowOffset, shadowOffset, shadowFadeLength, 0x000000, 0.5f );
		}

		/* Update the switch timer */
		immuneSwitchTimer -= dt;
		if (immuneSwitchTimer <= 0)
		{
			immuneSwitchTimer = maxTimeBetweenImmuneSwitch;
			flashRed = !flashRed;
		}
	}

	void Player::DrawDead(Surface* screen) const
	{
		if (mushroomMan)
		{
			DrawDeadMushroom( screen );
			return;
		}

		sprite->DrawInBlendedColor( screen, 
			static_cast<int>(centerPos.x - halfWidth),
			static_cast<int>(centerPos.y - halfHeight), 
			0x0000ff, 0.25f );
	}

	void Player::DrawMushroom( Surface* screen )
	{
		if (boost)
		{
			int offset;
			int decrement;
			/* If the deltaTime is too high, don't draw the far away previous positions */
			/* Adjust the offset and decrement to have it look roughly the same, */
			/* Though this is only really for 60 and 144 fps (not a very elegant solution) */
			if (dt >= 0.0118f) { offset = 6; decrement = 1; }
			else { offset = 1; decrement = 2; }

			if (immunityTimer <= 0 || !flashRed)
			{
				const int size = static_cast<int>(previousPos.size());
				if (boostFrameCount < size)
				{
					for (int i = boostFrameCount - offset; i >= 0; i -= decrement)
					{
						mushroom_sprite->DrawBlend( screen,
							static_cast<int>(previousPos[i].x - halfWidth),
							static_cast<int>(previousPos[i].y - halfHeight),
							1.0f / static_cast<float>(i + 2),
							shadowOffset, shadowOffset, shadowFadeLength, 0x000000, 0.8f / static_cast<float>(i + 2) );
					}
				}
				else
				{
					for (int i = size - offset; i >= 0; i -= decrement)
					{
						mushroom_sprite->DrawBlend( screen,
							static_cast<int>(previousPos[i].x - halfWidth),
							static_cast<int>(previousPos[i].y - halfHeight),
							1.0f / static_cast<float>(i + 2),
							shadowOffset, shadowOffset, shadowFadeLength, 0x000000, 0.8f / static_cast<float>(i + 2) );
					}
				}
			}
			else // fleshRed = true
			{
				const int size = static_cast<int>(previousPos.size());
				if (boostFrameCount < size)
				{
					for (int i = boostFrameCount - offset; i >= 0; i -= decrement)
					{
						mushroom_sprite->DrawInColorAndBlend( screen,
							static_cast<int>(previousPos[i].x - halfWidth),
							static_cast<int>(previousPos[i].y - halfHeight),
							hitColor,
							1.0f / static_cast<float>(i + 2) );
					}
				}
				else
				{
					for (int i = size - offset; i >= 0; i -= decrement)
					{
						mushroom_sprite->DrawInColorAndBlend( screen,
							static_cast<int>(previousPos[i].x - halfWidth),
							static_cast<int>(previousPos[i].y - halfHeight),
							hitColor,
							1.0f / static_cast<float>(i + 2) );
					}
				}
			}
		}

		if (immunityTimer <= 0)
		{
			mushroom_sprite->DrawWithShadow( screen,
				static_cast<int>(centerPos.x - halfWidth),
				static_cast<int>(centerPos.y - halfHeight),
				shadowOffset, shadowOffset, shadowFadeLength,
				0x000000, 0.5f );

			collisionWithGoldCoal = false;
			return;
		}
		else if (flashRed)
		{
			if (collisionWithGoldCoal)
			{
				/* Collided with GoldCoal, draw in gold */
				mushroom_sprite->DrawInColor( screen,
					static_cast<int>(centerPos.x - halfWidth),
					static_cast<int>(centerPos.y - halfHeight),
					goldColor );
			}
			else
			{
				/* Hit by an enemy, draw in red */
				mushroom_sprite->DrawInColor( screen,
					static_cast<int>(centerPos.x - halfWidth),
					static_cast<int>(centerPos.y - halfHeight),
					hitColor );
			}
		}
		else // !flashRed
		{
			mushroom_sprite->DrawInBlendedColor( screen,
				static_cast<int>(centerPos.x - halfWidth),
				static_cast<int>(centerPos.y - halfHeight),
				0x000000,
				0.25f,
				shadowOffset, shadowOffset, shadowFadeLength, 0x000000, 0.5f );
		}

		/* Update the switch timer */
		immuneSwitchTimer -= dt;
		if (immuneSwitchTimer <= 0)
		{
			immuneSwitchTimer = maxTimeBetweenImmuneSwitch;
			flashRed = !flashRed;
		}
	}

	void Player::DrawDeadMushroom(Surface* screen) const
	{
		mushroom_sprite->DrawInBlendedColor( screen,
			static_cast<int>(centerPos.x - halfWidth),
			static_cast<int>(centerPos.y - halfHeight),
			0x0000ff, 0.25f );
	}
}