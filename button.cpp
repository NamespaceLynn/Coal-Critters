#include "button.h"

#include "game.h"

namespace Tmpl8 {

	Button::Button(int _x, int _y, int _width, int _height, 
		const char* _text, int _size, int _offset,
		Pixel _baseColor, Pixel _hoverColor)
		: x( _x )
		, y( _y )
		, width( _width )
		, height( _height)
		, text( _text )
		, textSizeModifier( _size )
		, x_textOffset( _offset )
		, baseColor( _baseColor )
		, hoverColor( _hoverColor )
	{
		if (textSizeModifier < 1)
		{
			textSizeModifier = 1;
		}
	}

	void Button::Update(float deltaTime, int mousex, int mousey, bool mouseDown )
	{
		/* Simple collision check for the button and the mouse */
		if (mousex >= x &&
			mousex < x + width &&
			mousey >= y &&
			mousey < y + height)
		{
			hoverOver = true;
		}
		else
		{
			hoverOver = false;
		}

		/* The button is pressed when the mouse is pressed down and hovers over the button */
		isPressed = hoverOver && mouseDown;
	}

	void Button::Draw( Surface* screen )
	{
		if (hoverOver) /* bright red */
		{
			screen->Box( x, y, (x + width - 1), (y + height - 1), hoverColor );
			screen->Print( text, x + (width / textSizeModifier) + x_textOffset, 
				y + (height / textSizeModifier), hoverColor, textSizeModifier);
		}
		else /* dark purple */
		{
			screen->Box( x, y, (x + width - 1), (y + height - 1), baseColor );
			screen->Print( text, x + (width / textSizeModifier) + x_textOffset, 
				y + (height / textSizeModifier), baseColor, textSizeModifier );
		}
	}
}