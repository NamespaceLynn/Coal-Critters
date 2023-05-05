#include "game.h"

#include "surface.h"
#include "mathFunctions.h"

#include <memory>
#include <string>
#include <iostream>
#include <SDL_events.h>
#include <chrono>

using std::unique_ptr;
using std::make_shared;
using std::make_unique;

namespace Tmpl8
{
	/* Sprites */
	shared_ptr<Sprite> player_sprite = make_shared<Sprite>( new Surface( "assets/player.png" ), 60 );
	shared_ptr<Sprite> mushroom_sprite = make_shared<Sprite>( new Surface( "assets/mushroomMan.png" ), 60 );
	shared_ptr<Sprite> flame_sprite = make_shared<Sprite>( new Surface( "assets/flame.png" ), 60 );
	shared_ptr<Sprite> coalBasic_sprite = make_shared<Sprite>( new Surface( "assets/basicCoal.png" ), 8 );
	shared_ptr<Sprite> coalBomb_sprite = make_shared<Sprite>( new Surface( "assets/bombCoal.png" ), 6 );
	shared_ptr<Sprite> coalGold_sprite = make_shared<Sprite>( new Surface( "assets/goldCoal.png" ), 4 );
	shared_ptr<Sprite> explosion_sprite = make_shared<Sprite>( new Surface( "assets/explosion.png" ), 8 );
	shared_ptr<Sprite> fireball_sprite = make_shared<Sprite>( new Surface( "assets/fireball.png" ), 1 );
	shared_ptr<Sprite> volume_sprite = make_shared<Sprite>( new Surface( "assets/volume.png" ), 4 );
	unique_ptr<Sprite> background_sprite = make_unique<Sprite>( new Surface( "assets/background.png" ), 1 );
	unique_ptr<Sprite> foreground_sprite = make_unique<Sprite>( new Surface( "assets/foreground.png" ), 1 );
	unique_ptr<Sprite> info_sprite = make_unique<Sprite>( new Surface( "assets/info_background.png" ), 1 );
	unique_ptr<Sprite> menuScreen_sprite = make_unique<Sprite>( new Surface( "assets/mainMenu.png" ), 1 );
	unique_ptr<Sprite> cursorFlame_sprite = make_unique<Sprite>( new Surface( "assets/flameCursor.png" ), 10 );
	unique_ptr<Sprite> crossHair_sprite = make_unique<Sprite>( new Surface( "assets/crosshair.png" ), 1 );
	unique_ptr<Sprite> heart_sprite = make_unique<Sprite>( new Surface( "assets/heart.png" ), 1 );

	/* Constructor */
	Game::Game( Surface* _screen )
		: screen( _screen )
		, sfx( volume_sprite )
		, goldCoal( coalGold_sprite, { 0.0f, 0.0f }, coalRadius, globalSpeed )
		, player( player_sprite, mushroom_sprite, { 0.0f, 0.0f }, playerRadius, globalSpeed )
		, playButton( 425, 625, 185, 50, "start", 5, -15, 0xffffff, 0x82ff79 )
		, tryAgainButton( 449, 360, 250, 50, "try again", 4, -45, 0xffffff, 0x82ff79 )
		, menuButton( 100, 360, 250, 50, "menu", 5, 15, 0xffffff, 0xffe98f )
		, pauseButton( 309, 4, 181, 50, "pause", 5, -18, 0x1f161b, 0xffe98f )
		, resumeButton( 274, 328, 250, 50, "resume", 5, -11, 0xffffff, 0x82ff79 )
		, quitButton( 425, 690, 250, 50, "quit", 5, 15, 0xffffff, 0xff9079 )
		, infoButton( 625, 625, 50, 50, "?", 5, 0, 0xffffff, 0xffe98f )
	{}

	// -----------------------------------------------------------
	// Initialize the application
	// -----------------------------------------------------------
	void Game::Init()
	{
		/* Set the volumes of the sound effects */
		sfx.explosion.setVolume( 0.4f );
		sfx.button.setVolume( 0.2f );
		sfx.shoot.setVolume( 0.4f );
		sfx.flash.setVolume( 0.8f );
		sfx.gold.setVolume( 1.0f );
		sfx.bounce.setVolume( 0.2f );
		sfx.hurt.setVolume( 1.0f );
		sfx.contact.setVolume( 1.0f );
		sfx.hitCoal.setVolume( 0.4f );
		sfx.dash.setVolume( 0.4f );

		/* The default cursor is replaced with a custom one */
		SDL_ShowCursor( false );
		/* Set the mouse coordinates well outside of the screen, */
		/* so the custom cursor isn't drawn at (0, 0) at startup */
		mousex = -100;
		mousey = -100;

		/* Seed the random number generator */
		rng.seed( std::chrono::high_resolution_clock::now().time_since_epoch().count() );

		/* Set currentHighScore and possibly overwrite an invalid value in highScore.txt */
		highScore.open( "highScore.txt", fstream::in );
		if (highScore.is_open()) // highScore.txt already exists 
		{
			std::string line; // current high score
			int lines = 0; // amount of lines in highScore.txt (should be 0 or 1)
			while (std::getline( highScore, line ))
			{
				lines++;
			}
			highScore.close();

			/* If the text file is not empty and holds one score ( as it should ) */
			if (lines == 1)
			{
				currentHighScore = std::stoul( line );
				return;
			}
		}
		/* The .txt file doesn't exist or has been edited to not work with this program */
		/* Thus set the high score to 0 */
		highScore.open( "highScore.txt", fstream::out );
		if (highScore.is_open())
		{
			highScore << 0;
			highScore.close();

			currentHighScore = 0;
		}
	}

	// -----------------------------------------------------------
	// Close down application
	// -----------------------------------------------------------
	void Game::Shutdown()
	{}

