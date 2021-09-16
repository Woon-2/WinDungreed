#ifndef _memorypool
#define _memorypool
// 다음 오브젝트 주소 : 프리포인터
// 블록 주소, 오브젝트 갯수
#include <unordered_set>
#include <list>
#include "TMP.h"
#include <iostream>
#include <algorithm>

#ifdef max		// disable c macro
#undef max
#endif

// 자료구조를 이용하는 부분을 메모리 쓰기로 치환
// 키워드 붙이기, noexcept const 살펴보기
// mem_size
// substract_no_overflow ㄱㄱ
// new[]
// delete[]
// 간격, 들여쓰기 조정

// 기본 할당 메모리
// 커스텀 메모리 전략

// Ty : object type, unit : number of objects to allocate at once
template < typename Ty, size_t unit >
class memory_pool INTERFACE
{
public:
	using byte = char;
	using byte_pointer = byte*;
	using byte_pointer_address = byte**;
	using object = Ty;

	static void* operator new( size_t alloc_memory_length )
	{	
		std::cout << "operator new!\n";
		create_block_if_thereis_no_available_block();
		return allocate_memory_in_available_block( alloc_memory_length );
	}

	static void operator delete( void* exhausted_memory )
	{
		std::cout << "operator delete!\n";
		for ( auto block : memory_blocks )
		{
			if ( block->is_in( exhausted_memory ) )
			{
				if ( block->full() )
				{
					available_blocks.push_back( block );
				}

				block->deallocate( exhausted_memory );

				if ( block->empty() )
				{
					delete block;
					memory_blocks.erase( block );
					available_blocks.erase(
						std::find( available_blocks.begin(), available_blocks.end(), block )
					);
					
				}

				return;
			}
		}
	}

private:
	class memory_block
	{
	public:
		void* allocate( size_t alloc_memory_length ) noexcept
		{
			std::cout << "memory_block::allocate!\n";
			// get free object and move free_pointer to next.
			byte_pointer available_memory = free_pointer;
			// reinterpret_cast for moving free_pointer to next
			// dereferencing gets exact size of the pointer, not size of a byte.
			free_pointer = *reinterpret_cast< byte_pointer_address >( free_pointer );

			++obj_cnt;

			return available_memory;
		}

		void deallocate( void* exhausted_memory ) noexcept
		{
			std::cout << "memory_block::deallocate!\n";
			// to-delete object becomes a free object,
			// so write current free object's address on to-delete object's head.
			*reinterpret_cast< byte_pointer_address >( exhausted_memory ) = free_pointer;
			free_pointer = static_cast< byte_pointer >( exhausted_memory );

			--obj_cnt;
		}

		memory_block() : mem_size{ sizeof( object ) * unit + sizeof( void* ) },
			free_pointer{ nullptr }, block_address{ nullptr }, obj_cnt{ 0 }
		{
			std::cout << "memory_block::memory_block!\n";
			allocate_block();
			write_empty_memory_sequence();
		}

		~memory_block()
		{
			std::cout << "memory_block::~memory_block!\n";
			delete[] block_address;
		}

		const bool full() const noexcept NOASSIGNMENT
		{
			std::cout << "memory_block::full!\n";
			// the last object points null.
			return nullptr == free_pointer;
		}

		const bool empty() const noexcept NOASSIGNMENT
		{
			std::cout << "memory_blockk::empty!\n";
			return 0 == obj_cnt;
		}

		const bool operator<( const memory_block& rhs ) const noexcept NOASSIGNMENT
		{
			return block_address < rhs.block_address;
		}

		const bool operator<( memory_block&& rhs ) const noexcept NOASSIGNMENT
		{
			return block_address < rhs.block_address;
		}

		const bool operator==( const memory_block& rhs ) const noexcept NOASSIGNMENT
		{
			return block_address == rhs.block_address;
		}

		HELPER const bool is_in( void* finding_memory ) const noexcept NOASSIGNMENT
		{
			std::cout << "memory_block::is_in!\n";
			return subtract_no_underflow( reinterpret_cast< unsigned long long >(finding_memory), mem_size )
				< reinterpret_cast< unsigned long long >(block_address)
				&& block_address <= finding_memory;
		}

	private:
		const unsigned long long subtract_no_underflow( const unsigned long long lhs, const unsigned long long rhs ) const noexcept
		{
			return std::max( lhs - rhs, 0ull );
		}

		HELPER void allocate_block()
		{
			std::cout << "memory_block::allocate_block!\n";
			// additionally allocate size of a pointer for safe object address writing.
			free_pointer = new byte[ mem_size ];
			block_address = free_pointer;
		}

		HELPER void write_empty_memory_sequence() noexcept
		{
			std::cout << "memory_block::write_empty_memory_sequence!\n";
			// reinterpret_cast for writing next object's address on empty objects' head
			// dereferencing gets exact size of the pointer, not size of a byte.
			byte_pointer_address cur = reinterpret_cast<byte_pointer_address>( free_pointer );
			byte_pointer next = free_pointer;
			// unit - 1 is the last index,
			// at unit - 2 next points the last.
			for ( int i = 0; unit - 1 > i; ++i )
			{
				next += sizeof( object );
				// write next object's address on empty current object's head.
				*cur = next;
				cur = reinterpret_cast<byte_pointer_address>( next );
			}
			// the last object points null.
			*cur = nullptr;
		}

		byte_pointer free_pointer;
		byte_pointer block_address;
		size_t mem_size;
		size_t obj_cnt;
	};

	HELPER static void create_block_if_thereis_no_available_block()
	{
		std::cout << "create_block_if_thereis_no_available_block!\n";
		if ( available_blocks.empty() )
		{
			auto block = new memory_block;
			memory_blocks.insert( block );
			available_blocks.push_back( block );
		}
	}

	HELPER static auto allocate_memory_in_available_block( size_t alloc_memory_length )
	{
		std::cout << "allocate_memory_in_available_block!\n";
		auto block{ available_blocks.front() };
		auto ret{ block->allocate( alloc_memory_length ) };

		if ( block->full() )
		{
			available_blocks.pop_front();
		}

		return ret;
	}

	static std::unordered_set< memory_block* > memory_blocks;
	static std::list< memory_block* > available_blocks;

protected:
	memory_pool() {}
};

template< typename Ty, size_t unit >
std::unordered_set<typename memory_pool<Ty, unit>::memory_block*> memory_pool<Ty, unit>::memory_blocks;
template< typename Ty, size_t unit >
std::list< typename memory_pool<Ty, unit>::memory_block* > memory_pool<Ty, unit>::available_blocks;

class TestPool : public memory_pool<TestPool, 4>
{
public:
	int a[ 100 ];
};
#endif