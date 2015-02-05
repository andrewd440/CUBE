#pragma once
#include "Vector3.h"

struct FBox
{
	Vector3f Min;
	Vector3f Max;

	Vector3f GetCenter() const
	{
		return (Min + Max) * 0.5f;
	}
};