// Template, BUAS version https://www.buas.nl/games
// IGAD/BUAS(NHTV)/UU - Jacco Bikker - 2006-2020

#pragma once

#include "math.h"

namespace Tmpl8 {

constexpr int RedMask = 0xff0000;
constexpr int GreenMask = 0x00ff00;
constexpr int BlueMask = 0x0000ff;

typedef unsigned int Pixel; // unsigned int is assumed to be 32-bit, which seems a safe assumption.

// AlphaBlend function added by myself 
// Alpha is assumed to be a value between 0.0f - 1.0f
inline Pixel AlphaBlend( Pixel src, Pixel dest, float alpha)
{
	/* If alpha equals 1.0f, only src will be visible, thus no blending is necessary */
	/* If alpha equals 0.0f, only dest will be visible, thus no blending is necessary */
	if (alpha == 1.0f) { return src; }
	if (alpha == 0.0f) { return dest; }

	/* Clamp alpha to be a value between 0.0f - 1.0f */
	alpha = (alpha < 0.0f) ? 0.0f : (alpha > 1.0f) ? 1.0f : alpha;

	const unsigned int r_src = src & RedMask;
	const unsigned int g_src = src & GreenMask;
	const unsigned int b_src = src & BlueMask;

	const unsigned int r_dest = dest & RedMask;
	const unsigned int g_dest = dest & GreenMask;
	const unsigned int b_dest = dest & BlueMask;

	/* Applying the alpha value to both src and dest, then combine them together */
	auto r = static_cast<unsigned int>((alpha * static_cast<float>(r_src)) + ((1.0f - alpha) * static_cast<float>(r_dest)));
	auto g = static_cast<unsigned int>((alpha * static_cast<float>(g_src)) + ((1.0f - alpha) * static_cast<float>(g_dest)));
	auto b = static_cast<unsigned int>((alpha * static_cast<float>(b_src)) + ((1.0f - alpha) * static_cast<float>(b_dest)));

	r &= RedMask;
	g &= GreenMask;
	b &= BlueMask;

	return (r | g | b);
}

// ShadowBlend function added by myself
// Checks if the current pixel needs a shadow cast over it based on its coordinates
// The shadow is faded in if fadeLength > 0
inline Pixel ShadowBlend( int c_x, int c_y, int shadow_max_x, int shadow_max_y, int fadeLength, Pixel c, Pixel shadow_c, float alpha )
{
	if (c_x <= shadow_max_x && c_y <= shadow_max_y) // Within the range of the shadow (both x & y)
	{
		float tempAlpha = alpha;
		const auto distInsideShadow_x = static_cast<float>(shadow_max_x - c_x + 1);
		const auto distInsideShadow_y = static_cast<float>(shadow_max_y - c_y + 1);
		const float distInsideShadow = sqrtf( (distInsideShadow_x * distInsideShadow_x) + (distInsideShadow_y * distInsideShadow_y) );
		if (distInsideShadow < static_cast<float>(fadeLength))
		{
			// Calculate a new alpha value
			tempAlpha = alpha * (static_cast<float>(distInsideShadow) / static_cast<float>(fadeLength));
		}
		// Add shadow and return
		return AlphaBlend( shadow_c, c, tempAlpha );
	}
	else if (c_x <= shadow_max_x) // Within the range of the shadow (only x)
	{
		float tempAlpha = alpha;
		const int distInsideShadow = shadow_max_x - c_x + 1; // minimum 1
		if (distInsideShadow < fadeLength)
		{
			// Calculate a new alpha value
			tempAlpha = alpha * (static_cast<float>(distInsideShadow) / static_cast<float>(fadeLength));
		}
		// Add shadow and return
		return AlphaBlend( shadow_c, c, tempAlpha );
	}
	else if (c_y <= shadow_max_y) // Within the range of the shadow (only y)
	{
		float tempAlpha = alpha;
		const int distInsideShadow = shadow_max_y - c_y + 1; // minimum 1
		if (distInsideShadow < fadeLength)
		{
			// Calculate a new alpha value
			tempAlpha = alpha * (static_cast<float>(distInsideShadow) / static_cast<float>(fadeLength));
		}
		// Add shadow and return
		return AlphaBlend( shadow_c, c, tempAlpha );
	}
	else // Not within the range of the shadow
	{
		// Return the original color
		return c;
	}
}

inline Pixel AddBlend( Pixel a_Color1, Pixel a_Color2 )
{
	const unsigned int r = (a_Color1 & RedMask) + (a_Color2 & RedMask);
	const unsigned int g = (a_Color1 & GreenMask) + (a_Color2 & GreenMask);
	const unsigned int b = (a_Color1 & BlueMask) + (a_Color2 & BlueMask);
	const unsigned r1 = (r & RedMask) | (RedMask * (r >> 24));
	const unsigned g1 = (g & GreenMask) | (GreenMask * (g >> 16));
	const unsigned b1 = (b & BlueMask) | (BlueMask * (b >> 8));
	return (r1 + g1 + b1);
}

// subtractive blending
inline Pixel SubBlend( Pixel a_Color1, Pixel a_Color2 )
{
	int red = (a_Color1 & RedMask) - (a_Color2 & RedMask);
	int green = (a_Color1 & GreenMask) - (a_Color2 & GreenMask);
	int blue = (a_Color1 & BlueMask) - (a_Color2 & BlueMask);
	if (red < 0) red = 0;
	if (green < 0) green = 0;
	if (blue < 0) blue = 0;
	return static_cast<Pixel>(red + green + blue);
}

class Surface
{
	enum { OWNER = 1 };
public:
	// constructor / destructor
	Surface( int a_Width, int a_Height, Pixel* a_Buffer, int a_Pitch );
	Surface( int a_Width, int a_Height );
	Surface( char* a_File );

