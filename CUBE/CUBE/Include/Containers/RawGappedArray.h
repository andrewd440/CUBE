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
* is used. When more memory is needed for new objects, a new page is created that
* is the size of the specified pages in Init.
*/
class FTypelessPageArray
{
public:
	// Forward decl
	template <typename ElementType>
	class Iterator;

public:
	/**
	* Default Ctor
	*/
	FTypelessPageArray();

	/**
	* Dtor
 	* Free held data.
	*/
	~FTypelessPageArray();

	/**
	* Initializes this allocator for a specific capacity, respective to element size and alignment
	* of that element type.
	* If this is called on an already initialized object, all current data is cleared and reinitialized
	* according to the new data specs.
	* @param PageSize - The requested starting capacity for each memory page.
	* @param ElementSize - The size of elements allocated by this allocator.
	* @param Alignment - The memory alignment for the element type.
	*/
	void Init(const uint32_t PageSize, const uint32_t ElementSize, const uint32_t Alignment = 1);

	template <typename T>
	/**
	* Initializes this allocator for a specific capacity.
	* If this is called on an already initialized object, all current data is cleared and reinitialized
	* according to the new data specs.
	* @tparam T - The element type for this allocator.
	* @param PageSize - The requested starting capacity for each memory page.
	*/
	void Init(const uint32_t PageSize);

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

	template <typename T, typename Param>
	uint32_t AllocateAndConstruct(Param& Arg1);

	/**
	* Frees data back into the allocator.
	* @param Index - The index of the element to free.
	*/
	void Free(uint32_t Index);

	template <typename T>
	/**
	* Typed index into the data container held by the allocator. 
	* Only use this if you know the index of an allocated object.
	* For iteration of allocated objects, use FGapArrayAllocator::Begin() and
	* FGapArrayAllocator::End() iterators.
	*/
	T& At(const uint32_t Index);

	template <typename T>
	/**
	* Typed index into the data container held by the allocator.
	* Only use this if you know the index of an allocated object.
	* For iteration of allocated objects, use FGapArrayAllocator::Begin() and
	* FGapArrayAllocator::End() iterators.
	*/
	const T& At(const uint32_t Index) const;

	/**
	* Untyped index into the data container held by the allocator.
	* Only use this if you know the index of an allocated object.
	* For iteration of allocated objects, use FGapArrayAllocator::Begin() and
	* FGapArrayAllocator::End() iterators.
	*/
	void* operator[](const size_t Index);

	/**
	* Untyped index into the data container held by the allocator.
	* Only use this if you know the index of an allocated object.
	* For iteration of allocated objects, use FGapArrayAllocator::Begin() and
	* FGapArrayAllocator::End() iterators.
	*/
	const void* operator[](const size_t Index) const;

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
	uint32_t Capacity() const { return mPageSize * mPages.size(); }

	/**
	* Get the number of allocated objects.
 	*/
	uint32_t Size() const { return mActiveCount; }

private:

	void AddPage();

	void DeleteAllPages();

	void AssignNextFreePage();

private:
	struct Page
	{
		uint8_t* Data;
		std::vector<uint32_t> ActiveList; // Ordered list of active elements
		std::priority_queue<uint32_t, std::vector<uint32_t>, std::greater<uint32_t>> DeadList; // Min-heap of dead elements
	};

	uint32_t          mElementSize;
	uint32_t          mAlignment;
	uint32_t          mPageSize;
	uint32_t          mActiveCount;
	uint32_t          mNextFreePage;
	std::vector<Page> mPages;

public:
	template <typename T>
	friend class Iterator;
	template <typename ElementType>
	class Iterator : public std::iterator<std::forward_iterator_tag, ElementType>
	{
	public:
		Iterator()
			: mCurrentPage(0)
			, mIndex(0)
			, mContainer(nullptr)
		{
		}

		Iterator(FTypelessPageArray& Array, const uint32_t CurrentPage, const uint32_t Index)
			: mCurrentPage(CurrentPage)
			, mIndex(Index)
			, mContainer(&Array)
		{
			mElementSize = mContainer->mElementSize;
		}

		Iterator(const Iterator& Other)
			: mCurrentPage(Other.mCurrentPage)
			, mIndex(Other.mIndex)
			, mContainer(Other.mContainer)
			, mElementSize(Other.mElementSize)
		{
		}

		~Iterator() = default;

		Iterator& operator=(const Iterator& Other)
		{
			mCurrentPage = Other.mCurrentPage;
			mIndex = Other.mIndex;
			mContainer = Other.mContainer;
			mElementSize = Other.mElementSize;
		}

