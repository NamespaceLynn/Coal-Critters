#include "sfx.h"

SFX::SFX( std::shared_ptr<Tmpl8::Sprite> sprite )
	: volume_sprite( std::move( sprite ) )
{
	Audio::Device::setMasterVolume( volume );
	min_x = border_x + 10;
	max_x = border_x + border_w - 11;
	volumeBar_y = border_y + 10;
	volumeBar_h = border_h - 20;
	selected_x = (max_x - min_x) / 2 + min_x;
	oldSelected_x = (max_x - min_x) / 2 + min_x;
}

void SFX::UpdateVolumeBar(int mouse_x, int mouse_y, bool mouseHeldDown)
{
	// If the cursor clicks on the volume button
	if (!modifyingVolume && CursorOnVolumeButton(mouse_x, mouse_y) && (mouseHeldDown && !mouseDownLastFrame))
	{
		if (soundOff) // Turn the sound on if it was off
		{
			soundOff = false;
			volume = oldVolume;
			selected_x = oldSelected_x;
			SetVolume();
		}
		else // Turn the sound off if it was on
		{
			soundOff = true;
			oldVolume = volume;
			oldSelected_x = selected_x;
			// Set the volume bar as low as possible (volume will equal 0.0f)
			selected_x = min_x;
			SetVolume();
		}
	}
	// If the cursor is held down over the volume bar, "start" modifying the volume
	else if (!modifyingVolume && CursorOnVolumeBar( mouse_x, mouse_y ) && mouseHeldDown)
	{
		modifyingVolume = true;
	}
	// While the mouse is being held down while modifying the volume, update "volume position"
	else if (modifyingVolume && mouseHeldDown)
	{
		SetSelectedPos( mouse_x );
	}
	// If the mouse is no longer being held down, stop modifying the volume
	else if (modifyingVolume && !mouseHeldDown)
	{
		modifyingVolume = false;
		// Set the volume when done modifying it
		SetVolume();
	}

	mouseDownLastFrame = mouseHeldDown;
}

void SFX::DrawVolumeBar(Tmpl8::Surface* screen) const
{
	screen->Bar(	border_x,
					border_y,
					border_x + border_w - 1,
					border_y + border_h - 1,
					0x000000 );

	screen->Box(	border_x, 
					border_y,
					border_x + border_w - 1, 
					border_y + border_h - 1, 
					0xffffff );

	// When the "selected" portion of the slide bar is visible, draw it
	if (selected_x > min_x)
	{
		screen->Bar( min_x, volumeBar_y, selected_x, volumeBar_y + volumeBar_h, 0xffffff );
	}

	// Draw the volume button

	screen->Bar(	border_x + border_w + 9,
					border_y,
					border_x + border_w + 9 + border_h,
					border_y + border_h - 1,
					0x000000 );

	screen->Box(	border_x + border_w + 9,
					border_y,
					border_x + border_w + 9 + border_h,
					border_y + border_h - 1,
					0xffffff );

	// Set the correct frame
	if (selected_x == min_x)
	{
		volume_sprite->SetFrame( 3 );
	}
	else
	{
		const float temp_volume = static_cast<float>(selected_x - min_x) / static_cast<float>(max_x - min_x);
		if (temp_volume <= 0.333f)
		{
			volume_sprite->SetFrame( 0 );
		}
		else if (temp_volume <= 0.75f)
		{
			volume_sprite->SetFrame( 1 );
		}
		else
		{
			volume_sprite->SetFrame( 2 );
		}
	}

	// Draw the speaker icon
	volume_sprite->Draw( screen, border_x + border_w + 9, border_y );
}

inline bool SFX::CursorOnVolumeBar( int mouse_x, int mouse_y ) const
{
	return	mouse_x >= border_x &&
			mouse_x < border_x + border_w &&
			mouse_y >= border_y &&
			mouse_y < border_y + border_h;
}

inline bool SFX::CursorOnVolumeButton(int mouse_x, int mouse_y) const
{
	return	mouse_x >= border_x + border_w + 9 &&
			mouse_x < border_x + border_w + 9 + border_h &&
			mouse_y >= border_y &&
			mouse_y < border_y + border_h - 1;
}

inline void SFX::SetSelectedPos( int mouse_x )
{
	selected_x = (mouse_x <= min_x) ? min_x : (mouse_x >= max_x) ? max_x : mouse_x;
}

void SFX::SetVolume()
{
	if (selected_x == min_x)
	{
		volume = 0.0f;
	}
	else if (selected_x == max_x)
	{
		volume = 1.0f;
	}
	else
	{
		volume = static_cast<float>(selected_x - min_x) / static_cast<float>(max_x - min_x);
	}

	Audio::Device::setMasterVolume( volume );
	if (volume > 0.0f) { soundOff = false; }
	explosion.replay();
}