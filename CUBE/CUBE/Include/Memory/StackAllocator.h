#pragma once

#include <cstdint>

/**
* A fixed-size stack based allocator. Arbitrary length
* bytes can be extracted from this allocator. Data can be
* free back into this allocator by clearing all data or 
* rolling back to a previous location given by GetMarker.
*/
class FStackAllocator
{
public:
	// Represents a position into the data stack
	using UMarker = uint32_t;

	/**
	* Creates a fixed size stack allocator.
	*/
	FStackAllocator(const uint32_t SizeBytes);

	/**
	* Dtor
	*/
	~FStackAllocator();

	/**
	* Allocates a chunk of memory. If aligned memory is
	* required, use the overloaded version of this function
	* that takes an Alignment param.
	* @param Bytes requested
	*/
	void* Allocate(const uint32_t Bytes);

	/**
	* Allocates a chunk of aligned memory from the stack.
	* @param Bytes requested
	* @param Alignment of the requested bytes
	*/
	void* Allocate(const uint32_t Bytes, const uint32_t Alignment);

	template <typename Type>
	/**
	* Allocates memory for a specific data type. If no alignment for
	* the type is given, the type's default alignment is used.
	*/
	Type* Allocate(const uint32_t Alignment = __alignof(Type))
	{
		return reinterpret_cast<Type*>(Allocate(sizeof(Type), Alignment));
	}

	/**
	* Retrieves the current location marker of the stack.
	* This 
	*/
	UMarker GetMarker() const;

	/**
	* Clears all data from the stack.
	*/
	void Clear();

	/**
	* Clears all data up to a marker from the stack.
	*/
	void ClearToMarker(const UMarker Marker);

private:
	/**
	* Moves the current marker for the stack to a 
	* position that is a multiple of Alignment.
	*/
	void AlignData(const uint32_t Alignment);

private:
	uint8_t* mData;

	// The current location we are at in the memory stack
	UMarker mCurrentMarker;
	UMarker mCapacity;
};