	// -----------------------------------------------------------
	// Main application tick function
	// -----------------------------------------------------------
	void Game::Tick( float deltaTime )
	{
		/* Clamp deltaTime */
		deltaTime = Min( deltaTime, 33.33333333f );
		/* convert deltaTime to seconds */
		deltaTime *= 0.001f;

		previousLeftPressed = LeftPressed;
		LeftPressed = mouseDown;

		switch (gameState)
		{
			//-----------------------------------------------------------//
			// Main menu screen                                          //
			// accessed on startup or via a button on the game over menu //
			//-----------------------------------------------------------//
		case GameState::MENU:

			/* Check if an explosion should be created at the bomb's fuse */
			menuExplosionTimer -= deltaTime;
			if ((menuExplosionTimer <= 0.0f) &&
				(!previousLeftPressed && LeftPressed) &&
				(mousex >= 588 && mousex <= 613) && (mousey >= 92 && mousey <= 110))
			{
				AddExplosion( { static_cast<float>(mousex), static_cast<float>(mousey) } );
				menuExplosionTimer = menuExplosionCooldown;
			}

			/* Draw menu screen background */
			menuScreen_sprite->Draw( screen, 0, 0 );

			/* Update and draw the buttons */
			/* (!previousLeftPressed && leftPressed) is true when the left... */
			/* mouse button was just pressed, so the user can't activate the... */
			/* button by holding the mouse button before hovering over the button. */
			playButton.Update( deltaTime, mousex, mousey, (!previousLeftPressed && LeftPressed) );
			quitButton.Update( deltaTime, mousex, mousey, (!previousLeftPressed && LeftPressed) );
			infoButton.Update( deltaTime, mousex, mousey, (!previousLeftPressed && LeftPressed) );
			for (auto& e : explosions) { e.Update( deltaTime ); }

			playButton.Draw( screen );
			quitButton.Draw( screen );
			infoButton.Draw( screen );
			/* Draw the high score */
			if (currentHighScore > 0)
			{
				screen->Print( "high score:", 575, 15, 0x36454f, 3 );
				screen->Print( std::to_string( currentHighScore ).c_str(), 625, 50, 0x36454f, 4 );
			}
			/* Draw the explosion on the fuse if applicable */
			for (auto& e : explosions) { e.Draw( screen ); }
			/* Draw the bomb's fuse's hit box when enabled */
			if (drawHitBox) { screen->Box( 588, 92, 613, 110, 0x00ff00 ); }

			sfx.UpdateVolumeBar( mousex, mousey, LeftPressed );
			sfx.DrawVolumeBar( screen );

			flameCursorTimer -= deltaTime;
			if (flameCursorTimer <= 0)
			{
				if (++flameCursorFrame >= cursorFlame_sprite->Frames())
				{
					flameCursorFrame = 0;
				}
				cursorFlame_sprite->SetFrame( flameCursorFrame );
				flameCursorTimer = flameCursorSwitchTime;
			}
			cursorFlame_sprite->Draw( screen, mousex - 11, mousey - 37 );

			// Only change game state when the volume is not being modified
			if (!sfx.CurrentlyModifyingVolume())
			{
				if (playButton.IsPressed())
				{
					gameState = GameState::GAME;
					gameStartedViaMenu = true;
					playerAlive = true;
					explosions.clear();

					/* Play the button sound effect */
					sfx.button.replay();
				}
				else if (infoButton.IsPressed())
				{
					gameState = GameState::INFO;
					menuButton.SetPos( 275, 25 );
					explosions.clear();

					/* Play the button sound effect */
					sfx.button.replay();
				}
				else if (quitButton.IsPressed())
				{
					/* Quit the game */
					Quit();
				}
				else if (secretMode)
				{
					gameState = GameState::SECRET_MODE;
					gameStartedViaMenu = true;
					ResetGameVariables();
					SetUpSecretMode();
					playerAlive = true;
					player.ActivateMushroomMan();
					explosions.clear();

					/* The screen flashes when entering the secret game mode */
					flashTimer = maxFlashTime;
					/* Play a sound effect */
					sfx.flash.replay();
				}
			}

			break;

			//----------------------------------------------//
			// Controls + game info                         //
			// accessed via the '?' button on the main menu //
			//----------------------------------------------//
		case GameState::INFO:

			/* The 'fake' F5 button displayed on the info screen also switches the... */
			/* state of hit boxes when clicked */
			if ((!previousLeftPressed && LeftPressed) &&
				(mousex >= 209 && mousex <= 250) && (mousey >= 602 && mousey <= 643))
			{
				drawHitBox = !drawHitBox;
				sfx.button.replay();
			}

			info_sprite->Draw( screen, 0, 0 );

			menuButton.Update( deltaTime, mousex, mousey, (!previousLeftPressed && LeftPressed) );

			menuButton.Draw( screen );
			if (drawHitBox) { screen->Box( 209, 602, 250, 643, 0x00ff00 ); }

			flameCursorTimer -= deltaTime;
			if (flameCursorTimer <= 0)
			{
				if (++flameCursorFrame >= cursorFlame_sprite->Frames())
				{
					flameCursorFrame = 0;
				}
				cursorFlame_sprite->SetFrame( flameCursorFrame );
				flameCursorTimer = flameCursorSwitchTime;
			}
			cursorFlame_sprite->Draw( screen, mousex - 11, mousey - 37 );

			if (menuButton.IsPressed())
			{
				gameState = GameState::MENU;
				menuButton.SetPos( 100, 360 );

				/* Play the button sound effect */
				sfx.button.replay();
			}
			else if (secretMode)
			{
				gameState = GameState::SECRET_MODE;
				gameStartedViaMenu = true;
				ResetGameVariables();
				SetUpSecretMode();
				playerAlive = true;
				player.ActivateMushroomMan();
				explosions.clear();

				menuButton.SetPos( 100, 360 );

				/* The screen flashes when entering the secret game mode */
				flashTimer = maxFlashTime;
				/* Play a sound effect */
				sfx.flash.replay();
			}

			break;

			//----------------------------------------------------//
			// Main game play loop                                //
			// accessed via pressing a play button on either menu //
			//----------------------------------------------------//
		case GameState::GAME:

			/* Adjust the cooldowns and timer */
			gameTimer += deltaTime;
			flashTimer -= deltaTime;
			if (flashTimer < 0.0f) { flashTimer = 0.0f; }
			if (flameSpawnCooldown > 0) { flameSpawnCooldown -= deltaTime; }
			if (boostCooldown > 0) { boostCooldown -= deltaTime; }
			if (goldSpawnCooldown > 0) { goldSpawnCooldown -= deltaTime; }
			/* Always adjust the coalSpawnCooldown to know how long ago... */
			/* the last coal was created */
			coalSpawnCooldown -= deltaTime;

			/* Apply logic to the player's immunity timer */
			if (playerImmunityTimer > 0) { playerImmunityTimer -= deltaTime; }
			else { playerImmunityTimer = 0; }

			/* Spawns a flame when the left mouse button is being pressed and the cooldown is down */
			/* Sets a cooldown for spawning a new flame */
			if (mouseDown && flameSpawnCooldown <= 0 && gameTimer >= 0.3f && !pauseButton.IsHoveredOver())
			{
				AddFlame( player.GetPos() );
				flameSpawnCooldown = maxFlameSpawnCooldown;
			}

			UpdateAndManageCoalSpawning( deltaTime );
			UpdateObjects( deltaTime );
			UpdateBounceSFX();
			DoCollision();
			player.SetDeltaTime( deltaTime );
			pauseButton.Update( deltaTime, mousex, mousey, (!previousLeftPressed && LeftPressed) );

			DrawScreen();
			pauseButton.Draw( screen );
			/* Print the score */
			UpdateDisplayScore( deltaTime );
			if (score > 99999) { score = 99999; }
			if (score >= currentHighScore && score > 0)
			{
				/* Print the score in green if it matches or is greater than the current high score */
				screen->Print( ("score:" + std::to_string( displayedScore )).c_str(), 530, 17, 0x00ff00, 4 );
			}
			else
			{
				/* Print the score in white if it is lower than the current high score */
				screen->Print( ("score:" + std::to_string( displayedScore )).c_str(), 530, 17, 0xffffff, 4 );
			}
			/* Setting the cross hair's color manually, otherwise it doesn't draw the correct color */
			crossHair_sprite->DrawInColor( screen, mousex - 11, mousey - 11, 0x1f161b );
			if (drawHitBox) { DrawEntityHitBox(); }
			if (flashTimer > 0.0f) { screen->Clear( 0xedcd72, flashTimer / maxFlashTime ); }

			/* Change the game state if the player has no hit points left */
			if (player.GetHitPoints() == 0)
			{
				gameState = GameState::GAME_OVER;
				regularGameOver = true;
				player.SetBoostState( false );
				gameStartedViaMenu = false;
				/* The pause button cannot be selected while the game is over */
				pauseButton.SetPressed( false );
				playerAlive = false;
			}
			else if (pauseButton.IsPressed())
			{
				gameState = GameState::PAUSE;
				pauseButton.SetPressed( false );
				pausedByGame = true;
				menuButton.SetPos( 274, 393 );
				quitButton.SetPos( 274, 458 );

				/* Play the button sound effect */
				sfx.button.replay();
			}

			break;

			//-------------------------------------------------------------------//
			// Pause menu                                                        //
			// accessed via the pause button in the main or secret game play loop //
			//-------------------------------------------------------------------//
		case GameState::PAUSE:

			/* It is fine to update the flash timer when paused */
			/* It is only visual and looks better this way */
			flashTimer -= deltaTime;
			if (flashTimer < 0.0f) { flashTimer = 0.0f; }

			resumeButton.Update( deltaTime, mousex, mousey, (!previousLeftPressed && LeftPressed) );
			menuButton.Update( deltaTime, mousex, mousey, (!previousLeftPressed && LeftPressed) );
			quitButton.Update( deltaTime, mousex, mousey, (!previousLeftPressed && LeftPressed) );

			player.SetDeltaTime( deltaTime );

			DrawScreen();
			pauseButton.Draw( screen );
			if (drawHitBox) { DrawEntityHitBox(); }
			/* Print the score */
			if (!pausedByGame)
			{
				/* Print the score (in the secret mode it displays ERROR instead) */
				screen->Print( "  ERROR", 530, 17, 0xffffff, 4 );
			}
			else if (score >= currentHighScore && score > 0)
			{
				/* Print the score in green if it matches or is greater than the current high score */
				screen->Print( ("score:" + std::to_string( displayedScore )).c_str(), 530, 17, 0x00ff00, 4 );
			}
			else
			{
				/* Print the score in white if it is lower than the current high score */
				screen->Print( ("score:" + std::to_string( displayedScore )).c_str(), 530, 17, 0xffffff, 4 );
			}

			/* Darken everything drawn above */
			DarkenScreen();

			/* Then draw the cross hair and resume button */
			resumeButton.Draw( screen );
			menuButton.Draw( screen );
			quitButton.Draw( screen );
			/* Draw the cross hair white so it is better visible against the darkened background */
			crossHair_sprite->DrawInColor( screen, mousex - 11, mousey - 11, 0xffffff );
			if (flashTimer > 0.0f) { screen->Clear( 0xedcd72, flashTimer / maxFlashTime ); }

			if (resumeButton.IsPressed())
			{
				/* Resume in the correct game state */
				if (pausedByGame)
				{
					gameState = GameState::GAME;
					flameSpawnCooldown = maxFlameSpawnCooldown;
				}
				else
				{
					gameState = GameState::SECRET_MODE;
					flameSpawnCooldown = secretFlameSpawnCooldown;
				}
				menuButton.SetPos( 100, 360 );

				/* Play the button sound effect */
				sfx.button.replay();
			}
			else if (menuButton.IsPressed())
			{
				gameState = GameState::MENU;
				secretMode = false;
				menuButton.SetPos( 100, 360 );
				quitButton.SetPos( 425, 690 );
				player.SetPosX( static_cast<float>(ScreenWidth) / 2.0f );
				player.SetPosY( static_cast<float>(ScreenHeight) / 2.0f );
				player.SetBoostState( false );

				flames.clear();
				basicCoals.clear();
				bombCoals.clear();
				fireballs.clear();
				explosions.clear();
				goldCoal.SetActive( false );
				ResetGameVariables();

				/* Play the button sound effect */
				sfx.button.replay();
			}
			else if (quitButton.IsPressed())
			{
				/* Quit the game */
				Quit();
			}

			break;

			//--------------------------------------------------//
			// Transition from game play loop to game over menu //
			// accessed via main or secret game play loop       //
			//--------------------------------------------------//
		case GameState::GAME_OVER:

			/* Adjusts the necessary timer(s) */
			timeSinceGameOver += deltaTime;

			/* Update objects some objects (except coals) */
			for (auto& fBall : fireballs) { fBall.Update( deltaTime ); }
			UpdateBounceSFX();
			for (auto& flame : flames) { if (flame.IsActive()) { flame.Update( deltaTime ); } }
			for (auto& exp : explosions) { exp.Update( deltaTime ); }
			player.Update( deltaTime, mousex, mousey );

			/* Change immunity timer to > 0 so the player doesn't "take damage" from explosions */
			playerImmunityTimer = 1.0f;
			UpdateDespawnCoals( deltaTime );
			DoCollision();
			/* Change immunity to 0 again so the player isn't drawn as if it were immune */
			playerImmunityTimer = 0.0f;
			player.SetImmunity( 0.0f );
			DrawScreen();
			/* Don't update the pause button, it cannot be used in the game over menu */
			pauseButton.Draw( screen );
			/* Print the score */
			UpdateDisplayScore( deltaTime );
			if (regularGameOver)
			{
				if (score >= currentHighScore && score > 0)
				{
					/* Print the score in green if it matches or is greater than the current high score */
					screen->Print( ("score:" + std::to_string( displayedScore )).c_str(), 530, 17, 0x00ff00, 4 );
				}
				else
				{
					/* Print the score in white if it is lower than the current high score */
					screen->Print( ("score:" + std::to_string( displayedScore )).c_str(), 530, 17, 0xffffff, 4 );
				}
			}
			else
			{
				/* Print ERROR if the player just played the secret game mode */
				screen->Print( "  ERROR", 530, 17, 0xffffff, 4 );
			}
			/* Setting the cross hair's color manually, otherwise it doesn't draw the correct color */
			crossHair_sprite->DrawInColor( screen, mousex - 11, mousey - 11, 0x1f161b );
			if (drawHitBox) { DrawEntityHitBox(); }

			if (timeSinceGameOver > 1.5f)
			{
				for (auto& basic : basicCoals) { if (basic.IsDead()) { basic.Update( deltaTime, *this ); } }
				if (!killedEnemies) { KillEnemies(); }
			}
			if (timeSinceGameOver > 2.5f)
			{
				if (MovePlayerTowardsCenter())
				{
					/* Only change the game mode once the player has reached the center */
					gameState = GameState::GAME_OVER_MENU;

					killedEnemies = false;
					/* Update the high score when necessary*/
					if (regularGameOver) { UpdateHighScore(); }
					if (newHighScore) { scoreFlashTimer = maxScoreFlashTime; }
					timeSinceGameOver = 0.0f;
				}
			}

			break;

			//-------------------------------------------------//
			// Game over menu                                  //
			// automatically accessed via GameState::GAME_OVER //
			//-------------------------------------------------//
		case GameState::GAME_OVER_MENU:

			/* Update the score's flash timer */
			scoreFlashTimer -= deltaTime;
			if (scoreFlashTimer <= -maxScoreFlashTime)
			{
				scoreFlashTimer = maxScoreFlashTime;
			}

			tryAgainButton.Update( deltaTime, mousex, mousey, (!previousLeftPressed && LeftPressed) );
			menuButton.Update( deltaTime, mousex, mousey, (!previousLeftPressed && LeftPressed) );
			player.Update( deltaTime, mousex, mousey );
			UpdateDespawnCoals( deltaTime );

			DrawScreen();
			if (drawHitBox) { DrawEntityHitBox(); }
			tryAgainButton.Draw( screen );
			menuButton.Draw( screen );
			/* Don't update the pause button, it cannot be used in the game over menu */
			pauseButton.Draw( screen );
			/* Print the score and high score */
			if (newHighScore && currentHighScore <= 99999 && regularGameOver)
			{
				/* The high score "flashes" based on a timer if it has just been beaten */
				if (scoreFlashTimer >= 0)
				{
					/* Draw a beaten high score in green */
					screen->Print( "high", 270, 520, 0x00ff00, 5 );
					screen->Print( "score", 270, 550, 0x00ff00, 5 );
					screen->Print( (":" + std::to_string( currentHighScore )).c_str(), 420, 530, 0x00ff00, 5 );
				}
				screen->Print( ("score:" + std::to_string( score )).c_str(), 530, 17, 0x00ff00, 4 );
				screen->Print( ("score:" + std::to_string( score )).c_str(), 270, 600, 0xffffff, 5 );
			}
			else if (regularGameOver)
			{
				screen->Print( "high", 270, 520, 0xffff00, 5 );
				screen->Print( "score", 270, 550, 0xffff00, 5 );
				if (currentHighScore > 99999)
				{
					/* A score above 99,999 is practically impossible */
					/* The player must have changed the score manually, thus call them a 'bad egg' */
					screen->Print( ":bad egg", 420, 530, 0xfd5f44, 5 );
				}
				else
				{
					/* High score under 100,000 (draw in yellow when the high score has not been beaten) */
					screen->Print( (":" + std::to_string( currentHighScore )).c_str(), 420, 530, 0xffff00, 5 );
				}
				if (score >= currentHighScore && score > 0)
				{
					/* Print the score in green if it matches or is greater than the current high score */
					screen->Print( ("score:" + std::to_string( score )).c_str(), 530, 17, 0x00ff00, 4 );
				}
				else
				{
					/* Print the score in white if it is lower than the current high score */
					screen->Print( ("score:" + std::to_string( score )).c_str(), 530, 17, 0xffffff, 4 );
				}
				/* Always also print the score in white at about the bottom of the screen */
				screen->Print( ("score:" + std::to_string( score )).c_str(), 270, 600, 0xffffff, 5 );
			}
			else
			{
				/* Print ERROR if the player just played the secret game mode */
				screen->Print( "  ERROR", 530, 17, 0xffffff, 4 );
			}
			/* Setting the cross hair's color manually, otherwise it doesn't draw the correct color */
			crossHair_sprite->DrawInColor( screen, mousex - 11, mousey - 11, 0x1f161b );

			if (tryAgainButton.IsPressed())
			{
				gameState = GameState::GAME;
				gameStartedViaMenu = false;
				ResetGameVariables();
				playerAlive = true;

				/* Play the button sound effect */
				sfx.button.replay();
			}
			else if (menuButton.IsPressed())
			{
				gameState = GameState::MENU;
				gameStartedViaMenu = false;
				ResetGameVariables();
				quitButton.SetPos( 425, 690 );

				/* Play the button sound effect */
				sfx.button.replay();
			}
			else if (secretMode)
			{
				gameState = GameState::SECRET_MODE;
				gameStartedViaMenu = false;
				ResetGameVariables();
				SetUpSecretMode();
				playerAlive = true;
				player.ActivateMushroomMan();

				/* The screen flashes when entering the secret game mode */
				flashTimer = maxFlashTime;
				/* Play a sound effect */
				sfx.flash.replay();
			}

			break;

			//------------------------------------------------------//
			// Hidden game play loop                                //
			// accessed via entering the konami code on either menu //
			//------------------------------------------------------//
		case GameState::SECRET_MODE:

			/* Adjust the cooldowns and timer */
			gameTimer += deltaTime;
			flashTimer -= deltaTime;
			if (flashTimer < 0.0f) { flashTimer = 0.0f; }
			if (flameSpawnCooldown > 0) { flameSpawnCooldown -= deltaTime; }
			if (boostCooldown > 0) { boostCooldown -= deltaTime; }

			/* Spawns a flame when the left mouse button is being pressed and the cooldown is down */
			/* Sets a cooldown for spawning a new flame */
			if (mouseDown && flameSpawnCooldown <= 0 && gameTimer >= 0.3f && !pauseButton.IsHoveredOver())
			{
				AddFlame( player.GetPos() );
				/* The flame spawn cooldown is shorter in the secret mode */
				flameSpawnCooldown = secretFlameSpawnCooldown;
			}

			UpdateObjects( deltaTime );
			UpdateBounceSFX();
			player.SetDeltaTime( deltaTime );
			DoCollision();
			pauseButton.Update( deltaTime, mousex, mousey, (!previousLeftPressed && LeftPressed) );
			DrawScreen();
			pauseButton.Draw( screen );
			/* Print the score (in the secret mode it displays ERROR instead) */
			screen->Print( "  ERROR", 530, 17, 0xffffff, 4 );
			/* Setting the cross hair's color manually, otherwise it doesn't draw the correct color */
			crossHair_sprite->DrawInColor( screen, mousex - 11, mousey - 11, 0x1f161b );
			if (drawHitBox) { DrawEntityHitBox(); }
			if (flashTimer > 0.0f) { screen->Clear( 0xffffff, flashTimer / maxFlashTime ); }

			/* Change the game state if the player has no hit points left or all enemies died */
			/* or when there is only a single leftover fireball */
			if (player.GetHitPoints() == 0 || AllEnemiesDead() ||
				(basicCoals.empty() && bombCoals.empty() &&
					fireballs.size() == 1 && !goldCoal.IsActive()))
			{
				gameState = GameState::GAME_OVER;
				regularGameOver = false;
				secretMode = false;

				player.SetBoostState( false );
				gameStartedViaMenu = false;

				/* The pause button cannot be selected while the game is over */
				pauseButton.SetPressed( false );

				if (player.GetHitPoints() == 1) { oneHitPointLeft = true; }
				else { playerAlive = false; }
			}
			else if (pauseButton.IsPressed())
			{
				gameState = GameState::PAUSE;
				pauseButton.SetPressed( false );
				pausedByGame = false;
				menuButton.SetPos( 274, 393 );
				quitButton.SetPos( 274, 458 );

				/* Play the button sound effect */
				sfx.button.replay();
			}

			break;
		}
	}

