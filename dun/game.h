#ifndef _game
#define _game

#include <windows.h>
#include "timer.h"
#include "scene.h"
#include "sound.h"

class game
{
public:
	void process_input( UINT msg, WPARAM w_param, LPARAM l_param )
	{
		switch ( msg )
		{
		case WM_KEYDOWN:
			keyboard( w_param );
			break;
		}
	}

	void on_wtimer( UINT id )
	{
		// if ( game_timer.timer_id == id )
		// {
		auto lag = game_timer.getlag();
		auto ms_per_frame = game_timer.get_ms_per_frame();

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

	void update()
	{
		game_timer.update();
		sound::update();
	}

	void render()
	{

	}

	void keyboard( WPARAM key )
	{
		static std::vector< sound_ptr > test;

		std::cout << static_cast<char>(key) << " pushed\n";

		switch ( key )
		{
		case 'Z':
			test.push_back( sound::make( "sounds/BGM/0.Town.wav", sound::mode::loop ) );
			sound::tag_push( sound::sound_tag::BGM, test.back() );
			break;

		case 'x':
			break;

		case 'c':
			break;

		case 'q':
			break;

		case 'w':
			break;

		case 'e':
			break;

		case 'B':
			sound::tag_play( sound::sound_tag::BGM );
			break;

		case 'p':
			break;

		case 'l':
			break;
		}
	}

	void mouse( UINT msg, float x, float y )
	{

	}

	void motion( UINT msg, float x, float y )
	{

	}
};
#endif

