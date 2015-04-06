#pragma once

#include <cstdint>
#include "Plane.h"

struct FBox;
struct FSphere;

/**
* Represents a volume with 6 planes.
*/
WIN_ALIGN(16)
class FFrustum
{
public:
	/**
	* Types of planes in the frustum.
	*/
	enum PlaneType : uint32_t
	{
		Near,
		Far,
		Left,
		Right,
		Top,
		Bottom
	};

public:
	FFrustum() = default;
	~FFrustum() = default;

	/**
	* Set a specific plane in the frustum.
	*/
	void SetPlane(const PlaneType WhichPlane, const FPlane& NewPlane);

	/**
	* Get a specific plane in the frustum.
	*/
	FPlane GetPlane(const PlaneType WhichPlane) const;

	/**
	* Transform all planes in the frustum by a transform.
	*/
	void TransformBy(const FMatrix4& Transform);

	/**
	* Checks if a uniform aabb (all diminsions of the box is similar in length) is 
	* within the frustum.
	* Don't forget that the AABB and frustum must be in the 
	* same coordinate space.
	* @param CenterPoint - The center of the box.
	* @param Box - The AABB to check.
	*/
	bool IsUniformAABBVisible(const Vector4f& CenterPoint, const float BoxWidth) const;

	/**
	* Checks if a sphere is visible.
	* @param Sphere - The sphere to check.
	*/
	bool IsSphereVisible(const FSphere& Sphere) const;

private:
	FPlane mPlanes[6];
};

inline void FFrustum::SetPlane(const PlaneType WhichPlane, const FPlane& NewPlane)
{
	mPlanes[WhichPlane] = NewPlane;
}

inline FPlane FFrustum::GetPlane(const PlaneType WhichPlane) const
{
	return mPlanes[WhichPlane];
}
