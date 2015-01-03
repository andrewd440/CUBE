#pragma once

#include <cstdint>

namespace FMemory
{
	// TODO: Add comments to FMemory
	void* AllocateUnaligned(const uint32_t Bytes);

	void* AllocateAligned(const uint32_t Bytes, const uint32_t Alignment);

	void FreeUnaligned(void* Data);

	void FreeAligned(void* Data);
}