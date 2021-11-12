#ifndef _game
#define _game

#include <windows.h>
#include "timer.h"
#include "logo_scene.h"
#include "title_scene.h"
#include "sound.h"
#include "event_table.h"
#include "xyutility.h"

struct input_state
{
	bool is_lbutton_down;
	bool is_rbutton_down;
	float x;
	float y;
};

class game
{
public:
	void process_input( UINT msg, WPARAM w_param, LPARAM l_param )
	{
		switch ( msg )
		{
		default:
			break;

		case WM_KEYDOWN:
			keyboard( w_param );
			break;

		case WM_KEYUP:
			upkeyboard( w_param );
			break;

		case WM_LBUTTONDOWN: case WM_LBUTTONUP: case WM_RBUTTONDOWN: case WM_RBUTTONUP:
		{
			auto pt = convertpt( client, LOWORD( l_param ), HIWORD( l_param ) );
			mouse( msg, pt[ 0 ], pt[ 1 ] );
			break;
		}

		case WM_MOUSEMOVE:
		{
			auto pt = convertpt( client, LOWORD( l_param ), HIWORD( l_param ) );
			motion( msg, pt[ 0 ], pt[ 1 ] );
			break;
		}
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
		game_timer{ hWnd, timer_id, fps, clock },
		game_scene{ new logo_scene{ scene::SceneResources{ game_timer, event_queue, client, hWnd } } }
	{
		GetClientRect( hWnd, &client );
	}

	game( const game& ) = default;
	game& operator=( const game& ) = default;

private:
	HWND hWnd;
	HDC dc;
	RECT client;
	timer game_timer;
	std::unique_ptr< scene > game_scene;
	std::queue< Event > event_queue;

	void update()
	{
		game_timer.update();
		handle_scene_change();
		sound::update();
		game_scene->update();
	}

	void render()
	{
		game_scene->render();
	}

	void handle_scene_change()
	{
		if ( event_queue.size() )
		{
			switch ( event_queue.front() )
			{
			default:
				break;

			case Event::LOGO_END:
				game_scene.reset( new title_scene{ scene::SceneResources{ game_timer, event_queue, client, hWnd } } );
				event_queue.pop();
				break;
			}
		}
	}

	void keyboard( WPARAM key )
	{
		switch ( key )
		{
		default:
			break;

		case 'W':
			event_queue.push( Event::KEYDOWN_W );
			break;

		case 'A':
			event_queue.push( Event::KEYDOWN_A );
			break;

		case 'S':
			event_queue.push( Event::KEYDOWN_S );
			break;

		case 'D':
			event_queue.push( Event::KEYDOWN_D );
			break;

		case VK_LEFT:
			event_queue.push( Event::KEYDOWN_LEFT_ARROW );
			break;

		case VK_RIGHT:
			event_queue.push( Event::KEYDOWN_RIGHT_ARROW );
			break;

		case VK_UP:
			event_queue.push( Event::KEYDOWN_UP_ARROW );
			break;

		case VK_DOWN:
			event_queue.push( Event::KEYDOWN_DOWN_ARROW );
			break;
		}
	}

	void upkeyboard( WPARAM key )
	{
		switch ( key )
		{
		default:
			break;

		case 'W':
			event_queue.push( Event::KEYUP_W );
			break;

		case 'A':
			event_queue.push( Event::KEYUP_A );
			break;

		case 'S':
			event_queue.push( Event::KEYUP_S );
			break;

		case 'D':
			event_queue.push( Event::KEYUP_D );
			break;

		case VK_LEFT:
			event_queue.push( Event::KEYUP_LEFT_ARROW );
			break;

		case VK_RIGHT:
			event_queue.push( Event::KEYUP_RIGHT_ARROW );
			break;

		case VK_UP:
			event_queue.push( Event::KEYUP_UP_ARROW );
			break;

		case VK_DOWN:
			event_queue.push( Event::KEYUP_DOWN_ARROW );
			break;
		}
	}

	void mouse( UINT msg, float x, float y )
	{
		std::cout << x << ", " << y << " clicked\n";
	}

	void motion( UINT msg, float x, float y )
	{

	}
};
#endif

