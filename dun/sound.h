#ifndef _sound
#define _sound

#include "fmod.hpp"
#include "pool.h"

// 일반 사운드 재생
// 태그를 지정, 사운드 재생
// 보유자를 지정, 사운드 재생
// 태그와 보유자를 지정, 사운드 재생
// 특정 태그의 사운드 멈춤
// 특정 보유자의 사운드 멈춤
// 특정 태그의 사운드 재개
// 특정 보유자의 사운드 재개
// 특정 태그의 사운드 볼륨 조정
// 특정 보유자의 사운드 볼륨 조정
// 일정 게임월드시간(ms) 동안의 경우에 대해 전부 오버로딩

class sound
{
public:
	// volume : [0.0f, 1.0f]
	static void set_default_volume( const float vol )
	{
		fmods.default_volume = vol;
	}


private:
	struct fmod_service
	{
		fmod_service() : default_volume{ 1.0f }, sounds{ fmod_num_sounds }, channels{ fmod_num_channels }
		{
			FMOD::System_Create( &system );
			system->init( fmod_num_channels, FMOD_INIT_NORMAL, nullptr );
		}

		~fmod_service()
		{
			system->release();
		}
		
		fmod_service( const fmod_service& other ) = delete;
		fmod_service& operator=( const fmod_service& other ) = delete;

		// -----------------------------------------------------------
		// fmod datas

		static constexpr size_t fmod_num_sounds = 320;
		static constexpr size_t fmod_num_channels = 32;

		float default_volume;
		FMOD::System* system;
		pool< FMOD::Sound > sounds;
		pool< FMOD::Channel > channels;
	};

	static fmod_service fmods;

	float volume;
	int sound_id;
	bool will_roop;
};

#endif