#pragma once

#include <cstdint>
#include "Misc/Assertions.h"
#include "MemoryUtil.h"

#include <algorithm>

template <uint32_t ElementSize, uint32_t BlockSize>
/**
* Fixed size pool allocator that uses a singly-linked free list
* to store allocated memory. Currently, the pool is of fixed size,
* so you will need to specify a BlockSize that will be sufficent for the max
* number of objects required.
* On destruction of this object, all allocations need to be freed back into
* the pool with FPoolAllocator::Free.
* \n
* @oaram ElementSize The size of each allocation object.
* @param BlockSize The max number of objects contained in the pool.
*/
class FPoolAllocator
{
public:
	/**
	* Ctor
	* Constructs a fixed size pool allocator with specified alignment.
	* @param Alignment for each allocation
	*/
	FPoolAllocator(uint32_t Alignment)
	{
		ASSERT(0 < BlockSize && "BlockSize must be larger that 0");
		ASSERT(ElementSize >= sizeof(PoolElement) && "ElementSize must at least the size of a standard pointer type.");

		// The byte gap between each allocation
		const uint32_t BlockGap = std::max(ElementSize, Alignment);

		// Obtain a block of memory
		uint8_t* RawMem = (uint8_t*)FMemory::AllocateAligned(BlockGap * BlockSize, Alignment);
		mNextFreeBlock = (PoolElement*)RawMem;

		// Link the blocks of memory together
		for (int i = 1; i < BlockSize; i++)
		{
			mNextFreeBlock->Next = (PoolElement*)(&RawMem[i * BlockGap]);
			mNextFreeBlock = mNextFreeBlock->Next;
		}

		// Point back to the beginning of the free list
		mNextFreeBlock->Next = nullptr;
		mNextFreeBlock = (PoolElement*)(RawMem);
	}

	~FPoolAllocator()
	{
		ASSERT(mObjectsConstructed == 0 && "All objects should be back in the pool on destruction.");

		// Find the start of the allocated chunk of memory
		PoolElement* MemoryStart = mNextFreeBlock;
		mNextFreeBlock = mNextFreeBlock->Next;
		while (mNextFreeBlock)
		{
			MemoryStart = std::min(MemoryStart, mNextFreeBlock);
			mNextFreeBlock = mNextFreeBlock->Next;
		}

		FMemory::FreeAligned(MemoryStart);
	}

	/**
	* Allocate a new element from the memory pool.
	* If the memory pool is empty, nullptr is returned.
	*/
	void* Allocate()
	{
		if (!mNextFreeBlock)
			return nullptr;

		mObjectsConstructed++;
		void* Memory = mNextFreeBlock;
		mNextFreeBlock = mNextFreeBlock->Next;

		return Memory;
	}

	/**
	* Release an object back into the memory pool.
	*/
	void Free(void* Data)
	{
		ASSERT(mObjectsConstructed > 0);
		mObjectsConstructed--;
		PoolElement* Element = (PoolElement*)Data;
		Element->Next = mNextFreeBlock;
		mNextFreeBlock = Element;
	}

	/**
	* Gets the max number of objects that can be
	* allocated from the pool.
	*/
	uint32_t Capacity() const
	{
		return BlockSize;
	}

	/**
	* Returns the number of objects in the pool
	* that are constructed.
	*/
	uint32_t Size() const
	{
		return mObjectsConstructed;
	}

private:
	// Disable copy ctor and copy assignment
	FPoolAllocator(const FPoolAllocator& Other) = delete;
	FPoolAllocator& operator=(const FPoolAllocator& Other) = delete;

	struct PoolElement
	{
		PoolElement* Next{nullptr};
	};

private:
	PoolElement* mNextFreeBlock; // Entry into the freelist
	uint32_t mObjectsConstructed; // Number of active objects from the pool
};



template <typename ElementType, uint32_t BlockSize>
/**
* A wrapper class of FPoolAllocator for conveniently creating a 
* pool for a specific object type. All functions are inlined, so
* there is no performance difference between using this class versus
* the raw FPoolAllocator class.
* \n
* @param ElementType The object contained within the pool
* @param BlockSize The max number of objects contained in the pool.
*/
class FPoolAllocatorType : private FPoolAllocator<sizeof(ElementType), BlockSize>
{
public:
	FPoolAllocatorType(uint8_t Alignment)
		:FPoolAllocator(Alignment)
	{

	}

	~FPoolAllocatorType() = default;

	/**
	* Allocate a new element from the memory pool.
	* If the memory pool is empty, nullptr is returned.
	*/
	ElementType* Allocate()
	{
		return reinterpret_cast<ElementType*>(FPoolAllocator::Allocate());
	}

	/**
	* Release an object back into the memory pool.
	*/
	void Free(ElementType* Data)
	{
		FPoolAllocator::Free((void*)Data);
	}

	/**
	* Gets the max number of objects that can be
	* allocated from the pool.
	*/
	uint32_t Capacity() const
	{
		return FPoolAllocator::Capacity();
	}

	/**
	* Returns the number of objects in the pool
	* that are constructed.
	*/
	uint32_t Size() const
	{
		return FPoolAllocator::Size();
	}
};