	void Game::UpdateAndManageCoalSpawning( float deltaTime )
	{
		/* Update spawn mechanic timers */
		timeSinceKill += deltaTime;
		if (goldCoal.IsActive()) { timeSinceGoldSpawned += deltaTime; }

		/* The gold coal is checked less often, determine if it should be checked */
		bool checkGold{ false };
		if ((timeBetweenGoldSpawnUpdates -= deltaTime) <= 0.0f)
		{
			checkGold = true;
			timeBetweenGoldSpawnUpdates = maxTimeBetweenGoldSpawnUpdates;
		}

		const int totalCoals = static_cast<int>(basicCoals.size() + bombCoals.size());
		const int totalFireballs = static_cast<int>(fireballs.size());
		const int totalEnemies = totalCoals + totalFireballs;

		/* Determine if a goldCoal should be activated */
		if (checkGold &&
			!goldCoal.IsActive() && // gold coal can't already be active
			timeSinceKill <= lazyTime && // the player can't be 'lazy' and collect coals just to gain points with the gold coal
			totalEnemies >= minEnemiesForGoldSpawn && // there need to be enough enemies
			( // either the coal died quickly after spawning by a fireball or explosion, or the three statements after are both true
			(timeSinceGoldSpawned <= fastDeathTime && !goldDiedByPlayer) ||
			(goldSpawnCooldown <= 0 && // the cooldown needs to be over)
			score - scoreAtPreviousGoldCoal >= minScoreIncrease && // the score needs to have increased before a new gold coal can spawn
			rand_100(rng) <= goldHitPointMod[player.GetHitPoints() - 1]) // random chance to be spawned based on player's heart count
			))
		{
			float x, y;
			int positionsGenerated = 0;
			bool invalidPos = true;
			do
			{
				/* 50/50 for horizontal or vertical */
				if (rand_100( rng ) <= 50)
				{
					/* 50/50 for left or right */
					x = (rand_100( rng ) <= 50) ? 128.0f : 672.0f;
					/* Any valid y value */
					y = static_cast<float>(rand_y( rng ));
				}
				else
				{
					/* 50/50 for top or bottom */
					y = (rand_100( rng ) <= 50) ? 128.0f : 642.0f;
					/* Any valid x value */
					x = static_cast<float>(rand_x( rng ));
				}

				++positionsGenerated;

				bool fail{ false };
				/* Go over the explosions to check if the position is valid */
				for (auto& e : explosions)
				{
					/* If the generated position is too close to a coal */
					if (GetSquaredDist( e.GetPos(), { x, y } ) < (goldSpawnDistDanger * goldSpawnDistDanger))
					{
						/* Break from the for loop if a single coal is too close*/
						fail = true;
						break;
					}
				}
				if (fail) { continue; }

				/* Go over the fireballs to check if the position is valid */
				for (auto& f : fireballs)
				{
					/* If the generated position is too close to a coal */
					if (GetSquaredDist( f.GetPos(), { x, y } ) < (goldSpawnDistDanger * goldSpawnDistDanger))
					{
						/* Break from the for loop if a single coal is too close*/
						fail = true;
						break;
					}
				}
				if (fail) { continue; }

				if (GetSquaredDist( player.GetPos(), { x, y } ) < (goldSpawnDist * goldSpawnDist))
				{
					continue;
				}
				else
				{
					invalidPos = false;
				}

				/* The gold coal can't spawn close to the player*/
			} while (invalidPos && positionsGenerated <= 50);

			/* Activate the gold coal */
			goldCoal.Replace( { x, y }, { 0.0f, 0.0f } );
			goldSpawnCooldown = goldCooldown[player.GetHitPoints() - 1];
			timeSinceGoldSpawned = 0;
			scoreAtPreviousGoldCoal = score;
			goldDiedByPlayer = false;
		}
		
		/* Determine if a basic/bomb coal should be spawned */
		if (coalSpawnCooldown <= 0)
		{
			/* Set the cooldown index based on the timer */
			int cooldownIndex = 0;
			for (int i = static_cast<int>(timerStageCooldowns.size()) - 1; i >= 0; --i)
			{
				if (gameTimer >= timerStageCooldowns[i])
				{
					cooldownIndex = i;
					break;
				}
			}

			/* Set the percentage index based on the timer */
			int percentageIndex = 0;
			for (int i = static_cast<int>(timerStagePercentages.size()) - 1; i >= 0; --i)
			{
				if (gameTimer >= timerStagePercentages[i])
				{
					percentageIndex = i;
					break;
				}
			}

			float x, y;
			/* Stop generating positions after a certain number to avoid infinite loop */
			int positionsGenerated = 0;
			bool invalidPos = true;
			/* Search for a valid position (at random) */
			do
			{
				/* Generate a random position */
				x = static_cast<float>(rand_x( rng ));
				y = static_cast<float>(rand_y( rng ));
				++positionsGenerated;

				bool fail{ false };
				/* Go over the basic coals to check if the position is valid */
				for (auto& c : basicCoals)
				{
					/* If the generated position is too close to a coal */
					if (GetSquaredDist(c.GetPos(), {x, y}) < (coalSpawnDist * coalSpawnDist))
					{
						/* Break from the for loop if a single coal is too close*/
						fail = true;
						break;
					}
				}
				if (fail) { continue; }

				/* Go over the bomb coals to check if the position is valid */
				for (auto& c : bombCoals)
				{
					/* If the generated position is too close to a coal */
					if (GetSquaredDist( c.GetPos(), { x, y } ) < (coalSpawnDist * coalSpawnDist))
					{
						/* Break from the for loop if a single coal is too close*/
						fail = true;
						break;
					}
				}
				if (fail) { continue; }

				/* See if the generated position is valid based on the player's position */
				if (GetSquaredDist(player.GetPos(), {x, y}) < (coalSpawnDist * coalSpawnDist))
				{
					/* if invalid position */
					continue;
				}
				else
				{
					/* The position is valid */
					invalidPos = false;
				}

			} while (invalidPos && positionsGenerated <= 50);

			/* Decide to spawn a BasicCoal or BombCoal */
			if (rand_100(rng) <= basicCoalPercentage[percentageIndex])
			{
				AddCoalBasic( { x, y } );
			}
			else
			{
				AddCoalBomb( {	x, y } );
				/* The cooldown is lower by default when a bomb coal is added */
				/* except for when 'lazy time' is active */
				cooldownIndex = static_cast<int>(coalCooldown.size() - 2);
			}

			/* When 'lazy time' is active, spawn coals more frequently */
			if (timeSinceKill >= lazyTime)
			{
				/* Set the index to be the lowest cooldown (for lazy time) */
				cooldownIndex = static_cast<int>(coalCooldown.size()) - 1;
			}
			coalSpawnCooldown = coalCooldown[cooldownIndex];
		}
	}

