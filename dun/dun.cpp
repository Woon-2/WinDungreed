#include <windows.h>
#include "singleton.h"
#include "xyutility.h"
#include <cstddef>
#include "randomvalue.h"
#include <iostream>
#include "timer.h"
#include "database.h"
#include "saveload.h"
#include <array>
#include "pool.h"
#include "sound.h"
#include "timer.h"
#include "game.h"

std::unique_ptr< game > dungreed;

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"windows program";
RECT client;

#define BackBuffer _BackBuffer::getinst()
class _BackBuffer : public Singleton< _BackBuffer >
{
public:
	void Draw(HDC h_dc)
	{
		buf_dc = CreateCompatibleDC(h_dc);
		HBITMAP bit_buf{ CreateCompatibleBitmap(h_dc, client.right, client.bottom) };
		SelectObject(buf_dc, bit_buf);
		BitBlt(h_dc, 0, 0, client.right, client.bottom, buf_dc, 0, 0, SRCCOPY);
		DeleteObject(bit_buf);
		DeleteDC(buf_dc);
	}
	operator HDC() { return buf_dc; }

	_BackBuffer( Singleton::no_constructor_call ) : buf_dc{ nullptr } {}

private:
	HDC buf_dc;
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow )
{
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;
	g_hInst = hInstance;

	WndClass.cbSize = sizeof( WndClass );
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = ( WNDPROC )WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon( NULL, IDI_APPLICATION );
	WndClass.hCursor = LoadCursor( NULL, IDC_ARROW );
	WndClass.hbrBackground = ( HBRUSH )GetStockObject( BLACK_BRUSH );
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon( NULL, IDI_APPLICATION );
	RegisterClassEx( &WndClass );

	RECT rt = { 0, 0, 1280, 800 };
	AdjustWindowRect( &rt, WS_OVERLAPPEDWINDOW, false );

	hWnd = CreateWindow( lpszClass, lpszWindowName, WS_SYSMENU, 0, 0, 1280, 800, NULL, (HMENU)NULL, hInstance, NULL );

	ShowWindow( hWnd, nCmdShow );
	UpdateWindow( hWnd );

	// ------------------------------------------------------------------------

	dungreed = std::make_unique< game >( hWnd, 0, 60 );

	bool run = true;
	while ( run )
	{
		while ( PeekMessage( &Message, hWnd, 0, 0, PM_REMOVE ) )
		{
			if ( Message.message == WM_QUIT )
			{
				run = false;
			}

			TranslateMessage( &Message );
			DispatchMessage( &Message );
		}
	}

	return Message.wParam;
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	HDC hDC;
	PAINTSTRUCT ps;

	switch ( uMsg )
	{
	case WM_CREATE:
		GetClientRect( hWnd, &client );
		break;

	case WM_PAINT:
		hDC = BeginPaint( hWnd, &ps );
		BackBuffer.Draw( hDC );
		EndPaint( hWnd, &ps );
		break;

	case WM_KEYDOWN:
		dungreed->process_input( uMsg, wParam, lParam );
		break;

	case WM_TIMER:
		dungreed->on_wtimer( wParam );
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}