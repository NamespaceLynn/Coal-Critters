#pragma once

#include "entity.h"

#include <array>

using std::shared_ptr;

namespace Tmpl8 {
	
	class Player : public Entity
	{
	public:
		/* Constructor */
		Player( shared_ptr<Sprite> sprite, shared_ptr<Sprite> mushroom, 
			vec2 centerPos, float hitBoxRadius, float speed );

		void Update( float deltaTime ) override { /* UNIMPLEMENTED */ }
		void Update( float deltaTime, int mousex, int mousey );
		void CheckWallCollision();
		void DrawHitBox(Surface* screen) const override;
		void Draw(Surface* screen) override;
		/* Draws the player with a blue tint, to indicate it being "dead" */
		void DrawDead( Surface* screen ) const;
		void DrawMushroom( Surface* screen );
		void DrawDeadMushroom( Surface* screen ) const;

		/* Calculates and sets the sprite's current frame based on player and mouse coordinates */
		void CalcSetFrame( int mousex, int mousey );
		/* Sets up the movement, which is applied in Update */
		void SetMovement( float dx, float dy );
		void reduceHitPoints( int damage );

		static unsigned int GetFrame() { return frame; }
		bool IsFlashingRed() { return flashRed; }
		[[nodiscard]] int GetHitPoints() const { return hitPoints; }
		void SetHitPoints( int hp ) { hitPoints = hp; }
		[[nodiscard]] bool GetBoostState() const { return boost; }
		void SetBoostState( bool boostState ) { boost = boostState; }
		void SetDeltaTime( float _dt ) { dt = _dt; }
		void SetPosX( float x ) { centerPos.x = x; }
		void SetPosY( float y ) { centerPos.y = y; }
		void SetImmunity( float time ) { immunityTimer = time; }
		void SetHitByGoldCoal() { collisionWithGoldCoal = true; }
		void ActivateMushroomMan() { mushroomMan = true; }
		[[nodiscard]] float GetWidth() const { return static_cast<float>(sprite->GetWidth()); }
		[[nodiscard]] float GetHeight() const { return static_cast<float>(sprite->GetHeight()); }

	private:
		shared_ptr<Sprite> mushroom_sprite;
		/* Should the mushroom man sprite be displayed instead of the default one */
		bool mushroomMan{ false };

		int hitPoints{ 3 };

		/* DeltaTime (dt) used in Draw() */
		/* Updated via SetDeltaTime() in Game */
		float dt{ 0.0f };
		float immunityTimer{ 0.0f };
		bool immuneLastTick{ false };
		/* Only applies when immunityTimer > 0 */
		bool flashRed{ false };
		/* True when collided with the golden coal*/
		bool collisionWithGoldCoal{ false };
		float immuneSwitchTimer{ 0.0f };
		/* Time between the player flashing red/gold and being invisible */
		const float maxTimeBetweenImmuneSwitch{ 0.375f };
		unsigned int hitColor{ 0xfd5f44 };
		unsigned int goldColor{ 0xedcd72 };

		/* Angle between the player and the mouse */
		float angle{ 0.0f }; 
		vec2 movePos{ 0.0f, 0.0f };
		/* Holds the previous' frames' positions */
		std::array<vec2, 12> previousPos;
		inline static unsigned int frame{ 0 };

		/* boost mechanic variables */
		bool boost{ false };
		int boostFrameCount{ 0 };
		int afterBoostFrameCount{ 0 };
		float boostModifier{ 4.0f };
	};
}