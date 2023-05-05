#pragma once

#include "flame.h"
#include "coalBasic.h"
#include "coalBomb.h"
#include "coalGold.h"
#include "fireball.h"
#include "explosion.h"
#include "button.h"
#include "player.h"
#include "sfx.h"

#include <memory>
#include <vector>
#include <SDL_scancode.h>
#include <SDL_mouse.h>
#include <fstream>
#include <random>

using std::shared_ptr;
using std::vector;
using std::fstream;

namespace Tmpl8 {

	enum class GameState
	{
		MENU, // main menu
		INFO, // info menu
		GAME, // main game play loop
		PAUSE, // pause menu
		GAME_OVER, // transition from GAME (or SECRET_MODE) to GAME_OVER_MENU
		GAME_OVER_MENU, // menu reached upon death
		SECRET_MODE // reached with the Konami code in either menu's
	};
	
	class Game
	{
	public:
		Game( Surface* screen );

		Game( const Game& ) = delete;
		Game& operator=( const Game& ) = delete;

		Game( Game&& ) = default;
		Game& operator=( Game && ) = delete;
		
		void Init();
		void Shutdown();
		void Tick( float deltaTime );
		/* Determines if a (gold)Coal should be created, */
		/* and calls to create it */
		void UpdateAndManageCoalSpawning( float deltaTime );
		void UpdateObjects( float deltaTime );
		void UpdateDespawnCoals( float deltaTime );
		void UpdateDisplayScore( float deltaTime );
		/* Checks if the "bounce" sound effect should be played */
		void UpdateBounceSFX();
		void DoCollision();
		void DrawScreen();
		void DarkenScreen() const;
		void DrawEntityHitBox() const;
		void KillEnemies();
		[[nodiscard]] inline bool AllEnemiesDead() const;
		/* returns true if the player reached the center */
		bool MovePlayerTowardsCenter();
		void ResetGameVariables();
		void UpdateHighScore();
		void SetUpSecretMode();
		void MouseUp( Uint8 key );
		void MouseDown( Uint8 key );
		void MouseMove( int x, int y );
		void KeyUp( SDL_Scancode key );
		void KeyDown( SDL_Scancode key );
		/* Implementation adapted from MAX#2223 in the 3dgep.com discord server */
		/* message link: https://discord.com/channels/515453022097244160/686661689894240277/1095673953734901761 */
		void Quit();

		inline void AddFlame( vec2 pos );
		inline void AddCoalBasic( vec2 pos );
		inline void AddCoalBomb( vec2 pos );
		inline void AddExplosion( vec2 pos );
		inline void AddFireball( vec2 pos, vec2 dir );

		[[nodiscard]] int GetMouseX() const { return mousex; }
		[[nodiscard]] int GetMouseY() const { return mousey; }
		[[nodiscard]] bool IsMouseDown() const { return mouseDown; }
		[[nodiscard]] const vector<CoalBasic>& GetCoalBasicVec() const { return basicCoals; }
		[[nodiscard]] const vector<CoalBomb>& GetCoalBombVec() const { return bombCoals; }
		[[nodiscard]] const CoalGold& GetGoldCoal() const { return goldCoal; }
		[[nodiscard]] vec2 GetPlayerPos() const { return player.GetPos(); }

	private:

		Surface* screen;
		GameState gameState{ GameState::MENU };
		SFX sfx;

		/* Mersenne Twister random number generator */
		std::mt19937_64 rng;
		/* Numbers based on the screen borders and coal width / height */
		std::uniform_int_distribution<unsigned int> rand_x{88, 711};
		std::uniform_int_distribution<unsigned int> rand_y{83, 686};
		std::uniform_int_distribution<unsigned int> rand_100{ 1, 100 };

		bool gameStartedViaMenu{ true };
		/* The score is capped at 99,999 */
		unsigned int score{ 0 };
		unsigned int displayedScore{ 0 };
		float timeBetweenScoreUpdate{ 0.0f };
		const float maxTimeBetweenScoreUpdate{ 0.04f };
		unsigned int currentHighScore{ 0 };
		bool newHighScore{ false };
		float scoreFlashTimer{ 0.0f };
		const float maxScoreFlashTime{ 0.5f };
		fstream highScore;
		/* Points granted per enemy (might be multiplied for certain enemies) */
		const int points{ 5 };
		float gameTimer{ 0.0f };
		float timeSinceGameOver{ 0.0f };
		/* Timer for the explosion on the menu screen */
		float menuExplosionTimer{ 0.0f };
		const float menuExplosionCooldown{ 0.7f };
		/* Timer for the flash that plays when the player hits the golden coal... */
		/* or when entering the 'secret' game mode */
		float flashTimer{ 0.0f };
		const float maxFlashTime{ 1.4f };
		bool killedEnemies{ false };
		bool drawHitBox{ false };
		/* False if the game was paused via GameState::SECRET_MODE */
		/* True if the game was paused via GameState::GAME */
		bool pausedByGame{ false };
		/* False if game over via GameState::SECRET_MODE */
		/* True if game over via GameState::GAME */
		bool regularGameOver{ false };

		//----------------------------//
		// Secret game mode variables //
		//----------------------------//