		Iterator& operator++()
		{
			if (mIndex >= mContainer->mPages[mCurrentPage].ActiveList.size() - 1)
			{
				mIndex = 0;
				mCurrentPage++;
			}
			else
			{
				mIndex++;
			}

			return *this;
		}

		Iterator operator++(int32_t)
		{
			Iterator Pre{ *this };

			++(*this);

			return Pre;
		}

		ElementType& operator*()
		{
			const uint32_t PageIndex = mContainer->mPages[mCurrentPage].ActiveList[mIndex];
			return *reinterpret_cast<ElementType*>(mContainer->mPages[mCurrentPage].Data + (mElementSize * PageIndex));
		}

		ElementType* operator->()
		{
			const uint32_t PageIndex = mContainer->mPages[mCurrentPage].ActiveList[mIndex];
			return reinterpret_cast<ElementType*>(mContainer->mPages[mCurrentPage].Data + (mElementSize * PageIndex));
		}

		bool operator==(const Iterator& Other) const
		{
			return mIndex == Other.mIndex && mCurrentPage == Other.mCurrentPage && mContainer == Other.mContainer;
		}

		bool operator!=(const Iterator& Other) const
		{
			return mIndex != Other.mIndex || mCurrentPage != Other.mCurrentPage || mContainer != Other.mContainer;
		}

		/**
		* Get the current index of the iterator into the container.
		*/
		uint32_t GetIndex() const
		{
			return mIndex;
		}

		uint32_t GetPage() const
		{
			return mCurrentPage;
		}

	private:
		uint32_t mElementSize;
		uint32_t mCurrentPage;
		uint32_t mIndex;
		FTypelessPageArray* mContainer;
	};
};


template <typename T>
inline void FTypelessPageArray::Init(const uint32_t BlockSize)
{
	Init(BlockSize, sizeof(T), __alignof(T));
}

template <typename T>
inline uint32_t FTypelessPageArray::AllocateAndConstruct()
{
	uint32_t Index = Allocate();
	new (*this[Index]) T;
	return Index;
}

template <typename T, typename Param>
inline uint32_t FTypelessPageArray::AllocateAndConstruct(const Param& Arg1)
{
	uint32_t Index = Allocate();
	new (*this[Index]) T(Arg1);
	return Index;
}

template <typename T, typename Param>
inline uint32_t FTypelessPageArray::AllocateAndConstruct(Param& Arg1)
{
	uint32_t Index = Allocate();
	new (*this[Index]) T(Arg1);
	return Index;
}

template <typename T>
inline T& FTypelessPageArray::At(const uint32_t Index)
{
#ifndef NDEBUG
	const uint32_t PageID = Index / mPageSize;
	const uint32_t ElementID = Index % mPageSize;
	ASSERT(std::find(mPages[PageID].ActiveList.begin(), mPages[PageID].ActiveList.end(), ElementID) != mPages[PageID].ActiveList.end() && "Trying to access dead element.");
#endif
	return *(reinterpret_cast<T*>((*this)[Index]));
}

template <typename T>
inline const T& FTypelessPageArray::At(const uint32_t Index) const
{
#ifndef NDEBUG
	const uint32_t PageID = Index / mPageSize;
	const uint32_t ElementID = Index % mPageSize;
	ASSERT(std::find(mPages[PageID].ActiveList.begin(), mPages[PageID].ActiveList.end(), ElementID) != mPages[PageID].ActiveList.end() && "Trying to access dead element.");
#endif
	return *(reinterpret_cast<T*>((*this)[Index]));
}

template <typename T>
inline FTypelessPageArray::Iterator<T> FTypelessPageArray::Begin()
{
	return FTypelessPageArray::Iterator<T>(*this, 0, 0);
}

template <typename T>
inline FTypelessPageArray::Iterator<T> FTypelessPageArray::End()
{
	return FTypelessPageArray::Iterator<T>(*this, mPages.size(), 0);
}

inline void* FTypelessPageArray::operator[](const size_t Index) 
{ 
	const uint32_t PageID = Index / mPageSize;
	const uint32_t ElementID = Index % mPageSize;
	return mPages[PageID].Data + (ElementID * mElementSize);
}

inline const void* FTypelessPageArray::operator[](const size_t Index) const 
{ 
	const uint32_t PageID = Index / mPageSize;
	const uint32_t ElementID = Index % mPageSize;
	return mPages[PageID].Data + (ElementID * mElementSize);
}