#ifndef _doublebuffer
#define _doublebuffer

#include <windows.h>

class GraphicBuffer
{
public:
	GraphicBuffer( HDC dc, int swidth, int sheight ) : swidth{ swidth }, sheight{ sheight }, h_dc{ dc }, buf_dc{ CreateCompatibleDC( dc ) }
	{
		buf_bit = CreateCompatibleBitmap( h_dc, swidth, sheight );
		old_bit = static_cast< HBITMAP >( SelectObject( buf_dc, buf_bit ) );
	}

	~GraphicBuffer()
	{
		SelectObject( buf_dc, old_bit );
		if ( old_bit != buf_bit )
		{
			DeleteObject( buf_bit );
		}
		DeleteDC( buf_dc );
	}

	HDC h_dc;
	HDC buf_dc;
	HBITMAP buf_bit;
	HBITMAP old_bit;
	int swidth;
	int sheight;
};

#endif