		bool secretMode{ false };
		/* List of the code inputs in order */
		const vector<SDL_Scancode> konamiInputs{SDL_SCANCODE_UP,	SDL_SCANCODE_UP,
												SDL_SCANCODE_DOWN,	SDL_SCANCODE_DOWN,
												SDL_SCANCODE_LEFT,	SDL_SCANCODE_RIGHT,
												SDL_SCANCODE_LEFT,	SDL_SCANCODE_RIGHT,
												SDL_SCANCODE_B,		SDL_SCANCODE_A };
		/* Counts the number of correct Konami code inputs */
		unsigned int konamiKeyCount{ 0 };
		/* The game can end in the secret mode with 1 HP remaining */
		bool oneHitPointLeft{ false };

		//-----------------//
		// Mouse variables //
		//-----------------//

		int mousex{ 0 };
		int mousey{ 0 };
		/* Tracks if the left mouse button is down at all times */
		bool mouseDown{ false };
		/* Tracks if the left mouse button is down last tick and this tick */
		bool previousLeftPressed{ false };
		bool LeftPressed{ false };
		unsigned int flameCursorFrame{ 0 };
		float flameCursorTimer{ 0.0f };
		const float flameCursorSwitchTime{ 0.1f };

		//---------//
		// Objects //
		//---------//

		/* const object variables */

		const float globalSpeed{ 144.0f };
		const float coalRadius{ 19.0f };
		const float flameRadius{ 5.0f };
		const float explosionRadius{ 32.0f };
		const float fireballRadius{ 36.0f };
		const float playerRadius{ 19.0f };

		/* object (vectors) */

		vector<Flame> flames;
		vector<CoalBasic> basicCoals;
		vector<CoalBomb> bombCoals;
		vector<Fireball> fireballs;
		vector<Explosion> explosions;
		CoalGold goldCoal;
		Player player;
		Button playButton;
		Button tryAgainButton;
		Button menuButton;
		Button pauseButton;
		Button resumeButton;
		Button quitButton;
		Button infoButton;

		//---------------------------------//
		// Object spawn mechanic variables //
		//---------------------------------//

		/* Flames */

		float flameSpawnCooldown{ 0.0f };
		const float maxFlameSpawnCooldown{ 0.4f };
		const float secretFlameSpawnCooldown{ 0.08f };

		/* Gold Coal */

		float timeBetweenGoldSpawnUpdates{ 0.0f };
		const float maxTimeBetweenGoldSpawnUpdates{ 1.0f };
		float goldSpawnCooldown{ 0.0f };
		/* A gold coal can only spawn after a certain increase in score */
		unsigned int scoreAtPreviousGoldCoal{ 0 };
		const unsigned int minScoreIncrease{ 50 };
		/* The gold coal will spawn quicker if it died shortly after spawning, */
		/* This also does not apply if the player touched the coal */
		/* initialized to be higher than fastDeathTime to prevent immediate spawning */
		float timeSinceGoldSpawned{ 2.1f };
		const float fastDeathTime{ 2.0f };
		bool goldDiedByPlayer{ false };
		/* The gold coal's spawn cooldown for when the player is at 1, 2 & 3 HP */
		const std::array<float, 3> goldCooldown{ 10.0f, 20.0f, 30.0f };
		/* The minimum amount of enemies for a gold coal to spawn */
		const int minEnemiesForGoldSpawn{ 3 };
		/* The likeness out of 100 of a gold coal spawning (when it can) per HP */
		const std::array<unsigned int, 3> goldHitPointMod{ 20, 10, 5 };
		/* Minimum distance the gold coal needs to the player to spawn */
		/* and from fireballs/explosions (danger) */
		const float goldSpawnDist{ 300.0f };
		const float goldSpawnDistDanger{ 64.0f };

		/* Coals */

		float coalSpawnCooldown{ 1.0f };
		/* Cooldown based on the timer, the last variable is only for 'lazy time' */
		const std::array<float, 4> coalCooldown{ 2.0f, 1.5f, 1.0f, 0.5f };
		const std::array<float, 3> timerStageCooldowns{ 0.0f, 10.0f, 60.0f };
		/* Chance of a out of 100 BasicCoal spawning instead of a BombCoal based on the timer */
		const std::array<unsigned int, 3> basicCoalPercentage{ 50, 66, 75 };
		const std::array<float, 3> timerStagePercentages{ 0.0f, 30.0f, 90.0f };
		/* The time since the player last shot any coal */
		float timeSinceKill{ 0.0f };
		/* If the time since the played killed an enemy proceeds a certain amount, */
		/* determine the player is being lazy, spawn more coals to force action */
		const float lazyTime{ 8.0f };
		/* Minimum distance the coal needs to be from the player and other coals to spawn */
		const float coalSpawnDist{ 76.0f };

		//--------------------------//
		// Player related variables //
		//--------------------------//

		bool up{ false }, left{ false }, down{ false }, right{ false };
		bool playerAlive{ true };
		/* Boost mechanic */
		bool boost{ false };
		float boostLength{ 0.0f };
		float boostCooldown{ 0.0f };
		float maxBoostCooldown{ 1.0f };
		float playerImmunityTimer{ 0.0f };
		const float maxImmunityTime{ 2.625f };
		/* Damage dealt when the player is hit */
		const int playerDamage{ 1 };
		/* The direction used to move the player towards the center */
		vec2 playerDir{ 0.0f, 0.0f };
		bool calcedPlayerDir{ false };

	};

}; // namespace Tmpl8