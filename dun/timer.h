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
#include <queue>
#include <numeric>
#include <deque>
#include <functional>
#include <utility>

class timer
{
public:
	// for not frame-based timer uses
	class request
	{
	public:
		// after ms_time, wrapped_func will be executed.
		// wrap your func in a lambda expression.
		// [ &args... ]() { your_static_func( args... ) }
		// [ this, &args... ]() { this-> your_class_func( args... ) }
		template < typename Func >
		explicit request( const long long ms_time, Func wrapped_func ) :
			_world_time_expected{ ms_time }, wrapped_func{ wrapped_func } {}

		request() = delete;

		request( const request& other ) : _world_time_expected{ other._world_time_expected },
			wrapped_func{ other.wrapped_func } {}

		request& operator=( const request& other )
		{
			if ( this != &other )
			{
				_world_time_expected = other._world_time_expected;
				wrapped_func = other.wrapped_func;
			}

			return *this;
		}

		request( request&& other ) noexcept : _world_time_expected{ other._world_time_expected },
			wrapped_func{ std::move( other.wrapped_func ) } {}

		request& operator=( request&& other ) noexcept
		{
			if ( this != &other )
			{
				_world_time_expected = other._world_time_expected;
				wrapped_func = std::move( other.wrapped_func );
			}

			return *this;
		}

		const bool operator>( const request& other ) const noexcept
		{
			return _world_time_expected > other._world_time_expected;
		}

		const long long world_time_expected() const noexcept
		{
			return _world_time_expected;
		}

		void handle() const
		{
			wrapped_func();
		}

	private:
		long long _world_time_expected;
		std::function< void() > wrapped_func;
	};

	static constexpr void setFPS( const int fps ) noexcept
	{
		_ms_per_frame = 1000 / fps;
	}

	static constexpr const int ms_per_frame() noexcept
	{
		return _ms_per_frame;
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
	static void go_routines( const int current_lag )
	{
		update();
		if ( current_lag < ms_per_frame() )
			render();
	}

	static void update()
	{
		world_time_based_update();
		frame_based_update();
	}

	static void frame_based_update()
	{
		std::cout << "업데이트 루틴!\n";
		for ( int i = 0; i < 20000000; ++i )
		{
			i += 2;
		}
	}

	static void world_time_based_update()
	{
		update_world_time();

		while ( should_handle_a_request() )
		{
			handle_a_request();
		}
	}

	static void update_world_time()
	{
		using namespace std::chrono;
		static auto last_time = system_clock::now();
		world_time += duration_cast< milliseconds >( last_time - system_clock::now() ).count();
	}

	static const bool should_handle_a_request()
	{
		if ( requests.empty() )
		{
			return false;
		}
		return requests.top().world_time_expected() > world_time;
	}

	static void handle_a_request()
	{
		requests.top().handle();
		requests.pop();
	}

	static void render()
	{
		std::cout << "렌더 루틴!\n";
		for ( int i = 0; i < 40000000; ++i )
		{
			i += 2;
		}
	}

	static int _ms_per_frame;
	static long long world_time;
	static std::priority_queue< request, std::deque< request >, std::greater< request > > requests;
};

int timer::_ms_per_frame = 1000 / 30;
long long timer::world_time = 0;
std::priority_queue< timer::request, std::deque< timer::request >, std::greater< timer::request > > timer::requests;

#endif