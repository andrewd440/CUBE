#pragma once
#include <iostream>
#include <cstdint>

#if ASSERTIONS_ENABLED
 
#define debugBreak() __asm { int 3 };

#define ASSERT(expr) \
	if (expr) { } \
	else \
	{ \
		ReportAssertionFailure(#expr, \
						__FILE__, __LINE__); \
		debugBreak(); \
	}
#else

#define ASSERT(expr) // does nothing

#endif

inline void ReportAssertionFailure(const char* Expr, const char* File, const uint32_t Line)
{
	std::cerr << "Assertion Failed: " << Expr << " File: " << File << " Line: " << Line << std::endl;
}