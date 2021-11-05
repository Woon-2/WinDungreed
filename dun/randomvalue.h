#pragma once
#ifndef _randomvalue
#define _randomvalue
#include <random>
#include <vector>
#include <array>

namespace _rv
{
	std::random_device rd;
	std::default_random_engine dre( rd() );
}

const auto random_value( const std::uniform_int_distribution<>& uid )
{
	return uid( _rv::dre );
}

const auto random_value( const int lower_bound, const int upper_bound )
{
	return random_value( std::uniform_int_distribution<>{lower_bound, upper_bound} );
}

auto random_value_vector( const std::uniform_int_distribution<>& uid, const size_t cnt )
{
	std::vector< int > v;
	v.reserve( cnt );
	for ( size_t i = 0; i < cnt; ++i )
	{
		v.push_back( random_value( uid ) );
	}
	return v;
}

auto random_value_vector( const int lower_bound, const int upper_bound, const size_t cnt )
{
	return random_value_vector( std::uniform_int_distribution<>{lower_bound, upper_bound}, cnt );
}

template <size_t N>
auto random_value_array( const std::uniform_int_distribution<>& uid )
{
	std::array< int, N > a;
	for ( auto& elem : a )
	{
		elem = random_value( uid );
	}
	return a;
}

template <size_t N>
auto random_value_array( const int lower_bound, const int upper_bound )
{
	return random_value_array< N >( std::uniform_int_distribution<>{lower_bound, upper_bound} );
}

const auto random_value( const std::uniform_real_distribution<>& urd )
{
	return urd( _rv::dre );
}

const auto random_value( const double lower_bound, const double upper_bound )
{
	return random_value( std::uniform_real_distribution<>{lower_bound, upper_bound} );
}

auto random_value_vector( const std::uniform_real_distribution<>& urd, const size_t cnt )
{
	std::vector< double > v;
	v.reserve( cnt );
	for ( size_t i = 0; i < cnt; ++i )
	{
		v.push_back( random_value( urd ) );
	}
	return v;
}

auto random_value_vector( const double lower_bound, const double upper_bound, const size_t cnt )
{
	return random_value_vector( std::uniform_real_distribution<>{lower_bound, upper_bound}, cnt );
}

template <size_t N>
auto random_value_array( const std::uniform_real_distribution<>& urd )
{
	std::array< double, N > a;
	for ( auto& elem : a )
		elem = random_value( urd );
	return a;
}

template <size_t N>
auto random_value_array( const double lower_bound, const double upper_bound )
{
	return random_value_array< N >( std::uniform_real_distribution<>{lower_bound, upper_bound} );
}

#include <iostream>
void test_random_value()
{
	for ( int i = 0; i < 10; ++i )
	{
		auto lower_bound{ random_value( 0.0, 10000.0 ) };
		auto upper_bound{ random_value( lower_bound, 10000.0 ) };

		auto n = random_value_array< 5 >( lower_bound, upper_bound );
		auto random_num = random_value( std::uniform_real_distribution<>{lower_bound, upper_bound} );
		std::uniform_real_distribution<> uid{ lower_bound, upper_bound };
		auto random_num2 = random_value( uid );

		std::cout << "lower_bound : " << lower_bound << " upper_bound : " << upper_bound << std::endl;
		std::cout << "{ " << n[ 0 ] << ", " << n[ 1 ] << ", " << n[ 2 ] << ", " << n[ 3 ] << ", " << n[ 4 ] << ", " << random_num << ", " << random_num2 << " }\n\n";
	}
}
#endif