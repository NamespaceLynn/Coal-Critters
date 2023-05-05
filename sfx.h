#pragma once

#include "surface.h"

#include <memory>

// Using an Audio library from Jeremiah van Oosten: https://github.com/jpvanoosten/Audio
#include <Audio/Sound.hpp>
#include <Audio/Device.hpp>

// Class for holding sound effects, also has a volume bar to modify the volume
class SFX
{
public:
	// Constructor - set the default master volume 
	SFX( std::shared_ptr<Tmpl8::Sprite> sprite );

	// Returns true if the volume bar is being "held down" on with the cursor
	void UpdateVolumeBar(int mouse_x, int mouse_y, bool mouseHeldDown);
	void DrawVolumeBar( Tmpl8::Surface* screen ) const;
	// Returns true if the cursor overlaps with the slide bar
	[[nodiscard]] inline bool CursorOnVolumeBar( int mouse_x, int mouse_y ) const;
	[[nodiscard]] inline bool CursorOnVolumeButton( int mouse_x, int mouse_y ) const;
	inline void SetSelectedPos( int mouse_x );
	// Translates the selected position on the slide bar to a volume
	void SetVolume();

	[[nodiscard]] bool CurrentlyModifyingVolume() const { return modifyingVolume; }

	// Plays when an explosion occurs
	Audio::Sound explosion{ "assets/explosion.wav", Audio::Sound::Type::Sound };
	// Plays when clicking on any button
	Audio::Sound button{ "assets/button.wav", Audio::Sound::Type::Sound };
	// Plays when the player shoots a flame
	Audio::Sound shoot{ "assets/shoot.wav", Audio::Sound::Type::Sound };
	// Plays when entering the secret mode
	Audio::Sound flash{ "assets/flash.wav", Audio::Sound::Type::Sound };
	// Plays when the player collides with the golden coal
	Audio::Sound gold{ "assets/gold.wav", Audio::Sound::Type::Sound };
	// Plays when a fireball bounces against a wall
	Audio::Sound bounce{ "assets/bounce.wav", Audio::Sound::Type::Sound };
	// Plays when the player is hurt
	Audio::Sound hurt{ "assets/hurt.wav", Audio::Sound::Type::Sound };
	// Plays when the player collides with a basic coal while immune
	Audio::Sound contact{ "assets/contact.wav", Audio::Sound::Type::Sound };
	// Plays when a flame hits a basic coal
	Audio::Sound hitCoal{ "assets/hitCoal.wav", Audio::Sound::Type::Sound };
	// Plays when the player uses the "dash" mechanic
	Audio::Sound dash{ "assets/dash.wav", Audio::Sound::Type::Sound };

private:
	std::shared_ptr<Tmpl8::Sprite> volume_sprite;

	// 0.0f - 1.0f
	float volume{ 0.5f };

	// Variables used for the volume button

	bool mouseDownLastFrame{ false };
	bool soundOff{ false };
	// "Old" variables equal those before turning off the sound
	float oldVolume{ 0.5f };
	int oldSelected_x;

	// Variables used for the volume bar

	// True as long as the mouse is held down after being held down for at least...
	// one frame on the slide bar
	bool modifyingVolume{ false };
	// Minimum and maximum value the "selected" bar can take up
	int min_x, max_x;
	int volumeBar_y, volumeBar_h;
	// Where on the border is currently selected
	int selected_x;

	// Border values
	const int border_x{ 10 }, border_y{ 10 }, border_w{ 195 }, border_h{ 50 };

};