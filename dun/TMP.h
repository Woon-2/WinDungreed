#ifndef _TMP
#define _TMP

#include <type_traits>
#include <chrono>
#include <tuple>
#include <iostream>

// identify if type is in a parameter pack or not
template < typename Tp, typename... List >
struct contains : std::false_type {};

template < typename Tp, typename Head, typename... Rest >
struct contains< Tp, Head, Rest... > :
    std::conditional< std::is_same< Tp, Head >::value,
    std::true_type,
    contains< Tp, Rest... >
    >::type {};

template < typename Tp >
struct contains< Tp > : std::false_type {};


// everything is private!
template < typename T >
class passkey
{
    friend T;
    passkey() {}

    // noncopyable
    passkey( const passkey& ) = delete;
    passkey& operator=( const passkey& ) = delete;
};


// what keys are allowed
template < typename... Keys >
class allow
{
public:
    template < typename Key >
    allow( const passkey< Key >& ) {
        static_assert( contains< Key, Keys... >::value, "Pass key is not allowed" );
    }

    // noncopyable
    allow( const allow& ) = delete;
    allow& operator=( const allow& ) = delete;
};

template < typename ... Elems, size_t ... Idx >
std::istream& istreamget_impl( std::istream& is, std::tuple< Elems... >& tup, const std::index_sequence< Idx... > )
{
    int dummy[ sizeof...( Elems ) ] = { ( is >> std::get< Idx >( tup ), 0 )... };
    return is;
}

template < typename ... Elems >
std::istream& operator>>( std::istream& is, std::tuple< Elems... >& tup )
{
    istreamget_impl( is, tup, std::make_index_sequence< sizeof...( Elems ) >() );
    return is;
}

template < typename ... Elems, size_t ... Idx >
std::ostream& ostreamput_impl( std::ostream& os, std::tuple< Elems... >& tup, const std::index_sequence< Idx... > )
{
    int dummy[ sizeof...( Elems ) ] = { ( os << std::get< Idx >( tup ) << ' ', 0 )... };
    return os;
}

template < typename ... Elems >
std::ostream& operator<<( std::ostream& os, std::tuple< Elems... >& tup )
{
    ostreamput_impl( os, tup, std::make_index_sequence< sizeof...( Elems ) >() );
    return os;
}

template < typename _Timet = std::chrono::milliseconds, typename Func, typename ... Args >
auto timefunc( Func func, Args... args )
{
    using namespace std::chrono;
    using _Countt = std::chrono::nanoseconds;

    auto tp = system_clock::now();
    func( args... );

    return duration_cast< _Countt >( system_clock::now() - tp ).count()
        / static_cast< long double >( _Countt::period::den / _Timet::period::den );
}

template < typename Enum_t >
constexpr const auto etoi( Enum_t e )
{
    return static_cast< int >( e );
}


#endif