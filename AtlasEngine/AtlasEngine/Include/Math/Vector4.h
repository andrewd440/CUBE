#pragma once

#ifndef VECTOR4_H
#define VECTOR4_H
#endif

#include <cstdint>
#include <string>
#include <cstdio>

#include "..\Common.h"
#include "Vector3.h"

template <typename T>
/**
* Class of manipulating a 4 component vector.
*/
class WIN_ALIGN(16) TVector4
{
public:
	/**
	* Constructs vector with x, y, z, w components.
	* @param X - value of X component
	* @param Y - value of Y component
	* @param Z - value of Z component
	* @param W - value of W component
	*/
	explicit TVector4(const T X = 0, const T Y = 0, const T Z = 0, const T W = 1);

	/**
	* Constructs vector with first 3 components of
	* TVector3.
	* @param Vec3 Vector with x, y, z components.
	* @param W - The fourth component of the vec4
	*/
	TVector4(const TVector3<T>& Vec3, const T W);

	/**
	* Default copy constructor.
	*/
	TVector4(const TVector4<T>& Other) = default;

	/**
	* Default copy assignment.
	*/
	TVector4<T>& operator=(const TVector4<T>& Other) = default;

	template <typename U>
	/**
	* Performs vector-scalar multiplication.
	* @param Scalar - Unit to multiply by.
	* @return Reference to this vector.
	*/
	TVector4<T>& operator+=(const U Scalar);

	template <typename U>
	/**
	* Performs vector division.
	* @param Scalar - Unit to divide by.
	* @return Reference to this vector
	*/
	TVector4<T>& operator-=(const U Scalar);

	template <typename U>
	/**
	* Performs vector-scalar multiplication.
	* @param Scalar - Unit to multiply by.
	* @return Reference to this vector.
	*/
	TVector4<T>& operator*=(const U Scalar);

	template <typename U>
	/**
	* Performs vector division.
	* @param Scalar - Unit to divide by.
	* @return Reference to this vector
	*/
	TVector4<T>& operator/=(const U Scalar);

	/**
	* Performs vector addition.
	* @param Rhs - TVector4 to add.
	* @return Reference to this vector.
	*/
	TVector4<T>& operator+=(const TVector4<T>& Rhs);

	/**
	* Performs vector subtraction.
	* @param Rhs - TVector4 to subtract
	* @return Reference to this vector
	*/
	TVector4<T>& operator-=(const TVector4<T>& Rhs);

	/**
	* Performs component-wise vector multiplcation.
	* @param Rhs - TVector4 to add.
	* @return Reference to this vector.
	*/
	TVector4<T>& operator*=(const TVector4<T>& Rhs);

	/**
	* Performs component-wise vector division.
	* @param Rhs - TVector4 to subtract
	* @return Reference to this vector
	*/
	TVector4<T>& operator/=(const TVector4<T>& Rhs);

	/**
	* Checks vector equality.
	*/
	bool operator==(const TVector4<T>& Rhs) const;

	/**
	* Checks vector inequality.
	*/
	bool operator!=(const TVector4<T>& Rhs) const;

	/**
	* Overload of subscript operators for X = 0, Y = 1, Z = 2, W = 3.
	*/
	T& operator[](uint32_t idx);

	/**
	* Overload of subscript operators for X = 0, Y = 1, Z = 2, W = 3..
	*/
	const T& operator[](uint32_t idx) const;

	/**
	* Normalizes x, y, z components of the vector.
	*/
	TVector4<T>& Normalize3();

	/**
	* Calculates the length/magnatude of the vector with all four
	* components.
	* @return The length/magnatude
	*/
	float Length4() const;

	/**
	* Calculates the length/magnatude of the vector with x, y, z
	* components.
	* @return The length/magnatude
	*/
	float Length3() const;

	/**
	* Calculates the length squared of all four components of the vector.
	* When an absolute length is not need, such as
	* when vector-vector comparisions, use this instead of
	* Lenght() to aviod squareroot calculation.
	*/
	float LengthSquared4() const;

	/**
	* Calculates the length squared of three components of the vector.
	* When an absolute length is not need, such as
	* when vector-vector comparisions, use this instead of
	* Lenght() to aviod squareroot calculation.
	*/
	float LengthSquared3() const;

	/**
	* Reflects the vector against a normal.
	* @param Normal the normal being reflected by
	* @return Reflected vector
	*/
	TVector4<T> Reflect3(const TVector4<T>& Normal) const;

	/**
	* Projects this vector onto the axis of another vector. This does not
	* assume that Vector is normalized, use ProjectedOnNormal
	* if Vector is already normalized.
	* @param Vector to project upon
	* @return The projected vector.
	*/
	TVector4<T> ProjectedOnVector(const TVector4<T>& Vector) const;

	/**
	* Projects this vector onto the axis of normalized vector. This assumes
	* that Vector is normalized, use ProjectedOnVector if Vector
	* is not already normalized.
	* @param Vector to project upon
	* @return The projected vector.
	*/
	TVector4<T> ProjectedOnNormal(const TVector4<T>& Normal) const;

	/* Returns string representation of the vector */
	const std::string ToString() const;

	/**
	* Calculates the dot product of two vectors with x, y, and z components.
	* @param lhs - Left operand
	* @param rhs - Right operand
	* @return
	*/
	static T Dot3(const TVector4<T>& lhs, const TVector4<T>& rhs);

	/**
	* Calculates the dot product of two vectors with all components.
	* @param lhs - Left operand
	* @param rhs - Right operand
	* @return
	*/
	static T Dot4(const TVector4<T>& lhs, const TVector4<T>& rhs);

	/**
	* Calculates the cross product of two vectors with x, y, z components.
	* W component is always 0.
	* @param lhs - Left operand
	* @param rhs - Right operand
	* @return The cross product vector.
	*/
	static TVector4<T> Cross(const TVector4<T>& lhs, const TVector4<T>& rhs);

	/**
	* Checks if two directions, (X Y Z) components, are perpendicular, or orthogonal, to one another,
	* within a certain threshold. The normals given are assumed to be unit length.
	* @param Normal1 Direction to check
	* @param Normal2 Direction to check
	* @param CosineThreshold Threshold to test against
	* @return True if the cosine of the angle between the direction is within the threshold
	*/
	static bool Perpendicular(const TVector4<T>& Normal1, const TVector4<T>& Normal2, const float CosineThreshold);

	/**
	* Checks if two directions, (X Y Z) components, are parallel, to one another.  The normals given are 
	* assumed to be unit length.
	* This test ignores checking if the directions point in this same direction,
	* it checks if the angle between them is within a certain threshold.
	* @param Normal1 Direction to check
	* @param Normal2 Direction to check
	* @param CosineThreshold Threshold to test against
	* @return True if the cosine of the angle between the direction is within the threshold
	*/
	static bool Parallel(const TVector4<T>& Normal1, const TVector4<T>& Normal2, const float CosineThreshold);

public:
	T x; /* X component of the vector */
	T y; /* Y component of the vector */
	T z; /* Z component of the vector */
	T w; /* W component of the vector */
};

using Vector4i = TVector4<int32_t>;		/* Vector type for 32-bit signed integers */
using Vector4ui = TVector4<uint32_t>;	/* Vector type for 32-bit unsigned integers */
using Vector4f = TVector4<float>;		/* Vector type for floats */

#include "Vector.inl"
