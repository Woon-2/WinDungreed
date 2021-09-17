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

	_BackBuffer(Singleton::no_constructor_call) {}

private:
	HDC buf_dc;
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;
	g_hInst = hInstance;

	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszWindowName, WS_SYSMENU, 0, 0, 1280, 800, NULL, (HMENU)NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hDC;
	PAINTSTRUCT ps;

	switch (uMsg)
	{
	case WM_CREATE:
		GetClientRect(hWnd, &client);
		break;
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		BackBuffer.Draw(hDC);
		EndPaint(hWnd, &ps);
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'T':
		{
			pool<test_pool> pl1{ 20 };
			auto pl = std::move(pl1);

			for ( int i = 0; i < 5; ++i )
			{
				std::cout << "\n-----------------------------------------\n";

				for ( int j = 0; j < 5; ++j )
				{
					auto uptr = pl.salloc( j + 1, j + 2, j + 3 );
					std::cout << uptr->x << ", " << uptr->y << ", " << uptr->z << std::endl;
					//pl.dealloc( uptr );
				}
			}

			for ( int i = 0; i < 5; ++i )
			{
				std::cout << "\n-----------------------------------------\n";

				auto arr = pl.salloc< 5 >( i + 1, i + 2, i + 3 );
				for ( auto& iter : arr )
				{
					std::cout << iter->x << ", " << iter->y << ", " << iter->z << std::endl;
					//pl.dealloc( iter );
				}

				std::cout << pl.available_cnt() << std::endl;
			}

			SetTimer( hWnd, 1, timer::ms_per_frame(), NULL );
			break;
		}

		case 'M':
		{
			pool<test_pool> pl{ 10'000'000 };

			std::cout << "원시 동적 할당 : " << timefunc([]()
				{
					for ( int i = 0; i < 10'000'000; ++i )
					{
						auto ptr = new test_pool{ 1, 2, 3 };
						delete ptr;
					}
				} ) << " ms 소요\n";

			std::cout << "풀 동적 할당 : " << timefunc( [&pl]()
				{
					for ( int i = 0; i < 10'000'000; ++i )
					{
						auto ptr = pl.alloc( 1, 2, 3 );
					}
				} ) << " ms 소요\n";

			break;
		}

		case 'Q':
			KillTimer(hWnd, 1);
			SendMessage(hWnd, WM_DESTROY, 0, 0);
			break;
		}
		break;

	case WM_TIMER:
		timer::on_timer();
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}