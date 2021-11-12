#ifndef _state
#define _state

#include "event_table.h"

class State
{
public:
	virtual void enter( Event e ) = 0;
	virtual void quit( Event e ) = 0;
	virtual void update() = 0;
};

#endif