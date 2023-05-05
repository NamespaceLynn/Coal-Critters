#pragma once

#include "template.h"

namespace Tmpl8 {

	/* Calculates the angle between two points in degrees */
	/* Ranges from 0 - 359.999... */
	[[nodiscard]] inline float CalcDirInDegrees( float x1, float y1, float x2, float y2 )
	{
		float angle{ 0 };

		const float dX = x2 - x1;
		const float dY = y2 - y1;

		/* Ensures the function never divides by 0 */
		if ((dX != 0.0f))
		{
			angle = atan2f( dY, dX );
			/* (180.0f / PI) to convert from radians to degrees */
			/* multiply by -1 because the y-axis is inverted */
			angle = angle * (180.0f / PI) * -1;

			/* adjusts negative values so the angle ranges from 0 - 359.999... degrees */
			if (angle < 0.0f)
			{
				return (angle + 360.0f);
			}
			else return angle;
		}
		else /* dX is 0 */
		{
			if (dY < 0.0f) return 90.0f;
			else return 270.0f;
		}
	}

	/* Calculates a point on a circle based on the radius, the angle, and... */
	/* the provided coordinates, which should be the middle of the circle */
	[[nodiscard]] inline vec2 CalcPointOnCircle(Circle c, float angle)
	{
		return { cosf( angle ) * c.r + c.pos.x,
				sinf( angle ) * c.r + c.pos.y };
	}

	[[nodiscard]] inline bool DoCirclesOverlap(Circle c1, Circle c2)
	{
		/* If either radii are 0, the objects can't overlap */
		if (c1.r <= 0.0f || c2.r <= 0.0f) { return false; }

		/* Return true if the objects overlap */
		return ((c1.pos.x - c2.pos.x) * (c1.pos.x - c2.pos.x) +
				(c1.pos.y - c2.pos.y) * (c1.pos.y - c2.pos.y)
				< (c1.r + c2.r) * (c1.r + c2.r));
	}

	/* Save a sqrtf by returning the squared distance */
	[[nodiscard]] inline float GetSquaredDist(vec2 v1, vec2 v2)
	{
		return	(v1.x - v2.x) * (v1.x - v2.x) +
				(v1.y - v2.y) * (v1.y - v2.y);
	}
}