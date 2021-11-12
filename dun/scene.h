#ifndef _scene
#define _scene

#include "game.h"
#include "event_table.h"
#include "TMP.h"
#include "image.h"

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

	void add_event( Event e )
	{
		event_queue.push( e );
	}
};

#endif