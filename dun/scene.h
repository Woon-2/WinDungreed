#ifndef _scene
#define _scene

#include "game.h"
#include "event_table.h"
#include "TMP.h"

class scene
{
public:
	scene( timer& t, std::queue< Event >& eq ) : game_timer{ t }, event_queue{ eq } {}
	virtual ~scene() {}

	virtual void update() = 0
	{
		while ( event_queue.size() )
		{
			std::cout << etoi( event_queue.front() ) << " processed\n";
			event_queue.pop();
		}
	}

	virtual void render() = 0
	{

	}

protected:
	timer& game_timer;
	std::queue< Event >& event_queue;

	void add_event( Event e )
	{
		event_queue.push( e );
	}
};

#endif