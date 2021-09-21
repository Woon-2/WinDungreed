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
	// after ms_time, func will be executed.
	// func sholde be compatible with std::function< void() >
	template < typename Func >
	static void add_request( const double ms_time, Func func ) noexcept
	{
		worldtime_timer::add_request( worldtime_timer::request{ ms_time, func } );
	}

	// every ms_time, func will be executed.
	// func sholde be compatible with std::function< void() >
	// Condition should be compatible with std::function< bool() >
	template < typename Func, typename Condition >
	static void add_loop_request( const double ms_time, Func func, Condition condition ) noexcept
	{
		add_request( ms_time, [ func, condition ]() {
			if ( condition() )
			{
				func();
				add_loop_request( ms_time, func, condition );
			} } );
	}

	static constexpr void setFPS( const int fps ) noexcept { frame_timer::setFPS( fps ); }
	static constexpr const float ms_per_frame() noexcept { return frame_timer::ms_per_frame(); }
	static constexpr const int curFPS() noexcept { return static_cast< int >( 1000.0f / ms_per_frame() ); }

	static void on_timer()
	{
		std::cout << "\n현재 FPS : " << curFPS() << std::endl;

		static float lag = 0;
		std::cout << "루틴 시작 당시 lag : " << lag << std::endl;
		auto elapsed = static_cast< float >( timefunc( go_routines, lag ) );
		lag = std::max( lag + elapsed - ms_per_frame(), 0.0f );
	}

	timer() = delete;

private:
	static void go_routines( const float current_lag )
	{
		if ( current_lag < ms_per_frame() * 2 )
		{
			worldtime_timer::on_timer();
			frame_timer::on_timer( current_lag );
		}
	}

	class frame_timer
	{
	public:
		static constexpr void setFPS( const int fps ) noexcept
		{
			_ms_per_frame = 1000.0f / fps;
		}

		static constexpr const float ms_per_frame() noexcept
		{
			return _ms_per_frame;
		}

		static void on_timer( const float current_lag )
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

		static float _ms_per_frame;
	};

	class worldtime_timer
	{
	public:
		class request
		{
		public:
			template < typename Func >
			explicit request( const double ms_time, Func func ) :
				_world_time_expected{ ms_time + world_time }, func{ func } {}

			request() = delete;

			request( const request& other ) : _world_time_expected{ other._world_time_expected },
				func{ other.func } {}

			request& operator=( const request& other )
			{
				if ( this != &other )
				{
					_world_time_expected = other._world_time_expected;
					func = other.func;
				}

				return *this;
			}

			request( request&& other ) noexcept : _world_time_expected{ other._world_time_expected },
				func{ std::move( other.func ) } {}

			request& operator=( request&& other ) noexcept
			{
				if ( this != &other )
				{
					_world_time_expected = other._world_time_expected;
					func = std::move( other.func );
				}

				return *this;
			}

			const bool operator>( const request& other ) const noexcept
			{
				return _world_time_expected > other._world_time_expected;
			}

			const double world_time_expected() const noexcept
			{
				return _world_time_expected;
			}

			void sub( const double value ) noexcept
			{
				_world_time_expected -= value;
			}

			void handle() const
			{
				func();
			}

		private:
			double _world_time_expected;
			std::function< void() > func;
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

			world_time += duration_cast< nanoseconds >( cur_time - last_time ).count()
				/ static_cast< double >( nanoseconds::period::den / milliseconds::period::den );
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

		static void rearrange_requests( const double sub_val )
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

		static constexpr const double world_time_limit()
		{
			constexpr const double prevent_time = 100'000;			// 100s
			return std::numeric_limits< double >::max() - prevent_time;
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
		static double world_time;
		static std::priority_queue< request, std::deque< request >, std::greater< request > > requests;
	};
};

float timer::frame_timer::_ms_per_frame = 1000.0f / 30;
double timer::worldtime_timer::world_time = 0.0;
std::priority_queue< timer::worldtime_timer::request,
	std::deque< timer::worldtime_timer::request >,
	std::greater< timer::worldtime_timer::request > > timer::worldtime_timer::requests;

#endif