#include "Containers\RawGappedArray.h"
#include "Memory\MemoryUtil.h"

#include <algorithm>

FRawGappedArray::FRawGappedArray()
	: mElementSize(0)
	, mAlignment(1)
	, mActiveList()
	, mDeadList()
	, mData(nullptr)
{
}


FRawGappedArray::~FRawGappedArray()
{
	if (mData != nullptr)
		FMemory::FreeAligned(mData);
}

void FRawGappedArray::Init(const uint32_t BlockSize, const uint32_t ElementSize, const uint32_t Alignment)
{
	mElementSize = ElementSize;
	mAlignment = Alignment;

	// Delete previous data
	if (mActiveList.size() > 0)
	{
		FMemory::FreeAligned(mData);
		mActiveList.clear();
	}

	// Active list capacity should alway be the same size as the array capacity, in element count.
	mActiveList.reserve(BlockSize);
	
	// Add all elements to dead list
	for (uint32_t i = 0; i < BlockSize; i++)
	{
		mDeadList.push(i);
	}

	mData = (uint8_t*)FMemory::AllocateAligned(std::max(BlockSize, Alignment) * ElementSize, Alignment);
}

uint32_t FRawGappedArray::Allocate()
{
	if (mDeadList.empty())
	{
		Resize(mActiveList.size() * 2 + 1);
	}

	// Get the first free element
	const uint32_t FreeElement = mDeadList.top();
	mDeadList.pop();
	
	// Find the first element that is greater than the newly allocated index and insert before this position
	const auto ActivePosition = std::find_if(mActiveList.begin(), mActiveList.end(), [FreeElement](const uint32_t Index){ return Index > FreeElement; });
	mActiveList.insert(ActivePosition, FreeElement);

	return FreeElement;
}

void FRawGappedArray::Free(const uint32_t Index)
{
	auto ActiveIndex = std::find(mActiveList.begin(), mActiveList.end(), Index);
	ASSERT(ActiveIndex != mActiveList.end() && "Trying to free an inactive element.");
	mActiveList.erase(ActiveIndex);

	// Add to dead list
	mDeadList.push(Index);
}

void FRawGappedArray::Resize(const uint32_t NewSize)
{
	const uint32_t ActiveCap = mActiveList.capacity();
	uint8_t* NewMemory = (uint8_t*)FMemory::AllocateAligned(std::max(NewSize, mAlignment) * mElementSize, mAlignment);
	std::memcpy(NewMemory, mData, std::min(ActiveCap, NewSize) * mElementSize);
	
	// Free old memeory
	FMemory::FreeAligned(mData);

	// Point to new data
	mData = NewMemory;

	// Active list capacity should alway be the same size as the array capacity, in element count.
	if (NewSize < ActiveCap)
		mActiveList.resize(NewSize);
	else
	{
		mActiveList.reserve(NewSize);

		// Now update the dead list
		for (uint32_t i = ActiveCap; i < NewSize; i++)
		{
			mDeadList.push(i);
		}
	}
}