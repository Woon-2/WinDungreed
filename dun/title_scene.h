#ifndef _title_scene
#define _title_scene

#include "scene.h"

class title_scene : public scene
{
public:
	title_scene( timer& t, std::queue< Event >& eq ) : scene( t, eq )
	{

	}

	~title_scene() override
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
