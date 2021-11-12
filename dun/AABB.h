#ifndef _AABB
#define _AABB

#include <windows.h>
#include <functional>
#include "myvector.h""

struct AABB
{
	float min_x;
	float min_y;
	float max_x;
	float max_y;

	const bool collide( const AABB& rhs ) const
	{
		if ( max_x < rhs.min_x || min_x > rhs.max_x ) return false;
		if ( max_y < rhs.min_y || min_y > rhs.max_y ) return false;
		return true;
	}

	template < typename Func >
	void draw( HDC dc, Func&& world_to_screen, COLORREF color = RGB(255, 255, 255) ) const
	{
		POINT screen_lt = world_to_screen( min_x, max_y );		// 월드와 스크린은 y축 방향이 반대
		POINT screen_rb = world_to_screen( max_x, min_y );

		RECT screen_rect{ screen_lt.x, screen_lt.y, screen_rb.x, screen_rb.y };

		HBRUSH brush = CreateSolidBrush( color );
		HBRUSH old_brush = static_cast< HBRUSH >( SelectObject( dc, brush ) );

		FrameRect( dc, &screen_rect, brush );

		SelectObject( dc, old_brush );
		DeleteObject( brush );
	}
};

#endif