	void Game::UpdateObjects(float deltaTime)
	{
		/* Update (most) objects */
		if (goldCoal.IsActive())							{ goldCoal.Update( deltaTime ); }
		for (auto& coal : basicCoals)						{ coal.Update( deltaTime, *this ); }
		for (auto& coal : bombCoals)						{ coal.Update( deltaTime, *this ); }
		for (auto& fBall : fireballs)						{ fBall.Update( deltaTime ); }
		for (auto& flame : flames) { if (flame.IsActive())	{ flame.Update( deltaTime ); } }
		for (auto& exp : explosions)						{ exp.Update( deltaTime ); }
		
		/* Update the player position and direction based on input and mouse position */
		if (boost && boostCooldown <= 0 && (up || down || left || right))
		{
			player.SetBoostState( true );
			boostLength = 0.2f;
			boostCooldown = maxBoostCooldown;

			/* Play a sound effect */
			sfx.dash.replay();
		}
		if (boostLength > 0)
		{
			boostLength -= deltaTime;

			if (boostLength < 0)
			{
				player.SetBoostState( false );
			}
		}
		
		if (up)		player.SetMovement( 0, -1 );
		if (left)	player.SetMovement( -1, 0 );
		if (down)	player.SetMovement( 0, 1 );
		if (right)	player.SetMovement( 1, 0 );

		player.Update( deltaTime, mousex, mousey );
	}

