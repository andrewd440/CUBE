#pragma once

#include "Vector3.h"
#include "Vector4.h"
#include "Transform.h"

/**
* Represents an infinite 3D plane.
**/
WIN_ALIGN(16)
struct FPlane
{
	Vector4f NormalwDistance;

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
	* Tests a point for intersection with this plane.
 	* @return If 0, the plane intersects the point. If positive,
	*         the point is in front of the plane. If negative, the point
	*         is behind the plane.
	*/
	float IntersectsPoint(const Vector4f& Point) const;

	/**
	* Transforms this plane by a matrix.
	*/
	void TransformBy(const FMatrix4& Transform);

	/**
	* Normalizes the plane equation.
	*/
	FPlane& Normalize();
};

inline FPlane::FPlane()
	: NormalwDistance(0, 1, 0, 0)
{
}

inline FPlane::FPlane(const Vector3f& PlaneNormal, const Vector3f& PointOnPlane)
	: NormalwDistance(PlaneNormal, Vector3f::Dot(PlaneNormal, PointOnPlane))
{

}

inline FPlane::FPlane(const Vector3f& PlaneNormal, const float DistanceFromOrigin)
	: NormalwDistance(PlaneNormal, DistanceFromOrigin)
{}

inline FPlane::FPlane(const Vector3f& P1, const Vector3f& P2, const Vector3f& P3)
	: NormalwDistance()
{
	NormalwDistance = Vector4f{ Vector3f::Cross((P2 - P1), (P3 - P1)).Normalize(), 0};
	NormalwDistance.w = Vector4f::Dot3(Vector4f{ P1, 0 }, NormalwDistance);
}

inline FPlane::FPlane(const Vector4f& Plane)
	: NormalwDistance(Plane.x, Plane.y, Plane.z, Plane.w)
{

}

inline void FPlane::TransformBy(const FMatrix4& Transform)
{
	// From Mathematics for 3D Game Programming and Computer Graphics by Eric Lengyel p. 101

	// We can treat the plane as a 4 dimensional vector and transform it similar to 
	// transforming normal vectors with the transpose of the inverse.

	FMatrix4 CorrectTransform = Transform.GetInverse().Transpose();
	NormalwDistance = CorrectTransform.TransformVector(NormalwDistance);
}

inline FPlane& FPlane::Normalize()
{
	const float InvNormalLength = 1.0f / NormalwDistance.Length3();
	NormalwDistance *= InvNormalLength;
	return *this;
}

inline float FPlane::DistanceFromPoint(const Vector3f& Point) const
{
	const Vector3f Normal{ NormalwDistance.x, NormalwDistance.y, NormalwDistance.z };
	return (Vector3f::Dot(Point, Normal) - NormalwDistance.w) / Vector3f::Dot(Normal, Normal);
}

inline float FPlane::IntersectsPoint(const Vector4f& Point) const
{
	return Vector4f::Dot4(NormalwDistance, Point);
}

inline bool operator==(const FPlane& Lhs, const FPlane& Rhs)
{
	return Lhs.NormalwDistance == Rhs.NormalwDistance;
}

inline bool operator!=(const FPlane& Lhs, const FPlane& Rhs)
{
	return Lhs.NormalwDistance != Rhs.NormalwDistance;
}