	Surface( const Surface& ) = delete;
	Surface& operator=( const Surface& ) = delete;
	Surface( Surface&& ) = delete;
	Surface& operator=( Surface&& ) = delete;

	~Surface();
	// member data access
	Pixel* GetBuffer() { return m_Buffer; }
	void SetBuffer( Pixel* a_Buffer ) { m_Buffer = a_Buffer; }
	int GetWidth() { return m_Width; }
	int GetHeight() { return m_Height; }
	int GetPitch() { return m_Pitch; }
	void SetPitch( int a_Pitch ) { m_Pitch = a_Pitch; }
	// Special operations
	void InitCharset();
	void SetChar( int c, char* c1, char* c2, char* c3, char* c4, char* c5 );
	void Centre( char* a_String, int y1, Pixel color );
	/* Modified Surface::Print by Boyko, posted in the 3dgep.com discord server */
	/* Message link: https://discord.com/channels/515453022097244160/686661689894240277/943970778246967367 */
	/* Can now set the size of the text */
	void Print( const char* a_String, int x1, int y1, Pixel color, int width = 1 );
	void Clear( Pixel a_Color );
	/* Added another implementation of Surface::Clear */
	void Clear( Pixel a_Color, float alpha ) const;
	void Line( float x1, float y1, float x2, float y2, Pixel color );
	void Plot( int x, int y, Pixel c );
	void LoadImage( char* a_File );
	void CopyTo( Surface* a_Dst, int a_X, int a_Y );
	void BlendCopyTo( Surface* a_Dst, int a_X, int a_Y );
	void ScaleColor( unsigned int a_Scale );
	void Box( int x1, int y1, int x2, int y2, Pixel color );
	void Bar( int x1, int y1, int x2, int y2, Pixel color );
	void Resize( Surface* a_Orig );
private:
	// Attributes
	Pixel* m_Buffer{nullptr};	
	int m_Width{0}, m_Height{0};
	int m_Pitch{0};
	int m_Flags{0};
	// Static attributes for the buildin font
	static char s_Font[51][5][6];
	static bool fontInitialized;
	int s_Transl[256]{};		
};

class Sprite
{
public:
	// Sprite flags
	enum
	{
		FLARE		= (1<< 0),
		OPFLARE		= (1<< 1),	
		FLASH		= (1<< 4),	
		DISABLED	= (1<< 6),	
		GMUL		= (1<< 7),
		BLACKFLARE	= (1<< 8),	
		BRIGHTEST   = (1<< 9),
		RFLARE		= (1<<12),
		GFLARE		= (1<<13),
		NOCLIP		= (1<<14)
	};
	