	void Game::UpdateDespawnCoals( float deltaTime )
	{
		/* Basic Coals */
		for (auto basic = basicCoals.begin(); basic < basicCoals.end();)
		{
			if (basic->IsDespawned())
			{
				basic = basicCoals.erase( basic );
				break;
			}
			else
			{
				if (basic->IsDespawning())
				{
					basic->Update( deltaTime, *this );
				}
				++basic;
			}
		}
		/* Bomb Coals */
		for (auto bomb = bombCoals.begin(); bomb < bombCoals.end();)
		{
			if (bomb->IsDespawned())
			{
				bomb = bombCoals.erase( bomb );
				break;
			}
			else
			{
				if (bomb->IsDespawning())
				{
					bomb->Update( deltaTime, *this );
				}
				++bomb;
			}
		}
	}

	void Game::UpdateDisplayScore( float deltaTime )
	{
		if (timeBetweenScoreUpdate <= 0)
		{
			if (displayedScore < score)
			{
				displayedScore++;
				timeBetweenScoreUpdate = maxTimeBetweenScoreUpdate;
			}
		}
		else
		{
			timeBetweenScoreUpdate -= deltaTime;
		}
	}

	void Game::UpdateBounceSFX()
	{
		/* If at least one fireball just hit a wall, play a "bounce" sound */
		for (auto& f : fireballs)
		{
			if (f.IsHittingWall())
			{
				sfx.bounce.replay();
				break;
			}
		}
	}

