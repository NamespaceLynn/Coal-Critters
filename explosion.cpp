#include "explosion.h"

#include "mathFunctions.h"

namespace Tmpl8 {

	Explosion::Explosion( std::shared_ptr<Sprite> _sprite, vec2 _centerPos, 
						float _hitBoxRadius, float _speed )
		: Entity( std::move( _sprite ), _centerPos, _hitBoxRadius, _speed )
	{}

	void Explosion::Update(float deltaTime)
	{
		timeSinceLastFrame += deltaTime;

		if (timeSinceLastFrame >= timeBetweenFrames)
		{
			currentFrame = ++currentFrame;

			/* Have the hit box radius fit the sprite's frame */
			if		(currentFrame == 4)	{ hitBoxRadius = 16.0f; }
			else if (currentFrame == 5)	{ hitBoxRadius = 3.0f; }
			else if (currentFrame == 6) { hitBoxRadius = 0.0f; }

			/* When past the last frame, set alive to false... */
			/* to indicate it can be deleted */
			if (currentFrame > sprite->Frames())
			{
				active = false;
				return;
			}

			timeSinceLastFrame = 0.0f;
		}
	}

	void Explosion::Draw(Surface* screen)
	{
		sprite->SetFrame( currentFrame );
		Entity::Draw( screen );
	}

	void Explosion::DrawHitBox(Surface* screen) const
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
	}
}