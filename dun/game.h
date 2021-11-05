#ifndef _game
#define _game

#include <windows.h>
#include "timer.h"
#include "scene.h"

class game
{
public:
	void on_wtimer( UINT id )
	{
		// if ( game_timer.timer_id == id )
		// {
		auto lag = game_timer.getlag();
		auto ms_per_frame = game_timer.get_ms_per_frame();

		process_input();

		if ( lag < ms_per_frame * 2 )
		{
			update();

			if ( lag < ms_per_frame )
			{
				render();
			}
		}
		// }
	}

	game( HWND hWnd, const UINT timer_id, const float fps, const float clock = 10.f ) : hWnd{ hWnd },
		game_timer{ hWnd, timer_id, fps, clock } {}
	game( const game& ) = default;
	game& operator=( const game& ) = default;

private:
	HWND hWnd;
	timer game_timer;
	std::unique_ptr< scene > game_scene;

	void process_input()
	{

	}

	void update()
	{
		game_timer.update();
	}

	void render()
	{

	}
};
#endif

