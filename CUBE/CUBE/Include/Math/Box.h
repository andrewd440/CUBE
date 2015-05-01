#pragma once
#include "Vector3.h"

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
	FBox();

	Vector3f GetCenter() const { return (Min + Max) * 0.5f; }

	void TransformAABB(const FMatrix4& Transform);
	Vector3f GetDimensions() const;
};

inline Vector3f FBox::GetDimensions() const
{
	return Max - Min;
}