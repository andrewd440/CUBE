#include "Atlas\ComponentHandle.h"

namespace Atlas
{
	FComponentHandle::FComponentHandle(const EComponent::Type Type)
		: mID(Type)
		, mBit(NextBit)
	{
		// Shift to the next bit mask
		NextBit <<= 1;
	}


	EComponent::Type FComponentHandle::GetID() const
	{
		return mID;
	}

	std::bitset<BITSIZE> FComponentHandle::GetBitMask() const
	{
		return mBit;
	}

	std::bitset<BITSIZE> FComponentHandle::NextBit(1);
}