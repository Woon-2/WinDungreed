#ifndef _logoscene
#define _logoscene

#include "scene.h"
#include <iostream>
#include "videocomponent.h"

class logo_scene : public scene
{
public:
	logo_scene( const SceneResources& scene_resources ) : scene( scene_resources ), dir{ 1 }, revert{ false }
	{
		game_timer.alarm( 3000.f, [this]() { event_queue.push( Event::LOGO_END ); } );
		video_component.push( Image::make( L"images/kpulogo.PNG" ) );
		video_component.enable_alpha();
		video_component.set_alpha( 0x00 );
	}

	~logo_scene() override
	{

	}

	void update() override
	{
		video_component.add_alpha( game_timer.get_frame_time() / 2 * dir );

		if ( !revert )
		{
			if ( video_component.get_alpha() > 255.f )
			{
				dir = 0;
				video_component.set_alpha( 255.f );
				game_timer.alarm( 2000, [this]() { revert = true; dir = -1; } );
			}
		}
		else
		{
			if ( video_component.get_alpha() < 1.f )
			{
				video_component.set_alpha( 0.f );
				dir = 0;
			}
		}
	}

	void render() override
	{
		HDC dc = GetDC( hWnd );
		video_component.draw( dc, client.left, client.top, client.right - client.left, client.bottom - client.top );
		ReleaseDC( hWnd, dc );
		InvalidateRect( hWnd, nullptr, false );
	}

private:
	VideoComponent video_component;
	int dir;
	bool revert;
};

#endif