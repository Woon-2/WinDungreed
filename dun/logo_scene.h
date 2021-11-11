#ifndef _logoscene
#define _logoscene

#include "scene.h"

class logo_scene : public scene
{
public:
	logo_scene( timer& t, std::queue< Event >& eq ) : scene( t, eq )
	{

	}

	~logo_scene() override
	{

	}

	void update() override
	{
		scene::update();
	}

	void render() override
	{

	}

private:
};

#endif