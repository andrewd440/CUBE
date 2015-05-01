#include "..\..\Include\Memory\StackAllocator.h"
#include "../../Include/Misc/Assertions.h"

FStackAllocator::FStackAllocator(const uint32_t SizeBytes)
	: mCurrentMarker(0)
	, mCapacity(SizeBytes)
{
	ASSERT(SizeBytes > 0);
	mData = new uint8_t[SizeBytes];
}

FStackAllocator::~FStackAllocator()
{
	delete[] mData;
}

void* FStackAllocator::Allocate(const uint32_t Bytes)
{
	ASSERT(Bytes + mCurrentMarker <= mCapacity && "StackAllocator is out of memory.");

	void* Data = &mData[mCurrentMarker];
	mCurrentMarker += Bytes;

	return Data;
}

void* FStackAllocator::Allocate(const uint32_t Bytes, const uint32_t Alignment)
{
	ASSERT(Bytes + Alignment + mCurrentMarker <= mCapacity  && "StackAllocator is out of memory.");
	AlignData(Alignment);
	return Allocate(Bytes);
}

FStackAllocator::UMarker FStackAllocator::GetMarker() const
{
	return mCurrentMarker;
}

void FStackAllocator::Clear()
{
	mCurrentMarker = 0;
}

void FStackAllocator::ClearToMarker(const UMarker Marker)
{
	ASSERT(Marker >= 0 && Marker <= mCapacity && "Invalid UMarker value.");
	mCurrentMarker = Marker;
}


void FStackAllocator::AlignData(const uint32_t Alignment)
{
	ASSERT((Alignment & (Alignment - 1)) == 0x0 && "Alignments must be a power of 2.");
	ASSERT(Alignment > 0 && "Alignments must be greater than 0.");

	// Determine how must we a misaligned
	const uint32_t Alignmask = (Alignment - 1);
	const uintptr_t DataAddress = reinterpret_cast<uintptr_t>(&mData[mCurrentMarker]);
	const uintptr_t Misalignment = (DataAddress & Alignmask);

	if (Misalignment == 0)
		return;

	const ptrdiff_t Adjustment = Alignment - Misalignment;

	// Add the adjustment of the current marker
	mCurrentMarker += Adjustment;
}