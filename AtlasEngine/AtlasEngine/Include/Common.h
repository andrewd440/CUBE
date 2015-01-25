#pragma once

#define WIN_ALIGN(Size) __declspec(align(16))
#define FOR(i, Num) for(int32_t i = 0; i < Num; i++)