#ifndef _myvector
#define _myvector

#include <array>

template < typename Ty, size_t N >
class vec
{
public:
	vec() = default;
	vec( const std::array< Ty, N >& c ) : c{ c } {}
	vec( std::array< Ty, N >&& c ) noexcept : c{ std::move( c ) } {}

	Ty& operator[]( size_t idx )
	{
		return c[ idx ];
	}

	const Ty& operator[]( size_t idx ) const
	{
		return c[ idx ];
	}

	Ty dot( const vec& other ) const
	{
		return dot_impl( other, std::make_index_sequence< N >{} );
	}

	vec operator+( const vec& rhs ) const
	{
		return add_impl( rhs, std::make_index_sequence< N >{} );
	}

	vec operator-() const
	{
		return oppose_impl( std::make_index_sequence< N >{} );
	}

protected:
	std::array< Ty, N > c;

private:
	template < size_t ... Idx >
	Ty dot_impl( const vec& other, std::index_sequence< Idx... > )
	{
		return ((c[ Idx ] * other.c[ Idx ]) + ...);
	}

	template < size_t ... Idx >
	vec add_impl( const vec& rhs, std::index_sequence< Idx... > )
	{
		return vec{ std::array< Ty, N >{ ( c[ Idx ] + rhs.c[ Idx ] )...} };
	}

	template < size_t ... Idx >
	vec oppose_impl( std::index_sequence< Idx... > )
	{
		return vec{ std::array< Ty, N >{ -c[ Idx ]... } };
	}
};

template < typename Ty >
class vec3_t : public vec< Ty, 3 >
{
public:
	vec3_t() = default;
	vec3_t( float x, float y, float z ) : vec< Ty, 3 >{ std::array< Ty, 3 >{ x, y, z } } {}

private:
};

template < typename Ty >
class vec2_t : public vec< Ty, 2 >
{
public:
	vec2_t() = default;
	vec2_t( float x, float y ) : vec< Ty, 2 >{ std::array< Ty, 2 >{ x, y } } {}

private:
};

using vec3 = vec3_t< float >;
using vec2 = vec2_t< float >;
#endif