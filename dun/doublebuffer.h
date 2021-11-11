#ifndef _doublebuffer
#define _doublebuffer

#include <windows.h>

class doublebuffer
{
public:
	doublebuffer( HDC dc, int swidth, int sheight ) : swidth{ swidth }, sheight{ sheight }, h_dc{ dc }, buf_dc{ CreateCompatibleDC( dc ) }
	{
		buf_bit = CreateCompatibleBitmap( h_dc, swidth, sheight );
		old_bit = static_cast< HBITMAP >( SelectObject( buf_dc, buf_bit ) );
	}

	~doublebuffer()
	{
		SelectObject( buf_dc, old_bit );
		DeleteObject( buf_bit );
		DeleteDC( buf_dc );
	}

	HDC h_dc;
	HDC buf_dc;
	HBITMAP buf_bit;
	HBITMAP old_bit;
	int swidth;
	int sheight;
};

#define BackBuffer _BackBuffer::getinst()
class _BackBuffer : public Singleton< _BackBuffer >
{
public:
	void Draw( HWND hWnd, HDC h_dc )
	{
		RECT client;
		GetClientRect( hWnd, &client );

		buf_dc = CreateCompatibleDC( h_dc );
		HBITMAP bit_buf{ CreateCompatibleBitmap( h_dc, client.right, client.bottom ) };
		SelectObject( buf_dc, bit_buf );
		BitBlt( h_dc, 0, 0, client.right, client.bottom, buf_dc, 0, 0, SRCCOPY );
		DeleteObject( bit_buf );
		DeleteDC( buf_dc );
	}
	operator HDC() { return buf_dc; }

	_BackBuffer( Singleton::no_constructor_call ) : buf_dc{ nullptr } {}

private:
	HDC buf_dc;
};

#endif