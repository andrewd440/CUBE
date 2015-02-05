#pragma once

#include "Vector3.h"
#include "Vector4.h"
#include "Transform.h"

/**
* Represents an infinite 3D plane.
**/
class FPlane
{
public:
	Vector3f Normal;
	float DistanceFromOrigin;

public:
	/**
	* Default Ctor
	* Constructs a plane at origin, pointing upward.
	*/
	FPlane();

	/**
	* Ctor
	* Constructs a plane from a plane normal and a point on the plane.
	* @param PlaneNormal - The normal of the plane.
	* @param PointOnPlane - A point of the plane.
	*/
	FPlane(const Vector3f& PlaneNormal, const Vector3f& PointOnPlane);

	/**
	* Ctor
	* Constructs a plane from a plane normal and a point on the plane.
	* @param PlaneNormal - The normal of the plane.
	* @param DistanceFromOrigin - The distance of the plane from origin.
	*/
	FPlane(const Vector3f& PlaneNormal, const float DistanceFromOrigin);

	/**
	* Ctor
	* Constructs a plane from 3 points. (ccw ordering)
	* @param P1 - First point.
	* @param P2 - Second point.
	* @param P3 - Third point.
	*/
	FPlane(const Vector3f& P1, const Vector3f& P2, const Vector3f& P3);
	
	/**
	* Constructs a plane from a 4 component vector. x, y, z is the normal.
	* w is the distance from origin.
	*/
	FPlane(const Vector4f& Plane);

	/**
	* Copy Ctor
	*/
	FPlane(const FPlane& Other) = default;

	/**
	* Dtor
	*/
	~FPlane() = default;

	/**
	* Copy assignment.
	*/
	FPlane& operator=(const FPlane& Other) = default;

	/**
	* Calculate the signed distance a point is away from this plane.
	*/
	float DistanceFromPoint(const Vector3f& Point) const;

	/**
	* Transforms this plane by a matrix.
	*/
	void TransformBy(const FMatrix4& Transform);
};

inline FPlane::FPlane()
	: Normal(0, 1, 0)
	, DistanceFromOrigin(0.0f)
{
}

inline FPlane::FPlane(const Vector3f& PlaneNormal, const Vector3f& PointOnPlane)
	: Normal(PlaneNormal)
	, DistanceFromOrigin(Vector3f::Dot(PlaneNormal, PointOnPlane))
{

}

inline FPlane::FPlane(const Vector3f& PlaneNormal, const float DistanceFromOrigin)
	: Normal(PlaneNormal)
	, DistanceFromOrigin(DistanceFromOrigin)
{}

inline FPlane::FPlane(const Vector3f& P1, const Vector3f& P2, const Vector3f& P3)
	: Normal()
	, DistanceFromOrigin(0.0f)
{
	Normal = Vector3f::Cross((P2 - P1), (P3 - P1)).Normalize();
	DistanceFromOrigin = Vector3f::Dot(P1, Normal);
}

inline FPlane::FPlane(const Vector4f& Plane)
	: Normal(Plane.x, Plane.y, Plane.z)
	, DistanceFromOrigin(Plane.w)
{

}

inline void FPlane::TransformBy(const FMatrix4& Transform)
{
	// From Mathematics for 3D Game Programming and Computer Graphics by Eric Lengyel p. 101

	// We can treat the plane as a 4 dimensional vector and transform it similar to 
	// transforming normal vectors with the transpose of the inverse.

	const FMatrix4& CorrectTransform = Transform.GetInverse().Transpose();
	Normal = CorrectTransform.TransformDirection(Normal);
	DistanceFromOrigin = CorrectTransform.TransformDirection(Vector3f(DistanceFromOrigin, DistanceFromOrigin, DistanceFromOrigin)).x;
}

inline float FPlane::DistanceFromPoint(const Vector3f& Point) const
{
	return (Vector3f::Dot(Point, Normal) - DistanceFromOrigin) / Vector3f::Dot(Normal, Normal);
}

inline bool operator==(const FPlane& Lhs, const FPlane& Rhs)
{
	return Lhs.Normal == Rhs.Normal && Lhs.DistanceFromOrigin == Rhs.DistanceFromOrigin;
}

inline bool operator!=(const FPlane& Lhs, const FPlane& Rhs)
{
	return Lhs.Normal != Rhs.Normal || Lhs.DistanceFromOrigin != Rhs.DistanceFromOrigin;
}