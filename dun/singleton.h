#pragma once
#ifndef _singleton
#define _singleton

template < typename T >
class Singleton
{
public:
	static auto& getinst()
	{
		static T inst = no_constructor_call{};
		return inst;
	}
	Singleton(const Singleton&) = delete;
	void operator=(const Singleton&) = delete;

protected:
	struct no_constructor_call {};
	Singleton() {}
};
#endif