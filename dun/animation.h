#ifndef _animation
#define _animation

#include <vector>
#include <iostream>

class Animation
{
public:
	void push( const ClipArea& clip_area )
	{
		clip_info.push_back( clip_area );
	}

	const ClipArea& get_clip_area() const
	{
		return clip_info[ cur_idx ];
	}

	const bool get_is_finished() const
	{
		return is_finished;
	}

	Animation& operator++()
	{
		if ( !is_finished )
		{
			++cur_idx;

			if ( cur_idx == clip_info.size() )
			{
				if ( !will_loop )
				{
					--cur_idx;
					is_finished = true;
				}
				else
				{
					cur_idx = 0;
				}
			}
		}

		return *this;
	}

	Animation& operator--()
	{
		if ( cur_idx == 0 )
		{
			std::cerr << "Animation operator-- called when idx was 0.\n";
		}
		else
		{
			--cur_idx;
		}
		return *this;
	}

	static auto make( const bool will_loop = false, const size_t start_frame = 0 )
	{
		return std::shared_ptr< Animation >{ new Animation{ will_loop, start_frame } };
	}

private:
	Animation( const bool will_loop, const size_t start_frame ) : will_loop{ will_loop }, cur_idx{ start_frame },
		is_finished{ false } {}

	std::vector< ClipArea > clip_info;
	size_t cur_idx;
	bool will_loop;
	bool is_finished;
};

using animation_ptr = std::shared_ptr< Animation >;

#endif