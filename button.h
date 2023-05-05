#pragma once

#include "surface.h"

namespace Tmpl8 {

	class Game;
	class Button
	{
	public:
		Button( int x, int y, int width, int height, 
			const char* text, int size, int offset, 
			Pixel baseColor = 0x1f161b, Pixel hoverColor = 0xfd5f44 );

		void Update( float deltaTime, int mousex, int mousey, bool mouseDown );
		void Draw( Surface* screen );

		[[nodiscard]] bool IsPressed() const { return isPressed; }
		[[nodiscard]] bool IsHoveredOver() const { return (hoverOver || isPressed); }
		void SetPressed( bool pressed ) { isPressed = pressed; hoverOver = pressed; }
		void SetPos( int _x, int _y ) { x = _x; y = _y; }

	private:

		int x, y;
		int width, height;
		const char* text;
		int textSizeModifier;
		int x_textOffset;
		bool hoverOver{ false };
		bool isPressed{ false };
		Pixel baseColor;
		/* The color used when the mouse hovers over the button */
		Pixel hoverColor;
	};
}