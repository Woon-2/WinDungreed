#pragma once
#ifndef _singleton
#define _singleton
#include "keyword.h"
template <typename T>
class Singleton NONCOPYABLE INTERFACE
{
public:
	static auto& getinst() NOASSIGNMENT
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