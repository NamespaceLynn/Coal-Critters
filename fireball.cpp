#include "fireball.h"

namespace Tmpl8 {

	Fireball::Fireball( std::shared_ptr<Sprite> _sprite, vec2 _centerPos,
		vec2 _dir, float _hitBoxRadius, float _speed )
		: Entity( std::move( _sprite ), _centerPos, _hitBoxRadius, _speed )
	{
		speed *= speedMultiplier;
		dir = _dir;

		/* Set all "previous" positions to (0, 0) */
		for (auto& pos : previousPos)
		{
			pos = { 0.0f, 0.0f };
		}
	}

	void Fireball::Update(float deltaTime)
	{
		dt = deltaTime;

		/* Update previous positions */
		for (int i = static_cast<int>(previousPos.size()) - 1;
			i > 0; --i)
		{
			previousPos[i] = previousPos[i - 1];
		}
		previousPos[0] = centerPos;

		centerPos.x += dir.x * speed * deltaTime;
		centerPos.y += dir.y * speed * deltaTime;

		/* Makes sure the fireball is in bounds */
		justHitWall = false;
		CheckWallCollision();
	}

	void Fireball::Draw(Surface* screen)
	{
		int offset;
		int decrement;
		/* If the deltaTime is too high, don't draw the far away previous positions */
		/* Adjust the offset and decrement to have it look roughly the same, */
		/* Though this is only really for 60 and 144 fps (not a very elegant solution) */
		if (dt >= 0.0118f) { offset = 10; decrement = 1; }
		else { offset = 1; decrement = 2; }

		const int size = static_cast<int>(previousPos.size());

		for (int i = size - offset; i >= 0; i -= decrement)
		{
			if (previousPos[i].x == 0.0f) { continue; }
			sprite->DrawInColorAndBlend( screen,
				static_cast<int>(previousPos[i].x - halfWidth),
				static_cast<int>(previousPos[i].y - halfHeight),
				0xfd5f44,
				1.0f / static_cast<float>(i + 2),
				shadowOffset, shadowOffset, shadowFadeLength, 0x000000, 0.8f / static_cast<float>(i + 2) );
		}

		sprite->DrawWithShadow( screen,
			static_cast<int>(centerPos.x - halfWidth),
			static_cast<int>(centerPos.y - halfHeight),
			shadowOffset, shadowOffset, shadowFadeLength,
			0x000000, 0.5f );
	}

	void Fireball::CheckWallCollision()
	{
		/* Side wall collision check and correction */
		if (centerPos.x - hitBoxRadius < backgroundOffset)
		{
			centerPos.x = backgroundOffset + hitBoxRadius;
			dir.x = -dir.x;
			justHitWall = true;
		}
		else if ((centerPos.x + hitBoxRadius - 1) >= (ScreenWidth - backgroundOffset))
		{
			centerPos.x = ScreenWidth - backgroundOffset - hitBoxRadius;
			dir.x = -dir.x;
			justHitWall = true;
		}

		/* Bottom and top wall collision check and correction */
		if (centerPos.y - hitBoxRadius < backgroundOffset)
		{
			centerPos.y = backgroundOffset + hitBoxRadius;
			dir.y = -dir.y;
			justHitWall = true;
		}
		else if ((centerPos.y + hitBoxRadius - 1) >= (ScreenHeight - backgroundOffset))
		{
			centerPos.y = ScreenHeight - backgroundOffset - hitBoxRadius;
			dir.y = -dir.y;
			justHitWall = true;
		}
	}
}