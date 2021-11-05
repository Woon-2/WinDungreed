#ifndef _scene
#define _scene

class scene
{
public:
	virtual void process_input() = 0;
	virtual void update() = 0;
	virtual void render() = 0;

private:
};

#endif