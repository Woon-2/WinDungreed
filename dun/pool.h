#ifndef _pool
#define _pool

#include <memory>
#include <utility>
#include <array>
#include <type_traits>

#include <iostream>

template < typename Ty >
class pool
{
public:
	class dealloc_func
	{
	public:
		dealloc_func( pool& pl ) : pl{ pl } {}

		void operator()( Ty* exhausted ) noexcept
		{
			pl.deallocate( exhausted );
		}

	private:
		pool& pl;
	};

	using byte = char;
	using byte_ptr = char*;
	using byte_pptr = char**;
	using Uptr = std::unique_ptr< Ty, dealloc_func >;
	using Rptr = Ty*;
	using Sptr = std::shared_ptr< Ty >;

	// requires constructor args
	// returns an unique pointer
	// automatically deallocate
	template < typename ... Args >
	Uptr alloc( Args&& ... args )
	{
		if ( !avl_cnt )
		{
			throw std::bad_alloc{};
		}

		Ty* obj_ptr = new( getmem() ) Ty{ std::forward< Args >( args )... };	
		Uptr ret{ obj_ptr, dealloc_func{ *this } };

		--avl_cnt;

		return ret;
	}

	// requires constructor args
	// returns a shared pointer
	// automatically deallocate
	template < typename ... Args >
	Sptr salloc( Args&& ... args )
	{
		if ( !avl_cnt )
		{
			throw std::bad_alloc{};
		}

		Ty* obj_ptr = new( getmem() ) Ty{ std::forward< Args >( args )... };
		Sptr ret{ obj_ptr, dealloc_func{ *this } };

		--avl_cnt;

		return ret;
	}

	// requires constructor args
	// returns a raw pointer
	// automatically deallocate
	template < typename ... Args >
	Rptr ralloc( Args&& ... args )
	{
		if ( !avl_cnt )
		{
			throw std::bad_alloc{};
		}

		Ty* obj_ptr = new( getmem() ) Ty{ std::forward< Args >( args )... };

		--avl_cnt;

		return obj_ptr;
	}

	// requires array size template args, constructor args
	// returns array of unique pointer
	// automatically deallocate
	template < size_t arr_size, typename ... Args >
	std::array< Uptr, arr_size > alloc( Args&& ... args )
	{
		return alloc_array( std::make_index_sequence< arr_size >{}, std::forward< Args >( args )... );
	}

	// requires array size template args, constructor args
	// returns array of shared pointer
	// automatically deallocate
	template < size_t arr_size, typename ... Args >
	std::array< Sptr, arr_size > salloc( Args&& ... args )
	{
		return salloc_array( std::make_index_sequence< arr_size >{}, std::forward< Args >( args )... );
	}

	// requires array size template args, constructor args
	// returns array of raw pointer
	// automatically deallocate
	template < size_t arr_size, typename ... Args >
	std::array< Sptr, arr_size > ralloc( Args&& ... args )
	{
		return ralloc_array( std::make_index_sequence< arr_size >{}, std::forward< Args >( args )... );
	}

	const size_t available_cnt() const noexcept
	{
		return avl_cnt;
	}

	pool( const size_t capacity ) : mem{ new byte[ sizeof( Ty ) * capacity + sizeof( void* ) ] },
		free_ptr{ mem }, avl_cnt{ capacity }, ref_cnt{ new size_t{1} }
	{
		// write next memory's address in each memory
		byte_pptr cur = reinterpret_cast< byte_pptr >( mem );
		byte_ptr next = mem;

		for ( size_t i = 0; i < capacity; ++i )
		{
			next += sizeof( Ty );
			*cur = next;
			cur = reinterpret_cast< byte_pptr >( next );
		}

		*cur = nullptr;
	}

	pool( const pool& other ) noexcept : mem{ other.mem }, free_ptr{ other.free_ptr },
		avl_cnt{ other.avl_cnt }, ref_cnt{ other.ref_cnt }
	{
		++*ref_cnt;
	}

	pool& operator=( const pool& other ) noexcept
	{
		if ( this != &other )
		{
			mem = other.mem;			free_ptr = other.free_ptr;
			avl_cnt = other.avl_cnt;	ref_cnt = other.ref_cnt;

			++*ref_cnt;
		}

		return *this;
	}

	pool( pool&& other ) noexcept : mem{ other.mem }, free_ptr{ other.free_ptr },
		avl_cnt{ other.avl_cnt }, ref_cnt{ other.ref_cnt }
	{
		other.mem = nullptr;		other.free_ptr = nullptr;
		other.avl_cnt = 0;			other.ref_cnt = nullptr;
	}

	pool& operator=( pool&& other ) noexcept
	{
		if ( this != &other )
		{
			mem = other.mem;			free_ptr = other.free_ptr;
			avl_cnt = other.avl_cnt;	ref_cnt = other.ref_cnt;

			other.mem = nullptr;		other.free_ptr = nullptr;
			other.avl_cnt = 0;			other.ref_cnt = nullptr;
		}
		
		return *this;
	}

	~pool()
	{
		if ( mem )
		{
			if ( !--*ref_cnt )
			{
				delete[] mem;
			}
		}
	}

private:
	void deallocate( Ty* exhausted ) noexcept
	{
		putmem( reinterpret_cast< byte_ptr >( exhausted ) );

		++avl_cnt;
	}

	template < typename ... Args, size_t ... Idx >
	std::array< Uptr, sizeof...( Idx ) > alloc_array( std::index_sequence< Idx... >, Args&& ... args )
	{
		return std::array< Uptr, sizeof...( Idx ) >{ _dummy( alloc( std::forward< Args >( args )... ), Idx )... };
	}

	template < typename ... Args, size_t ... Idx >
	std::array< Sptr, sizeof...( Idx ) > salloc_array( std::index_sequence< Idx... >, Args&& ... args )
	{
		return std::array< Sptr, sizeof...( Idx ) >{ _dummy( salloc( std::forward< Args >( args )... ), Idx )... };
	}

	template < typename ... Args, size_t ... Idx >
	std::array< Rptr, sizeof...( Idx ) > ralloc_array( std::index_sequence< Idx... >, Args&& ... args )
	{
		return std::array< Rptr, sizeof...( Idx ) >{ _dummy( ralloc( std::forward< Args >( args )... ), Idx )... };
	}

	Uptr _dummy( Uptr elem, size_t ) const noexcept
	{
		return elem;
	}

	byte_ptr getmem() noexcept
	{
		byte_ptr ret = free_ptr;
		free_ptr = *reinterpret_cast< byte_pptr >( free_ptr );
		return ret;
	}

	void putmem( byte_ptr mem ) noexcept
	{
		*reinterpret_cast< byte_pptr >( mem ) = free_ptr;
		free_ptr = mem;
	}

	byte_ptr mem;
	byte_ptr free_ptr;
	size_t avl_cnt;
	size_t* ref_cnt;
};

struct test_pool
{
	int x, y, z;

	test_pool( int x, int y, int z ) : x{ x }, y{ y }, z{ z } { std::cout << "생성자 호출\n"; }
	~test_pool() { std::cout << "소멸자 호출\n"; }
};

#endif