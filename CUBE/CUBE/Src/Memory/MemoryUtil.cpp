#include "../Include/Memory/MemoryUtil.h"
#include "../Include/Misc/Assertions.h"

namespace FMemory
{
	void* AllocateUnaligned(uint32_t Bytes)
	{
		return (void*)new uint8_t[Bytes];
	}

	void* AllocateAligned(uint32_t Bytes, uint32_t Alignment)
	{
		ASSERT(Alignment >= 1 && "Aligned allocation shouldn't be less than 1 byte.");
		ASSERT(Alignment <= 128 && "Aligned allocations over 128 bytes not supported.");
		ASSERT((Alignment & (Alignment - 1)) == 0x0 && "Alignment must be a power of 2.");

		// Get the maximum needed space to insure an aligned allocation
		const uint32_t ExpandedSize = Bytes + Alignment;

		// Get an unaligned block of memory
		uintptr_t RawMemory = reinterpret_cast<uintptr_t>(AllocateUnaligned(ExpandedSize));

		// Mask the LO bits of the unaligned block of memory to obtain an offset
		// for correct alignment
		const uint32_t AlignMask = (Alignment - 1);
		const uintptr_t Misalignment = AlignMask & RawMemory;
		const ptrdiff_t MemoryAdjustment = Alignment - Misalignment;

		// Align the block of raw memory
		RawMemory += MemoryAdjustment;

		// Store the alignment adjustment in the preceding byte. This is
		// used to retrieve the corrent address when this block of memory is freed.
		ASSERT(MemoryAdjustment < 256);
		uint8_t* AlignedMemory = reinterpret_cast<uint8_t*>(RawMemory);
		AlignedMemory[-1] = static_cast<uint8_t>(MemoryAdjustment);

		return static_cast<void*>(AlignedMemory);
	}

	void FreeUnaligned(void* Data)
	{
		delete[](uint8_t*)Data;
	}

	void FreeAligned(void* Data)
	{
		const uint8_t* Aligned = reinterpret_cast<const uint8_t*>(Data);

		// Get the adjustment used for alignment
		const uintptr_t AlignedAddress = reinterpret_cast<uintptr_t>(Data);
		const ptrdiff_t Adjustment = static_cast<uintptr_t>(Aligned[-1]);

		// Adjust the aligned memory to the original address
		const uintptr_t CompleteAddress = AlignedAddress - Adjustment;

		FreeUnaligned(reinterpret_cast<void*>(CompleteAddress));
	}
}