	void Game::DoCollision()
	{
		/* Delete inactive coals, explosions and flames */
		for (auto basic = basicCoals.begin(); basic < basicCoals.end();)
		{
			if (basic->IsDespawned()) { basic = basicCoals.erase( basic ); }
			else { ++basic; }
		}
		for (auto exp = explosions.begin(); exp < explosions.end();)
		{
			if (!exp->IsActive()) { exp = explosions.erase( exp ); }
			else { ++exp; }
		}
		for (auto flame = flames.begin(); flame < flames.end();)
		{
			if (!flame->IsActive()) { flame = flames.erase( flame ); }
			else { ++flame; }
		}

		/* goldCoal with player collision (regardless of immunity) */
		/* Check for overlap */
		if (goldCoal.IsActive() && DoCirclesOverlap( player.GetCircle(), goldCoal.GetCircle()))
		{
			/* Grant points for all coal enemies*/
			score += points * static_cast<int>(basicCoals.size());
			score += points * static_cast<int>(bombCoals.size());

			/* Remove all enemies */
			goldCoal.SetActive( false );
			basicCoals.clear();
			bombCoals.clear();
			fireballs.clear();
			explosions.clear();

			goldDiedByPlayer = true;
			/* Flash the screen gold */
			flashTimer = maxFlashTime;
			/* The player should not be punished for touching the... */
			/* golden coal and then take damage from explosions */
			playerImmunityTimer = maxImmunityTime;
			/* Set the player's immunity timer, it is otherwise */
			/* skipped because of the return. */
			player.SetImmunity( playerImmunityTimer );
			player.SetHitByGoldCoal();

			/* Play a sound effect */
			sfx.gold.replay();

			/* return to avoid redundant collision checks */
			return;
		}

		/*====================*\
		|| Fireball collision ||
		\*====================*/
		for (auto fBall = fireballs.begin(); fBall < fireballs.end();)
		{
			/* Don't check collision if the fireball is about to be erased */
			if (fBall->IsToBeErased()) { ++fBall; continue; }

			/* Keeps track off if the current fireball was erased */
			bool erased = false;

			/* Fireball with Fireball collision */
			for (auto target = fireballs.begin(); target < fireballs.end(); ++target)
			{
				/* Check for overlap */
				if (fBall != target &&
					DoCirclesOverlap(fBall->GetCircle(), target->GetCircle()))
				{
					AddExplosion( (fBall->GetPos() + target->GetPos()) / 2.0f );
					fBall->SetErase();
					target->SetErase();
					erased = true;
					/* There still needs to be an increment because */
					/* the fireball has technically not been erased yet */
					++fBall;

					/* Break because the current fireball should not */
					/* undergo any more collisions */
					break;
				}

			}
			if (erased) { continue; }

			/* Fireball with flame collision */
			for (auto flame = flames.begin(); flame < flames.end();)
			{
				/* Check for overlap */
				if (DoCirclesOverlap(fBall->GetCircle(), flame->GetCircle()))
				{
					flame = flames.erase( flame );
				}
				else
				{
					++flame;
				}
			}
			/* Fireball with player collision */
			if (DoCirclesOverlap(fBall->GetCircle(), player.GetCircle()))
			{
				AddExplosion( player.GetPos() );
				fBall = fireballs.erase( fBall );

				/* continue because the current fireball should not */
				/* undergo any more collisions */
				continue;
			}

			/* Fireball with bombCoal collision */
			for (auto bomb = bombCoals.begin(); bomb < bombCoals.end();)
			{
				/* Check for overlap */
				if (!bomb->IsInvincible() && DoCirclesOverlap(fBall->GetCircle(), bomb->GetCircle()))
				{
					AddExplosion( bomb->GetPos() );
					bombCoals.erase( bomb );
					fBall = fireballs.erase( fBall );
					erased = true;

					/* Break because the current fireball should not */
					/* undergo any more collisions */
					break;
				}
				else
				{
					++bomb;
				}
			}
			if (erased) { continue; }

			/* Fireball with basicCoal collision */
			for (auto basic = basicCoals.begin(); basic < basicCoals.end();)
			{
				/* Check for overlap */
				if (!basic->IsInvincible() && DoCirclesOverlap(fBall->GetCircle(), basic->GetCircle()))
				{
					AddExplosion( basic->GetPos() );
					basicCoals.erase( basic );
					fBall = fireballs.erase( fBall );
					erased = true;

					/* Break because the current fireball should not */
					/* undergo any more collisions */
					break;
				}
				else
				{
					++basic;
				}
			}
			if (erased) { continue; }

			/* Fireball with goldCoal collision */
			if (goldCoal.IsActive() &&
				DoCirclesOverlap(fBall->GetCircle(), goldCoal.GetCircle()))
			{
				AddExplosion( goldCoal.GetPos() );
				goldCoal.SetActive( false );
				fBall = fireballs.erase( fBall );

				continue;
			}

			/* Only increment if the current fireball wasn't erased */
			if (!erased) { ++fBall; }
		}

		/* Safely erases Fireballs from their vector */
		fireballs.erase( std::remove_if( fireballs.begin(), fireballs.end(),
			[]( Fireball& f ) { return f.IsToBeErased(); } ), fireballs.end() );

		/*====================*\
		|| BombCoal collision ||
		\*====================*/
		for (auto bomb = bombCoals.begin(); bomb < bombCoals.end();)
		{
			/* Keeps track off if the current bombCoal was erased */
			bool erased = false;

			/* Don't check collision if the coal is invincible */
			if (bomb->IsInvincible())
			{
				++bomb;
				continue;
			}

			/* bombCoal with player collision */
			/* The bomb's circle if modified to make the radius larger... */
			/* as it should explode when close enough to damage the player... */
			/* with an explosion. */
			if (DoCirclesOverlap( { bomb->GetPos(), explosionRadius }, player.GetCircle() ))
			{
				AddExplosion( bomb->GetPos() );
				bomb = bombCoals.erase( bomb );

				/* continue because the current coal should not */
				/* undergo any more collisions */
				continue;
			}

			/* bombCoal with flame collision */
			for (auto flame = flames.begin(); flame != flames.end();)
			{
				/* Check for overlap */
				if (DoCirclesOverlap(bomb->GetCircle(), flame->GetCircle()))
				{
					AddExplosion( bomb->GetPos() );
					flame = flames.erase( flame );
					bomb = bombCoals.erase( bomb );
					erased = true;

					/* Grant points for shooting the coal */
					score += points;
					timeSinceKill = 0;

					/* Break because the coal doesn't exist anymore */
					break;
				}
				else
				{
					++flame;
				}
			}
			if (!erased) { ++bomb; }
		}

		/*=====================*\
		|| BasicCoal collision ||
		\*=====================*/
		for (auto basic = basicCoals.begin(); basic < basicCoals.end();)
		{
			/* Keeps track off if the current basicCoal was erased */
			bool erased = false;

			/* Don't check collision if the coal is invincible */
			if (basic->IsInvincible())
			{
				++basic;
				continue;
			}

			/* basicCoal with player collision */
			if (DoCirclesOverlap(basic->GetCircle(), player.GetCircle()))
			{
				basic->SetDead();

				if (playerImmunityTimer <= 0)
				{
					player.reduceHitPoints( playerDamage );
					playerImmunityTimer = maxImmunityTime;

					/* Play a sound effect */
					sfx.hurt.replay();
				}
				else
				{
					/* Play a sound effect */
					sfx.contact.replay();
				}
			}

			/* basicCoal with flame collision */
			for (auto flame = flames.begin(); flame != flames.end();)
			{
				/* Check for overlap */
				if (DoCirclesOverlap(basic->GetCircle(), flame->GetCircle()))
				{
					AddFireball( basic->GetPos(), flame->GetDir() );
					flame = flames.erase( flame );
					basic = basicCoals.erase( basic );
					erased = true;

					/* Grant points for shooting the coal */
					score += points * 2;
					timeSinceKill = 0.0f;

					/* Play a sound effect */
					sfx.hitCoal.replay();

					/* Break because the coal doesn't exist anymore */
					break;
				}
				else
				{
					++flame;
				}
			}
			if (!erased) { ++basic; }
		}

		/*=================*\
		|| Flame collision ||
		\*=================*/
		for (auto flame = flames.begin(); flame < flames.end(); )
		{
			/* Flame with goldCoal collision */
			if (goldCoal.IsActive() &&
				DoCirclesOverlap(flame->GetCircle(), goldCoal.GetCircle()))
			{
				flame = flames.erase( flame );
			}
			else
			{
				++flame;
			}
		}

		/*=====================*\
		|| Explosion collision ||
		\*=====================*/
		for (auto exp = explosions.begin(); exp < explosions.end(); ++exp)
		{
			/* Explosion with flame collision */
			for (auto flame = flames.begin(); flame < flames.end();)
			{
				/* Check for overlap */
				if (DoCirclesOverlap( exp->GetCircle(), flame->GetCircle() ))
				{
					flame = flames.erase( flame );
				}
				else
				{
					++flame;
				}
			}
			/* Explosion with bombCoal collision */
			for (auto bomb = bombCoals.begin(); bomb < bombCoals.end();)
			{
				/* Check for overlap */
				if (!bomb->IsInvincible() && DoCirclesOverlap( exp->GetCircle(), bomb->GetCircle() ))
				{
					bomb = bombCoals.erase( bomb );
				}
				else
				{
					++bomb;
				}
			}
			/* Explosion with basicCoal collision */
			for (auto basic = basicCoals.begin(); basic < basicCoals.end();)
			{
				/* Check for overlap */
				if (!basic->IsInvincible() && DoCirclesOverlap( exp->GetCircle(), basic->GetCircle() ))
				{
					basic = basicCoals.erase( basic );
				}
				else
				{
					++basic;
				}
			}
			/* Explosion with goldCoal collision */
			if (goldCoal.IsActive() &&
				DoCirclesOverlap( exp->GetCircle(), goldCoal.GetCircle() ))
			{
				goldCoal.SetActive( false );
			}
			/* Explosion with player collision */
			if (playerImmunityTimer <= 0 &&
				DoCirclesOverlap( exp->GetCircle(), player.GetCircle() ))
			{
				playerImmunityTimer = maxImmunityTime;
				player.reduceHitPoints( playerDamage );

				/* Play a sound effect */
				sfx.hurt.replay();

				/* break to avoid redundant collision checks */
				break;
			}
		}
		
		player.SetImmunity( playerImmunityTimer );
	}

