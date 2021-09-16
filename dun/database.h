#ifndef _database
#define _database

#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <tuple>
#include "TMP.h"
#include "keyword.h"

namespace _csv
{
    HELPER auto csv_read_row( std::istream& in, char delimiter )
    {
        std::stringstream ss;
        bool inquotes = false;
        bool packs = false;
        std::vector< std::string > row;

        while (in.good())
        {
            char c = in.get();

            switch (c)
            {
            case L'"':
                if ( !inquotes )      // begin quote char
                {
                    inquotes = true;
                }
                else
                {
                    if ( in.peek() == '"' )   // 2 consecutive quotes resolve to 1
                        ss << static_cast<char>( in.get() );
                    else                    // end quote char
                        inquotes = false;
                }
                break;

            case L'{':
                if ( !packs )
                {
                    packs = true;
                }
                else
                {
                    throw std::ios_base::failure{ "csv_read_row : '{' duplicated" };
                }
                break;

            case L'}':
                if ( !packs )
                {
                    throw std::ios_base::failure{ "csv_read_row : '{' must be followed by '}'" };
                }
                else
                {
                    packs = false;
                }
                break;

            case L'\r': case L'\n':
                if ( !inquotes && !packs )
                {
                    if ( in.peek() == '\n' )
                    {
                        in.get();
                    }

                    row.push_back( ss.str() );
                    row.shrink_to_fit();
                    return row;
                }
                break;

            default:
                if ( c == delimiter && !inquotes )
                {
                    row.push_back( ss.str() );
                    ss.str( "" );
                }
                else
                {
                    ss << c;
                }
                break;

            }
        }

        throw std::ios_base::failure{ "csv_read_row : '\\n', '\\r' not found" };
    }

    template < typename Str_t >
    auto csv_read( const Str_t file_path )
    {
        std::ifstream in{ file_path };
        std::vector< std::vector< std::string > > parsed;
        parsed.reserve( 0x20 );

        if ( in.fail() )
        {
            throw std::ios_base::failure{ "csv_read : cannot open file" };
        }

        while ( in.good() )
        {
            parsed.push_back( _csv::csv_read_row( in, ',' ) );
        }

        parsed.shrink_to_fit();
        return parsed;
    }

    void test_csv_read()
    {
        auto parsed = csv_read( "testcsv.csv" );
        for (const auto& row : parsed)
        {
            for (const auto& key : row)
            {
                std::cout << "[ " << key << " ] ";
            }
            std::cout << std::endl;
        }
    }
}

template < typename... TupleElems >
class database NONCOPYABLE
{
public:
    template < typename Str_t >
    database( const Str_t file_path )
    {
        auto parsed_csv{ _csv::csv_read( file_path ) };

        impl.reserve( 0x20 );
        
        for ( const auto& row : parsed_csv )
        {
            std::tuple< TupleElems... > tp;
            std::stringstream ss;
            for ( const auto& field : row )
            {
                ss << field << ' ';
            }
            ss >> tp;
            impl.push_back( std::move( tp ) );
        }

        impl.shrink_to_fit();
    }

    // row begins with 0
    decltype( auto ) get( const size_t row ) const NOASSIGNMENT
    {
        return impl.at( row );
    }

    // row begins with 0
    decltype( auto ) get( const size_t row ) NOASSIGNMENT
    {
        return impl.at( row );
    }

    // num of rows
    const size_t size() const { return impl.size(); }

    database() = delete;
    database( const database& other ) = delete;
    database& operator=( const database& other ) = delete;

private:
    std::vector< std::tuple< TupleElems... > > impl;
};

#include <iostream>
void test_database()
{
    database<int, int, int, int, std::string> db{ "testcsv.csv" };

    for ( size_t i = 0; i < db.size(); ++i )
    {
        std::cout << db.get( i ) << std::endl;
    }
}

#endif