	// Constructors
	Sprite( Surface* a_Surface, unsigned int a_NumFrames );

	Sprite( const Sprite& ) = delete;
	Sprite& operator=( const Sprite& ) = delete;
	Sprite( Sprite&& ) = delete;
	Sprite& operator=( Sprite&& ) = delete;

	~Sprite();
	// Methods
	void Draw( Surface* a_Target, int a_X, int a_Y );
	// Added a function to draw a sprite blend in with the background
	// Can also add shadows
	void DrawBlend( Surface* a_Target, int a_X, int a_Y, float alpha,
					int shadow_max_x = 0, int shadow_max_y = 0, int fadeLength = 0,
					Pixel shadow_c = 0, float shadow_alpha = 0.0f );
	// Added a function to draw a sprite as one specified color 
	void DrawInColor( Surface* a_Target, int a_X, int a_Y, Pixel color );
	// Added a function to draw a sprite as one specified color and blend in with the background
	// Can also add shadows
	void DrawInColorAndBlend(	Surface* a_Target, int a_X, int a_Y, Pixel color, float alpha, 
								int shadow_max_x = 0, int shadow_max_y = 0, int fadeLength = 0, 
								Pixel shadow_c = 0, float shadow_alpha = 0.0f );
	// Added a function to draw a sprite blended with a specified color
	// Can also add shadows
	void DrawInBlendedColor(	Surface* a_Target, int a_X, int a_Y, Pixel color, float alpha,
								int shadow_max_x = 0, int shadow_max_y = 0, int fadeLength = 0,
								Pixel shadow_c = 0, float shadow_alpha = 0.0f );
	// Added a function to draw a sprite with shadow (assuming light is at the top left)
	// Also has the option of "fading in" the shadow if fadeLength is set > 0
	void DrawWithShadow( Surface* a_Target, int a_X, int a_Y, int shadow_max_x, int shadow_max_y, int fadeLength, Pixel shadow_c, float alpha );
	void DrawScaled( int a_X, int a_Y, int a_Width, int a_Height, Surface* a_Target );
	void SetFlags( unsigned int a_Flags ) { m_Flags = a_Flags; }
	void SetFrame( unsigned int a_Index ) { m_CurrentFrame = a_Index; }
	unsigned int GetFlags() const { return m_Flags; }
	int GetWidth() { return m_Width; }
	int GetHeight() { return m_Height; }
	Pixel* GetBuffer() { return m_Surface->GetBuffer(); }	
	unsigned int Frames() { return m_NumFrames; }
	Surface* GetSurface() { return m_Surface; }
private:
	// Methods
	void InitializeStartData();
	// Attributes
	int m_Width, m_Height, m_Pitch;
	unsigned int m_NumFrames;          
	unsigned int m_CurrentFrame;       
	unsigned int m_Flags;
	unsigned int** m_Start;
	Surface* m_Surface;
};

class Font
{
public:
	Font();
	Font( char* a_File, char* a_Chars );
	~Font();
	void Print( Surface* a_Target, char* a_Text, int a_X, int a_Y, bool clip = false );
	void Centre( Surface* a_Target, char* a_Text, int a_Y );
	int Width( char* a_Text );
	int Height() { return m_Surface->GetHeight(); }
	void YClip( int y1, int y2 ) { m_CY1 = y1; m_CY2 = y2; }
private:
	Surface* m_Surface;
	int* m_Offset, *m_Width, *m_Trans, m_Height, m_CY1, m_CY2;
};

}; // namespace Tmpl8