	void Game::DrawScreen()
	{
		/* Draw everything to the screen */
		background_sprite->Draw( screen, 0, 0 );

		/* Draw the drop shadows of the coals and player */
		for (auto& coal : basicCoals)	{ coal.DrawShadow( screen ); }
		for (auto& coal : bombCoals)	{ coal.DrawShadow( screen ); }
		if (goldCoal.IsActive())		{ goldCoal.DrawShadow( screen ); }
		player.DrawShadow( screen );

		foreground_sprite->Draw( screen, 0, 0 );

		bool drawnHearts = false;
		/* After the first attempt, have the hearts appear one by one */
		if (!gameStartedViaMenu && !secretMode)
		{
			if (gameTimer < 0.2f && !oneHitPointLeft)
			{
				drawnHearts = true;
			}
			else if (gameTimer < 0.4f)
			{
				heart_sprite->Draw( screen, 64, 3 );

				/* When the second heart is drawn, the player has at least two hit points */
				if (oneHitPointLeft && gameTimer >= 2.0f) { oneHitPointLeft = false; }

				drawnHearts = true;
			}
			else if (gameTimer < 0.6f)
			{
				heart_sprite->Draw( screen, 64, 3 );
				heart_sprite->Draw( screen, 128, 3 );

				drawnHearts = true;
			}
		}
		/* Draw the player's hearts, if they weren't drawn already this tick */
		if (!drawnHearts)
		{
			for (int i = 1; i <= 3; ++i)
			{
				if (i <= player.GetHitPoints())
				{
					heart_sprite->Draw( screen, (64 * i), 3 );
				}
			}
		}

		/* First draw the invincible coals, then the ones that have finished spawning */
		for (auto& coal : basicCoals)	{ if (coal.IsInvincible()) { coal.Draw( screen ); } }
		for (auto& coal : bombCoals)	{ if (coal.IsInvincible()) { coal.Draw( screen ); } }
		for (auto& coal : basicCoals)	{ if (!coal.IsInvincible()) { coal.Draw( screen ); } }
		for (auto& coal : bombCoals)	{ if (!coal.IsInvincible()) { coal.Draw( screen ); } }
		if (goldCoal.IsActive())		{ goldCoal.Draw( screen ); }
		for (auto& flame : flames)		{ if (flame.IsActive()) 
										{ flame.Draw( screen ); } }
		if (playerAlive)	{ player.Draw( screen ); }
		else				{ player.DrawDead( screen ); }
		for (auto& fBall : fireballs) { fBall.Draw( screen ); }
		for (auto& exp : explosions) { exp.Draw( screen ); }

		/* Print the time for which the game has been going on for */
		const std::string seconds = std::to_string( static_cast<int>(gameTimer) % 60 );
		const std::string minutes = std::to_string( (static_cast<int>(gameTimer) - (static_cast<int>(gameTimer) % 60)) / 60 );
		screen->Print( (minutes + "m " + seconds + "s").c_str(), 320, 730, 0xffffff, 5 );

		// Draw a bar showing for how long the player boost/dash is on cooldown for
		if (boostCooldown > 0.0f && (gameState != GameState::GAME_OVER) && (gameState != GameState::GAME_OVER_MENU))
		{
			// Draw an outline
			screen->Bar(	static_cast<int>(player.GetPos().x - (player.GetWidth() * 0.5f)) + 10, 
							static_cast<int>(player.GetPos().y + (player.GetHeight() * 0.5f)) + 5,
							static_cast<int>(player.GetPos().x + (player.GetWidth() * 0.5f)) - 10,
							static_cast<int>(player.GetPos().y + (player.GetHeight() * 0.5f)) + 15,
							0xffffff );
			// Draw the bar (based on the amount of cooldown time left
			screen->Bar(	static_cast<int>(player.GetPos().x - (player.GetWidth() * 0.5f)) + 11,
							static_cast<int>(player.GetPos().y + (player.GetHeight() * 0.5f)) + 6,
							static_cast<int>((player.GetPos().x - (player.GetWidth() * 0.5f) + 11) + 
									((player.GetWidth() - 22) * (boostCooldown / maxBoostCooldown))),
							static_cast<int>(player.GetPos().y + (player.GetHeight() * 0.5f)) + 14,
							0xfd5f44 );
		}
	}

	void Game::DarkenScreen() const
	{
		Pixel* address = screen->GetBuffer();

		/* Go over all pixels (colors) on the screen */
		for (int y = 0; y < ScreenHeight; ++y)
		{
			for (int x = 0; x < ScreenWidth; ++x)
			{
				/* Get the current pixel's color */
				const int color = static_cast<int>(address[x + y * ScreenWidth]);
				/* Divide the color into red, green and blue */
				int r = color & RedMask;
				int g = color & GreenMask;
				int b = color & BlueMask;

				r >>= 16;
				g >>= 8;

				/* Darken the color */
				r /= 2;
				g /= 2;
				b /= 2;

				const Pixel newColor = (r << 16) + (g << 8) + b;

				//const Pixel newColor = AddBlend( color, 0xa5a5a5 );
				address[x + y * ScreenWidth] = newColor;
			}
		}
	}

	void Game::DrawEntityHitBox() const
	{
		for (auto& e : explosions)	{ if (e.IsActive()) e.DrawHitBox( screen ); }
		for (auto& e : basicCoals)	{ if (!e.IsInvincible()) { e.DrawHitBox( screen ); } }
		for (auto& e : bombCoals)	{ if (!e.IsInvincible()) { e.DrawHitBox( screen ); } }
		if (goldCoal.IsActive())	{ goldCoal.DrawHitBox( screen ); }
		for (auto& e : flames)		{ if (e.IsActive()) { e.DrawHitBox( screen ); } }
		for (auto& e : fireballs)	{ e.DrawHitBox( screen ); }
		player.DrawHitBox( screen );
	}

	void Game::KillEnemies()
	{
		/* Add an explosion on every entity */
		/* except coals that haven't finished spawning */
		for (auto& e : basicCoals)	
		{
			if (e.IsDead())
			{
				continue;
			}
			else if (e.FinishedSpawning())
			{
				AddExplosion( e.GetPos() );
			}
			else
			{
				e.Despawn();
			}
		}
		for (auto& e : bombCoals)
		{
			if (e.FinishedSpawning())
			{
				AddExplosion( e.GetPos() );
			}
			else
			{
				e.Despawn();
			}
		}
		for (auto& e : fireballs)	{ AddExplosion( e.GetPos() ); }
		if (goldCoal.IsActive())	{ AddExplosion( goldCoal.GetPos() ); }

		fireballs.clear();
		goldCoal.SetActive( false );

		killedEnemies = true;
	}

	inline bool Game::AllEnemiesDead() const
	{
		return (basicCoals.empty() &&
				bombCoals.empty() &&
				fireballs.empty() &&
				!goldCoal.IsActive());
	}

	bool Game::MovePlayerTowardsCenter()
	{
		constexpr float centerX = (static_cast<float>(ScreenWidth) / 2.0f);
		constexpr float centerY = (static_cast<float>(ScreenHeight) / 2.0f);

		if (!calcedPlayerDir)
		{
			playerDir.x = centerX - player.GetPos().x;
			playerDir.y = centerY - player.GetPos().y;

			calcedPlayerDir = true;
			if (playerDir.length() == 0.0f) { return true; }
		}

		if ((playerDir.x >= 0 && player.GetPos().x > centerX) ||
			(playerDir.x < 0 && player.GetPos().x < centerX)  ||
			(playerDir.y >= 0 && player.GetPos().y > centerY) ||
			(playerDir.y < 0 && player.GetPos().y < centerY)) 
		{
			player.SetPosX( centerX );
			player.SetPosY( centerY );

			return true;
		}

		/* Player movement is normalized to a maximum distance within it's own class */
		player.SetMovement( playerDir.x, playerDir.y );
		return false;
	}

	void Game::ResetGameVariables()
	{
		score = 0;
		displayedScore = 0;
		newHighScore = false;
		scoreFlashTimer = 0.0f;
		gameTimer = 0;
		timeSinceGameOver = 0;
		killedEnemies = false;
		konamiKeyCount = 0;
		previousLeftPressed = false;
		LeftPressed = false;
		flameSpawnCooldown = 0.0f;
		timeBetweenGoldSpawnUpdates = 0.0f;
		goldSpawnCooldown = 0;
		scoreAtPreviousGoldCoal = 0;
		timeSinceGoldSpawned = 2.1f;
		goldDiedByPlayer = false;
		coalSpawnCooldown = 1.0f;
		timeSinceKill = 0.0f;
		boost = false;
		boostLength = 0.0f;
		boostCooldown = 0.0f;
		playerImmunityTimer = 0.0f;
		playerDir = { 0.0f, 0.0f };
		calcedPlayerDir = false;

		player.SetImmunity( 0.0f );
		player.SetHitPoints( 3 );
	}

