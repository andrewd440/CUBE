#include "..\..\Include\Windows\WindowsClock.h"
#include "..\..\Include\Clock.h"

const uint64_t FClock::GetSystemClockFrequency()
{
	LARGE_INTEGER Frequency;
	QueryPerformanceFrequency(&Frequency);
	return Frequency.QuadPart;
}

uint64_t FClock::ReadSystemTimer()
{
	// Make sure we are always using the same thread
	DWORD_PTR OldMask = SetThreadAffinityMask(GetCurrentThread(), 0x1);

	LARGE_INTEGER CurrentClock;
	QueryPerformanceCounter(&CurrentClock);

	// Switch back to old thread
	SetThreadAffinityMask(GetCurrentThread(), OldMask);

	return CurrentClock.QuadPart;
}

const uint64_t FClock::SystemClockFrequency = GetSystemClockFrequency();