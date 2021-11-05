#pragma once
#ifndef _saveload
#define _saveload

#include <fstream>
#include <string>
#include "database.h"

class saveLoad
{
public:

#pragma pack(push, 1)
	// 저장할 데이터 구조체
	// SaveData 객체를 파일에 저장하고, (save_bin)
	// 파일을 SaveData 객체에 불러온다. (load_bin)
	// SaveData 객체를 프록시로 코드 내의 변수들과 상호작용하면 된다.
	struct saveData
	{
		// 저장, 불러오기에 쓰일 변수들은 여기에서 추가하거나 제거한다.
	};
#pragma pack(pop)

	static void save_bin( const char* file_path, const saveData& data )
	{
		save_bin_impl( file_path, data );
	}

	static void save_bin( const char* file_path, saveData&& data )
	{
		save_bin_impl( file_path, std::move( data ) );
	}

	static void save_bin( const wchar_t* file_path, const saveData& data )
	{
		save_bin_impl( file_path, data );
	}

	static void save_bin( const wchar_t* file_path, saveData&& data )
	{
		save_bin_impl( file_path, std::move( data ) );
	}

	static void save_bin( const std::string& file_path, const saveData& data )
	{
		save_bin_impl( file_path, data );
	}

	static void save_bin( const std::string& file_path, saveData&& data )
	{
		save_bin_impl( file_path, std::move( data ) );
	}

	static void save_bin( const std::wstring& file_path, const saveData& data )
	{
		save_bin_impl( file_path, data );
	}

	static void save_bin( const std::wstring& file_path, saveData&& data )
	{
		save_bin_impl( file_path, std::move( data ) );
	}

	static void load_bin( const char* file_path, saveData& data )
	{
		load_bin_impl( file_path, data );
	}

	static void load_bin( const wchar_t* file_path, saveData& data )
	{
		load_bin_impl( file_path, data );
	}

	static void load_bin( const std::string& file_path, saveData& data )
	{
		load_bin_impl( file_path, data );
	}

	static void load_bin( const std::wstring& file_path, saveData& data )
	{
		load_bin_impl( file_path, data );
	}

	saveLoad() = delete;

private:
	template < typename Str_t, typename SV >
	static void save_bin_impl( Str_t&& file_path, SV&& data )
	{
		std::ofstream out{ file_path, std::ios_base::binary | std::ios_base::out };

		if ( out.fail() )
			throw std::ios_base::failure{ "SaveLoad::save_bin_impl : cannot open file" };

		out.write( reinterpret_cast< const char* >( &data ), sizeof( decltype( data ) ) );
	}

	template < typename Str_t, typename SV >
	static void load_bin_impl( Str_t&& file_path, SV&& data )
	{
		std::ifstream in{ file_path, std::ios_base::binary | std::ios_base::in };

		if ( in.fail() )
			throw std::ios_base::failure{ "SaveLoad::load_bin_impl : cannot open file" };

		in.read( reinterpret_cast< char* >( &data ), sizeof( decltype( data ) ) );
	}
};

#endif