	/* The game crashes if highScore.txt holds non-numbers */
	void Game::UpdateHighScore()
	{
		bool overwriteHighScore = false;
		/* Overwriting the high score when necessary */
		highScore.open( "highScore.txt", fstream::in );
		if (highScore.is_open()) // highScore.txt already exists 
		{
			std::string line; // current high score
			int lines = 0; // amount of lines in highScore.txt (should be 0 or 1)
			while (std::getline( highScore, line ))
			{
				lines++;
			}
			highScore.close();

			/* If the text file is not empty and holds one score( as it should ) */
			if (lines == 1)
			{
				currentHighScore = std::stoul( line );

				/* Replace the high score when score is higher */
				if (score > currentHighScore)
				{
					overwriteHighScore = true;
					newHighScore = true;
				}
			}
			/* The text file is empty or holds more lines than intended */
			/* (This can only happen if a player edits it themselves) */
			else
			{
				overwriteHighScore = true;
				// the high score isn't 'new'
			}
		}
		else // highScore.txt does not exist (doesn't open)
		{
			overwriteHighScore = true;
			newHighScore = true;
		}

		if (overwriteHighScore)
		{
			highScore.open( "highScore.txt", fstream::out );
			if (highScore.is_open())
			{
				highScore << score;
				highScore.close();

				currentHighScore = score;
			}
		}
	}

	void Game::SetUpSecretMode()
	{
		player.SetHitPoints( 1 );

		/* top left to top right */
		AddCoalBasic( { 89.0f, 89.0f } );
		AddCoalBasic( { 158.0f, 89.0f } );
		AddCoalBasic( { 227.0f, 89.0f } );
		AddCoalBasic( { 296.0f, 89.0f } );
		AddCoalBasic( { 365.0f, 129.0f } );
		AddCoalBasic( { 434.0f, 129.0f } );
		AddCoalBasic( { 503.0f, 89.0f } );
		AddCoalBasic( { 572.0f, 89.0f } );
		AddCoalBasic( { 641.0f, 89.0f } );
		AddCoalBasic( { 710.0f, 89.0f } );

		/* bottom left to bottom right */
		AddCoalBasic( { 89.0f, 680.0f } );
		AddCoalBasic( { 158.0f, 680.0f } );
		AddCoalBasic( { 227.0f, 680.0f } );
		AddCoalBasic( { 296.0f, 680.0f } );
		AddCoalBasic( { 365.0f, 640.0f } );
		AddCoalBasic( { 434.0f, 640.0f } );
		AddCoalBasic( { 503.0f, 680.0f } );
		AddCoalBasic( { 572.0f, 680.0f } );
		AddCoalBasic( { 641.0f, 680.0f } );
		AddCoalBasic( { 710.0f, 680.0f } );

		/* left side */
		AddCoalBasic( { 89.0f, 162.875f } );
		AddCoalBasic( { 89.0f, 236.75f } );
		AddCoalBasic( { 89.0f, 310.625f } );
		AddCoalBasic( { 129.0f, 384.5f } );
		AddCoalBasic( { 89.0f, 458.375f } );
		AddCoalBasic( { 89.0f, 532.25 } );
		AddCoalBasic( { 89.0f, 606.125 } );

		/* right side */
		AddCoalBasic( { 710.0f, 162.875f } );
		AddCoalBasic( { 710.0f, 236.75f } );
		AddCoalBasic( { 710.0f, 310.625f } );
		AddCoalBasic( { 670.0f, 384.5f } );
		AddCoalBasic( { 710.0f, 458.375f } );
		AddCoalBasic( { 710.0f, 532.25 } );
		AddCoalBasic( { 710.0f, 606.125 } );
	}

	void Game::MouseUp( Uint8 key)
	{
		if (key == SDL_BUTTON_LEFT) { mouseDown = false; }
	}

	void Game::MouseDown(Uint8 key)
	{
		if (key == SDL_BUTTON_LEFT) { mouseDown = true; }
	}

	void Game::MouseMove(int x, int y)
	{
		mousex = x;
		mousey = y;
	}

	void Game::KeyUp( SDL_Scancode key )
	{
		if (key == SDL_SCANCODE_W)	{ up = false; }
		if (key == SDL_SCANCODE_A)	{ left = false; }
		if (key == SDL_SCANCODE_S)	{ down = false; }
		if (key == SDL_SCANCODE_D)	{ right = false; }
		
		if (key == SDL_SCANCODE_SPACE) { boost = false; }

		if (key == SDL_SCANCODE_F5) { drawHitBox = !drawHitBox; }

		/* Checks for inputs matching the Konami code */
		if ((gameState == GameState::GAME_OVER_MENU ||
			(gameState == GameState::MENU && !sfx.CurrentlyModifyingVolume()) ||
			gameState == GameState::INFO) && 
			!secretMode)
		{
			/* Progress the key count to check for the next input in the code */
			if (key == konamiInputs[konamiKeyCount])
			{
				++konamiKeyCount;
			}
			/* If the key was incorrect, reset the input to check */
			/* (start from the beginning of the code again) */
			else
			{
				konamiKeyCount = 0;
			}
			/* If the final key of the code was pressed, set to secret mode */
			if (konamiKeyCount == konamiInputs.size())
			{
				secretMode = true;
				konamiKeyCount = 0;
			}
		}
	}

	void Game::KeyDown( SDL_Scancode key )
	{
		if (key == SDL_SCANCODE_W)	{ up = true; }
		if (key == SDL_SCANCODE_A)	{ left = true; }
		if (key == SDL_SCANCODE_S)	{ down = true; }
		if (key == SDL_SCANCODE_D)	{ right = true; }

		if (key == SDL_SCANCODE_SPACE) { boost = true; }

		if (key == SDL_SCANCODE_ESCAPE)
		{
			if (gameState == GameState::GAME)
			{
				gameState = GameState::PAUSE;
				pauseButton.SetPressed( false );
				pausedByGame = true;
				menuButton.SetPos( 274, 393 );
				quitButton.SetPos( 274, 458 );

				/* Play the button sound effect */
				sfx.button.replay();
			}
			else if (gameState == GameState::PAUSE)
			{
				/* Resume in the correct game state */
				if (pausedByGame)
				{
					gameState = GameState::GAME;
					flameSpawnCooldown = maxFlameSpawnCooldown;
				}
				else
				{
					gameState = GameState::SECRET_MODE;
					flameSpawnCooldown = secretFlameSpawnCooldown;
				}
				menuButton.SetPos( 100, 360 );

				/* Play the button sound effect */
				sfx.button.replay();
			}
			else if (gameState == GameState::SECRET_MODE)
			{
				gameState = GameState::PAUSE;
				pauseButton.SetPressed( false );
				pausedByGame = false;
				menuButton.SetPos( 274, 393 );
				quitButton.SetPos( 274, 458 );

				/* Play the button sound effect */
				sfx.button.replay();
			}
		}
	}

	/* Implementation adapted from MAX#2223 in the 3dgep.com discord server */
	/* message link: https://discord.com/channels/515453022097244160/686661689894240277/1095673953734901761 */
	void Game::Quit()
	{
		/* Invoke an event with the type SDL_QUIT */
		SDL_Event user_event;
		user_event.type = SDL_QUIT;
		SDL_PushEvent( &user_event );
	}

	inline void Game::AddFlame(vec2 _pos)
	{
		flames.emplace_back( flame_sprite, _pos, flameRadius, globalSpeed );
		sfx.shoot.replay();
	}
	inline void Game::AddCoalBasic(vec2 _pos)
	{
		basicCoals.emplace_back( coalBasic_sprite, _pos, coalRadius, globalSpeed );
	}
	inline void Game::AddCoalBomb(vec2 _pos)
	{
		bombCoals.emplace_back( coalBomb_sprite, _pos, coalRadius, globalSpeed );
	}
	inline void Game::AddExplosion(vec2 _pos)
	{
		explosions.emplace_back( explosion_sprite, _pos, explosionRadius, globalSpeed );
		sfx.explosion.replay();
	}
	inline void Game::AddFireball(vec2 _pos, vec2 _dir)
	{
		fireballs.emplace_back( fireball_sprite, _pos, _dir, fireballRadius, globalSpeed );
	}
};