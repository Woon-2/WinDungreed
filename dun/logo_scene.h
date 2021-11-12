#ifndef _logoscene
#define _logoscene

#include "scene.h"
#include <iostream>

class logo_scene : public scene
{
public:
	logo_scene( const SceneResources& scene_resources ) : scene( scene_resources )
	{
		video_component.push( Image::make( L"images/kpulogo.PNG" ) );
		video_component.enable_alpha();
		video_component.set_alpha( 0x00 );

		game_timer.alarm( 3000.f, [this]() { event_queue.push( Event::LOGO_END ); } );
		game_timer.alarm( 2400.f, [this]() { fadeout( 400.f ); } );
		fadein( 400.f );
	}

	~logo_scene() override
	{

	}

	void update() override
	{

	}

	void render() override
	{
		HDC dc = GetDC( hWnd );
		video_component.draw( dc, client.left, client.top, client.right - client.left, client.bottom - client.top );
		ReleaseDC( hWnd, dc );
		InvalidateRect( hWnd, nullptr, false );
	}

private:
};

#endif