#ifndef _timer
#define _timer

#ifdef max
#undef max		// c 매크로 max를 제거하기 위함
#endif

#include <chrono>
#include <iostream>
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
	static constexpr void setFPS( const int fps ) noexcept { frame_timer::setFPS( fps ); }
	static constexpr const int ms_per_frame() noexcept { return frame_timer::ms_per_frame(); }

	// after ms_time, wrapped_func will be executed.
	// wrap your func in a lambda expression.
	// [ &args... ]() { your_static_func( args... ) }
	// [ this, &args... ]() { this-> your_class_func( args... ) }
	template < typename Func >
	static void add_request( const long long ms_time, Func wrapped_func ) noexcept
	{
		worldtime_timer::add_request( worldtime_timer::request{ ms_time, wrapped_func } );
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
		worldtime_timer::on_timer();
		frame_timer::on_timer( current_lag );
	}


	class frame_timer
	{
	public:
		static constexpr void setFPS( const int fps ) noexcept
		{
			_ms_per_frame = 1000 / fps;
		}

		static constexpr const int ms_per_frame() noexcept
		{
			return _ms_per_frame;
		}

		static void on_timer( const int current_lag )
		{
			update();
			if ( current_lag < ms_per_frame() )
			{
				render();
			}
		}

	private:

		static void update()
		{
			std::cout << "업데이트 루틴!\n";
			for ( int i = 0; i < 20000000; ++i )
			{
				i += 2;
			}
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
	};

	class worldtime_timer
	{
	public:
		class request
		{
		public:
			template < typename Func >
			explicit request( const long long ms_time, Func wrapped_func ) :
				_world_time_expected{ ms_time + world_time }, wrapped_func{ wrapped_func } {}

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

			void sub( const long long value ) noexcept
			{
				_world_time_expected -= value;
			}

			void handle() const
			{
				wrapped_func();
			}

		private:
			long long _world_time_expected;
			std::function< void() > wrapped_func;
		};

		static void on_timer()
		{
			update_world_time();
			prevent_world_time_overflow();

			while ( should_handle_a_request() )
			{
				handle_a_request();
			}

			std::cout << "worldtime : " << world_time << std::endl;
			std::cout << "남은 요청 수 : " << requests.size() << std::endl;
		}

		static void add_request( request&& req )
		{
			requests.push( req );
		}

	private:
		static void update_world_time()
		{
			using namespace std::chrono;
			static auto last_time = system_clock::now();
			auto cur_time = system_clock::now();

			world_time += duration_cast< milliseconds >( cur_time - last_time ).count();
			last_time = cur_time;
		}

		static void prevent_world_time_overflow()
		{
			if ( world_time > world_time_limit() )
			{
				world_time -= world_time_limit();
				rearrange_requests( world_time_limit() );
			}
		}

		static void rearrange_requests( const long long sub_val )
		{
			decltype( requests ) temp;

			while ( !requests.empty() )
			{
				auto req = requests.top();
				req.sub( sub_val );

				temp.push( std::move( req ) );
				requests.pop();
			}

			requests = std::move( temp );
		}

		static constexpr const long long world_time_limit()
		{
			constexpr const long long prevent_time = 100'000;			// 100s
			return std::numeric_limits< long long >::max() - prevent_time;
		}

		static const bool should_handle_a_request()
		{
			if ( requests.empty() )
			{
				return false;
			}
			return requests.top().world_time_expected() < world_time;
		}

		static void handle_a_request()
		{
			requests.top().handle();
			requests.pop();
		}

	private:
		static long long world_time;
		static std::priority_queue< request, std::deque< request >, std::greater< request > > requests;
	};
};

int timer::frame_timer::_ms_per_frame = 1000 / 30;
long long timer::worldtime_timer::world_time = 0;
std::priority_queue< timer::worldtime_timer::request,
	std::deque< timer::worldtime_timer::request >,
	std::greater< timer::worldtime_timer::request > > timer::worldtime_timer::requests;

#endif