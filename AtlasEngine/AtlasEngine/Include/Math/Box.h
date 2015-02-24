#pragma once
#include "Vector3.h"
#include <limits>

struct FMatrix4;

struct FBox
{
	Vector3f Min;
	Vector3f Max;
	
	/**
	* Constucts a box with the min point at the floating
	* point max value and max point at the floating point
	* min value.
	*/
	FBox()
		: Min(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max())
		, Max(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max())
	{}

	Vector3f GetCenter() const { return (Min + Max) * 0.5f; }

	void TransformAABB(const FMatrix4& Transform);
	Vector3f GetDimensions() const;
};

inline Vector3f FBox::GetDimensions() const
{
	return Max - Min;
}