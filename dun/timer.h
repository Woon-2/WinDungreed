#pragma once
#ifndef _timer
#define _timer

#ifdef max
#undef max		// c 매크로 max를 제거하기 위함
#endif

#include <chrono>
#include <iostream>
#include "keyword.h"
#include "TMP.h"
#include <algorithm>

class timer NONCREATABLE
{
public:
	static constexpr int ms_per_frame() PURE
	{
		return 30;
	}

	static void on_timer()
	{
		static int lag = 0;
		std::cout << "\n루틴 시작 당시 lag : " << lag << std::endl;
		auto elapsed = static_cast< int >( timefunc( go_routines, lag ) );
		lag = std::max( lag + elapsed - ms_per_frame(), 0 );
	}

	timer() = delete;

private:
	HELPER static inline void go_routines( const int current_lag )
	{
		update();
		if ( current_lag < ms_per_frame() )
			render();
	}

	HELPER static void update()
	{
		std::cout << "업데이트 루틴!\n";
		for ( int i = 0; i < 20000000; ++i )
			i += 2;
	}

	HELPER static void render()
	{
		std::cout << "렌더 루틴!\n";
		for ( int i = 0; i < 40000000; ++i )
			i += 2;
	}
};

#endif