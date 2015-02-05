#pragma once
#include <cstdint>
#include "Vector3.h"

class FPlane;
struct FBox;

const float _PI = 3.14159265f;
const float _EPSILON = 0.00001f;

enum EAxis : uint8_t
{
	X = 0, 
	Y = 1,
	Z = 2,
};

namespace FMath
{
	/**
	* Linearly interpolate between two 3D points.
	* @param From Starting position
	* @param To End position
	* @param Value within the two position
	* @return Position along the line segment at the given value
	*/
	inline Vector3f Lerp(const Vector3f& From, const Vector3f& To, const float Value)
	{
		return ((1 - Value) * From) + (Value * To);
	}

	/**
	* Checks the bounds of a vector against current min and max bounds.
	* The min and max bounds are updated if the new point is outside the current
	* bounds.
	* @param CurrentMin The current min bounds.
	* @param CurrentMax The current max bounds.
	* @param NewPoint The point to check.
	*/
	void UpdateBounds(Vector3f& CurrentMin, Vector3f& CurrentMax, const Vector3f& NewPoint);

	/**
	* Computes the barycentric coordinates of a point in respect
	* to a triangle. If the point is outside the bounds of the 
	* triangle, negative barycentric values will be given.
	* Vertex order should follow clockwise ordering.
	* @param V0 First vertex of the triangle
	* @param V1 Second vertex of the triangle
	* @param V2 Third vertex of the triangle
	* @param SurfacePoint Point to compute barycentric coordinates for.
	* @param BOut Barycentric values are output through this.
	*/
	void ComputeBarycentric(const Vector3f& V0,
								const Vector3f& V1,
								const Vector3f& V2,
								const Vector3f& SurfacePoint,
								float BOut[3]);

	/**
	* Converts degrees to radians.
	*/
	inline float ToRadians(float Degrees)
	{
		static const float PiOverOneEighty = _PI / 180.0f;
		return Degrees * PiOverOneEighty;
	}

	/**
	* Converts radians to degrees.
	*/
	inline float ToDegrees(float Radians)
	{
		static const float OneEightyOverPi = 180.0f / _PI;
		return Radians * OneEightyOverPi;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////// Intersection Tests ///////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	enum class IntersectionType
	{
		Front, // For planes
		Behind,
		Straddle,
		Inside, // For boxes, sphere
		Outside
	};

	/**
	* Plane-AABB intersection test.
	*/
	IntersectionType Intersects(const FPlane& Plane, const FBox& Box);
}