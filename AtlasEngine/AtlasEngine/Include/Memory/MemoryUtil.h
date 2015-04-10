#pragma once

#include <cstdint>

#define ALIGNED_ALLOC(Alignment) \
	void* operator new(size_t Size) { return FMemory::AllocateAligned(Size, (Alignment)); }  \
	void operator delete(void* Data) { FMemory::FreeAligned(Data); } \

/**
* Namespace for utility memory-based operations.
*/
namespace FMemory
{
	/**
	* Allocates an unaligned chunk of Bytes sized memory on
	* the heap. Memory allocated from this function must be freed
	* with FMemory::FreeUnaligned. For aligned dynamic allocations, use 
	* FMemory::AllocateAligned
	*/
	void* AllocateUnaligned(const uint32_t Bytes);

	/**
	* Allocates an aligned chunk of Bytes sized memory 
	* on the heap. Memory allocated from this function must be freed
	* with FMemory::FreeAligned.
	* @param Bytes Size of requested memory block.
	* @param Alignment Requested alignment of the memory block.
	*/
	void* AllocateAligned(const uint32_t Bytes, const uint32_t Alignment);

	/**
	* Frees a block of memory allocated by FMemory::AllocateUnaligned.
	* @param Data to be freed.
	*/
	void FreeUnaligned(void* Data);

	/**
	* Frees a block of memory allocated by FMemory::AllocateAligned.
	* @param Data to be freed.
	*/
	void FreeAligned(void* Data);
}