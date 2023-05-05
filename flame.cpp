#include "flame.h"

#include "game.h"
#include "mathFunctions.h"

namespace Tmpl8 {

	Flame::Flame( std::shared_ptr<Sprite> _sprite, vec2 _centerPos,
					float _hitBoxRadius, float _speed )
		: Entity( std::move( _sprite ), _centerPos, _hitBoxRadius, _speed )
	{
		speed *= speedMultiplier;

		/* Sets the original direction */
		frame = Player::GetFrame();
		UpdateDirection();

		/* placing the flame in front of the player, instead of on */
		/* Also adjusting the initial position based on the sprite's height and width */
		centerPos.x += (dir.x * 10.0f);
		centerPos.y += (dir.y * 10.0f);

		/* Set the hit box in the correct position */
		hitBoxPos = centerPos + (dir * 7);
	}

	void Flame::Update( float deltaTime )
	{
		if (hitBoxPos.x - hitBoxRadius < backgroundOffset || 
			hitBoxPos.x + hitBoxRadius > ScreenWidth - backgroundOffset ||
			hitBoxPos.y - hitBoxRadius < backgroundOffset || 
			hitBoxPos.y + hitBoxRadius > ScreenHeight - backgroundOffset)
		{
			active = false;
			return;
		}

		centerPos.x += dir.x * speed * deltaTime;
		centerPos.y += dir.y * speed * deltaTime;
		hitBoxPos.x += dir.x * speed * deltaTime;
		hitBoxPos.y += dir.y * speed * deltaTime;
	}

	void Flame::UpdateDirection()
	{
		/* converts the current frame into radians */
		const float radian = static_cast<float>(frame * 6) * PI / 180.0f;

		/* multiply the y direction by -1 because the y-axis is inverted */
		dir.x = cosf( radian );
		dir.y = sinf( radian ) * -1;
	}

	void Flame::Draw( Surface* screen )
	{
		/* Changes the sprite's selected frame, one per tick for each Flame object */
		sprite->SetFrame( frame );
		sprite->DrawWithShadow( screen,
			static_cast<int>(centerPos.x - halfWidth),
			static_cast<int>(centerPos.y - halfHeight),
			shadowOffset, shadowOffset, shadowFadeLength,
			0x000000, 0.5f );
	}

	void Flame::DrawHitBox(Surface* screen) const
	{
		Pixel* address = screen->GetBuffer();

		for (float i = 0.0f; i < (2 * PI);)
		{
			/* Get a point on the hit box's circle outline */
			/* (dir * 7) is added to the centerPos to get the */
			/* position of the hit box */
			vec2 point = CalcPointOnCircle( GetCircle(), i );

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
		vec2 tempDir = dir;
		tempDir.normalize();
		const vec2 point1 = hitBoxPos + (tempDir * hitBoxRadius);
		const vec2 point2 = hitBoxPos + (tempDir * (hitBoxRadius + dirLineLength));
		screen->Line( point1.x, point1.y, point2.x, point2.y, 0x00ff00 );
	}
}
