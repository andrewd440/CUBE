#pragma once

#include <cstdint>
#include <vector>
#include <queue>
#include <functional>

#include "Misc\Assertions.h"

/**
* Allocation Strategy
* This allocator keeps elements as dense as possible up front
* without relocating elements when an object is removed from the
* container. In order to provide faster iterating through the list of
* all objects that are currently constructed, an active index list is maintained
* as elements are allocated. The iterator for this allocator grants read access
* to this active list. As elements are added, the first avaliable slot in the list
* is used.
*/
class FRawGappedArray
{
public:
	// Forward decl
	template <typename ElementType>
	class Iterator;

public:
	/**
	* Default Ctor
	*/
	FRawGappedArray();

	/**
	* Dtor
 	* Free held data.
	*/
	~FRawGappedArray();

	/**
	* Initializes this allocator for a specific capacity, respective to element size and alignment
	* of that element type.
	* If this is called on an already initialized object, all current data is cleared and reinitialized
	* according to the new data specs.
	* @param BlockSize - The requested starting capacity for this allocator.
	* @param ElementSize - The size of elements allocated by this allocator.
	* @param Alignment - The memory alignment for the element type.
	*/
	void Init(const uint32_t BlockSize, const uint32_t ElementSize, const uint32_t Alignment = 1);

	template <typename T>
	/**
	* Initializes this allocator for a specific capacity.
	* If this is called on an already initialized object, all current data is cleared and reinitialized
	* according to the new data specs.
	* @tparam T - The element type for this allocator.
	* @param BlockSize - The requested starting capacity for this allocator.
	*/
	void Init(const uint32_t BlockSize);

	/**
	* Allocates an element in the first available slot in the container.
	* @return The index of the new element.
	*/
	uint32_t Allocate();

	template <typename T>
	/**
	* Allocate and construct an element in the first available slot in the container.
	* @tparam T - The type of object to construct.
	* @return The index of the new element.
	*/
	uint32_t AllocateAndConstruct();

	template <typename T, typename Param>
	/**
	* Allocate and construct an element in the first available slot in the container.
	* @tparam T - The type of object to construct.
	* @tparam Param - Type of the first constructor argument for the type.
	* @param Arg1 - Constructor argument for the object type.
	* @return The index of the new element.
	*/
	uint32_t AllocateAndConstruct(const Param& Arg1);

	/**
	* Frees data back into the allocator.
	* @param Index - The index of the element to free.
	*/
	void Free(uint32_t Index);

	template <typename T>
	/**
	* Indexes into the data container held by the allocator. 
	* Only use this if you know the index of an allocated object.
	* For iteration of allocated objects, use FGapArrayAllocator::Begin() and
	* FGapArrayAllocator::End() iterators.
	*/
	T& At(const uint32_t Index);

	template <typename T>
	/**
	* Indexes into the data container held by the allocator.
	* Only use this if you know the index of an allocated object.
	* For iteration of allocated objects, use FGapArrayAllocator::Begin() and
	* FGapArrayAllocator::End() iterators.
	*/
	const T& At(const uint32_t Index) const;

	template <typename T>
	/**
	* An iterator to the beginning of this container.
	* @tparam T - The type of elements held in this container.
	*/
	Iterator<T> Begin();

	template <typename T>
	/**
	* An iterator to the beginning of this container.
	* @tparam T - The type of elements held in this container.
	*/
	Iterator<T> End();

	/**
	* Get the current capacity.
	*/
	uint32_t Capacity() const { return mActiveList.capacity(); }

	/**
	* Get the number of allocated objects.
 	*/
	uint32_t Size() const { return mActiveList.size(); }

private:
	/**
	* Resizes this allocator. This simply copies all data
	* to a new location in memory with a larger capacity. This will no
	* invalidate indeices into this object.
	*/
	void Resize(const uint32_t NewSize);

private:
	uint32_t mElementSize;
	uint32_t mAlignment;
	uint8_t* mData;
	std::vector<uint32_t> mActiveList; // Ordered list of active elements
	std::priority_queue<uint32_t, std::vector<uint32_t>, std::greater<uint32_t>> mDeadList; // Min-heap of dead elements


public:
	template <typename ElementType>
	class Iterator : public std::iterator<std::forward_iterator_tag, ElementType>
	{
	public:
		Iterator()
			: mIndex(0)
			, mActiveList()
			, mContainer()
		{
		}

		Iterator(FRawGappedArray& Array, const std::vector<uint32_t>& ActiveList, const uint32_t StartingIndex = 0)
			: mIndex(StartingIndex)
			, mActiveList(ActiveList)
			, mContainer(Array)
		{
		}

		Iterator(const Iterator& Other)
			: mIndex(Other.mIndex)
			, mActiveList(Other.mActiveList)
			, mContainer(Other.mContainer)
		{
		}

		~Iterator() = default;

		Iterator& operator=(const Iterator& Other)
		{
			mIndex = Other.mIndex;
			mActiveList = Other.mActiveList;
			mContainer = Other.mContainer;
		}

		Iterator& operator++()
		{
			mIndex++;
			return *this;
		}

		Iterator operator++(int32_t)
		{
			Iterator Pre{ *this };
			mIndex++;
			return Pre;
		}

		Iterator& operator+=(uint32_t Offset)
		{
			mIndex += Offset;
			return *this;
		}

		ElementType& operator*()
		{
			return mContainer.At<ElementType>(mActiveList[mIndex]);
		}

		ElementType* operator->()
		{
			return &mContainer.At<ElementType>(mActiveList[mIndex]);
		}

		bool operator==(const Iterator& Other) const
		{
			return mIndex == Other.mIndex && mActiveList == Other.mActiveList && &mContainer == &Other.mContainer;
		}

		bool operator!=(const Iterator& Other) const
		{
			return mIndex != Other.mIndex || mActiveList != Other.mActiveList || &mContainer != &Other.mContainer;
		}

		/**
		* Get the current index of the iterator into the container.
		*/
		uint32_t GetIndex() const
		{
			return mIndex;
		}

	private:
		uint32_t mIndex;
		const std::vector<uint32_t>& mActiveList;
		FRawGappedArray& mContainer;
	};
};


template <typename T>
inline void FRawGappedArray::Init(const uint32_t BlockSize)
{
	Init(BlockSize, sizeof(T), __alignof(T));
}

template <typename T>
inline uint32_t FRawGappedArray::AllocateAndConstruct()
{
	uint32_t Index = Allocate();
	new (mData + Index * mElementSize) T;
	return Index;
}

template <typename T, typename Param>
inline uint32_t FRawGappedArray::AllocateAndConstruct(const Param& Arg1)
{
	uint32_t Index = Allocate();
	new (mData + Index * mElementSize) T(Arg1);
	return Index;
}

template <typename T>
inline T& FRawGappedArray::At(const uint32_t Index)
{
	return *(reinterpret_cast<T*>(mData + Index * mElementSize));
}

template <typename T>
inline const T& FRawGappedArray::At(const uint32_t Index) const
{
	return *(reinterpret_cast<T*>(mData + Index * mElementSize));
}

template <typename T>
inline FRawGappedArray::Iterator<T> FRawGappedArray::Begin()
{
	return FRawGappedArray::Iterator<T>(*this, mActiveList, 0);
}

template <typename T>
inline FRawGappedArray::Iterator<T> FRawGappedArray::End()
{
	return FRawGappedArray::Iterator<T>(*this, mActiveList, mActiveList.size());
}