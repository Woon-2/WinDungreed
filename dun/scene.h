#ifndef _scene
#define _scene

#include "game.h"
#include "event_table.h"
#include "TMP.h"
#include "image.h"
#include "videocomponent.h"

class scene
{
public:
	struct SceneResources
	{
		timer& game_timer;
		std::queue< Event >& event_queue;
		RECT& client;
		HWND hWnd;
	};

public:
	virtual void update() = 0;
	virtual void render() = 0;

	scene( const SceneResources& scene_resources ) : game_timer{ scene_resources.game_timer }, event_queue{ scene_resources.event_queue },
		client{ scene_resources.client }, hWnd{ scene_resources.hWnd } {}
	
	virtual ~scene() {}

protected:
	timer& game_timer;
	std::queue< Event >& event_queue;
	RECT& client;
	HWND hWnd;
	VideoComponent video_component;

	void add_event( Event e )
	{
		event_queue.push( e );
	}

	void fadein( const float ms_time, int fade_frame = 50 )
	{
		for ( int i = 0; i < fade_frame; ++i )
		{
			game_timer.alarm( ms_time / fade_frame * i, [this, ms_time, fade_frame]()
				{
					video_component.add_alpha( 255.f / fade_frame );
					if ( video_component.get_alpha() > 255.f )
					{
						video_component.set_alpha( 255.f );
					}
				} );
		}
	}

	void fadeout( const float ms_time, int fade_frame = 50 )
	{
		for ( int i = 0; i < fade_frame; ++i )
		{
			game_timer.alarm( ms_time / fade_frame * i, [this, ms_time, fade_frame]()
				{
					video_component.add_alpha( -255.f / fade_frame );
					if ( video_component.get_alpha() < 0.f )
					{
						video_component.set_alpha( 0.f );
					}
				} );
		}
	}
};

#endif