#include "coal.h"

#include "game.h"

namespace Tmpl8 {

	Coal::Coal( std::shared_ptr<Sprite> _sprite, vec2 _centerPos,
				float _hitBoxRadius, float _speed )
		: Entity( std::move(_sprite), _centerPos, _hitBoxRadius, _speed)
	{}

	void Coal::DrawShadow(Surface* screen)
	{
		sprite->SetFrame( frame );
		Entity::DrawShadow( screen );
	}

	vec2 Coal::ApplySeparation( float deltaTime, const Game& game )
	{
		if constexpr (separationRadius == 0.0f) { return { 0.0f, 0.0f }; }

		/* coalCount and averagePos do not include 'e' */
		int coalCount{ 0 };
		vec2 averageDist{ 0.0f, 0.0f };
		vec2 move{ 0.0f, 0.0f };

		/* Go over basic coals */
		for (auto& c : game.GetCoalBasicVec())
		{
			/* Don't go over 'this' */
			if (c.GetId() != id)
			{
				/* Make sure the target 'c' is within distance */
				if (centerPos.dist( c.GetPos() ) <= separationRadius)
				{
					coalCount++;
					averageDist.x += centerPos.distToPointX( c.GetPos().x );
					averageDist.y += centerPos.distToPointY( c.GetPos().y );
				}
			}
		}
		/* Go over bomb coals */
		for (auto& c : game.GetCoalBombVec())
		{
			/* Don't go over 'this' */
			if (c.GetId() != id)
			{
				/* Make sure the target 'c' is within distance */
				if (centerPos.dist( c.GetPos() ) <= separationRadius)
				{
					coalCount++;
					averageDist.x += centerPos.distToPointX( c.GetPos().x );
					averageDist.y += centerPos.distToPointY( c.GetPos().y );
				}
			}
		}

		if (coalCount)
		{
			averageDist /= coalCount;

			float dX = Clamp( averageDist.x * separationPower, -maxSeparationDist, maxSeparationDist );
			float dY = Clamp( averageDist.y * separationPower, -maxSeparationDist, maxSeparationDist );

			move.x -= dX;
			move.y -= dY;
		}

		const CoalGold& gold = game.GetGoldCoal();
		vec2 toGold = centerPos - gold.GetPos();

		/* If the gold coal is active and */
		/* if 'this' makes contact with the gold coal */
		if	(gold.IsActive() &&
			(toGold.length() <= (gold.GetHitBoxRadius() + hitBoxRadius)))
		{
			toGold.normalize();
			move.x += toGold.x * separationPower;
			move.y += toGold.y * separationPower;
		}
		return move;
	}

	void Coal::CheckWallCollision()
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
}