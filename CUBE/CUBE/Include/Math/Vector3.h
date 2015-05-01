#pragma once

#include <cstdint>
#include <cstdio>
#include <string>

#include "Common.h"

template <typename T>
class TVector4;

template <typename T>
/**
* Class of manipulating a vector with 3 components.
*/
class TVector3
{
public:
	static const TVector3<T> Forward;
	static const TVector3<T> Backward;
	static const TVector3<T> Left;
	static const TVector3<T> Right;
	static const TVector3<T> Up;
	static const TVector3<T> Down;

public:
	/**
	* Constructs vector with x, y, z coordinates.
	* @param X - position of X coordinate
	* @param Y - position of Y coordinate
	* @param Z - position of Z coordinate
	*/
	explicit TVector3(T X = 0, T Y = 0, T Z = 0);

	template <typename U>
	/**
	* Constructs vector with x, y, z components.
	* @param X - value of X component
	* @param Y - value of Y component
	* @param Z - value of Z component
	*/
	TVector3(const U X, const U Y, const U Z);

	/**
	* Default copy constructor.
	*/
	TVector3(const TVector3<T>& Other) = default;

	template <typename U>
	TVector3(const TVector4<U>& Other);

	/**
	* Default copy assignment.
	*/
	TVector3<T>& operator=(const TVector3<T>& Other) = default;

	template <typename U>
	TVector3<T>(const TVector3<U>& Other);

	template <typename U>
	TVector3<T>& operator=(const TVector4<U>& Other);

	template <typename U>
	TVector3<T>& operator=(const TVector3<U>& Other);

	template <typename U>
	/**
	* Performs vector-scalar subtraction.
	* @param Rhs - Value to subtract
	* @return Reference to this vector
	*/
	TVector3<T>& operator-=(const U Scalar);

	template <typename U>
	/**
	* Performs vector-scalar addition.
	* @param Rhs - Value to subtract
	* @return Reference to this vector
	*/
	TVector3<T>& operator+=(const U Scalar);

	template <typename U>
	/**
	* Performs vector-scalar multiplication.
	* @param scalar - Scalar to multiply by.
	* @return Reference to this vector.
	*/
	TVector3<T>& operator*=(const U Scalar);

	template <typename U>
	/**
	* Performs vector division.
	* @param rhs - Unit to divide by.
	* @return Reference to this vector
	*/
	TVector3<T>& operator/=(const U Scalar);

	/**
	* Performs componentwise multiplication.
	* @param Rhs - Vector to multiply by.
	* @return Reference to this vector.
	*/
	TVector3<T>& operator*=(const TVector3<T>& Rhs);

	/**
	* Performs componentwise division.
	* @param Rhs - Vector to multiply by.
	* @return Reference to this vector.
	*/
	TVector3<T>& operator/=(const TVector3<T>& Rhs);

	/**
	* Performs vector addition.
	* @param Rhs - TVector3 to add.
	* @return Reference to this vector.
	*/
	TVector3<T>& operator+=(const TVector3<T>& Rhs);

	/**
	* Performs vector subtraction.
	* @param Rhs - TVector3 to subtract
	* @return Reference to this vector
	*/
	TVector3<T>& operator-=(const TVector3<T>& Rhs);

	/**
	* Checks vector equality.
	*/
	bool operator==(const TVector3<T>& Rhs) const;

	/**
	* Checks vector inequality.
	*/
	bool operator!=(const TVector3<T>& Rhs) const;

	/**
	* Overload of subscript operators for X = 0, Y = 1, Z = 2.
	*/
	T& operator[](uint32_t idx);

	/**
	* Overload of subscript operators for X = 0, Y = 1, Z = 2.
	*/
	const T& operator[](uint32_t idx) const;

	/**
	* Normalizes the vector.
	*/
	TVector3<T>& Normalize();

	/**
	* Calculates the length/magnatude of the vector.
	* @return The length/magnatude
	*/
	float Length() const;

	/**
	* Calculates the length squared of the vector.
	* When an absolute length is not need, such as
	* when vector-vector comparisions, use this instead of
	* Lenght() to aviod squareroot calculation. 
	*/
	float LengthSquared() const;

	/**
	* Reflects the vector across a normal.
	* @return Reflected vector
	*/
	TVector3<T> Reflect(const TVector3<T>& Normal) const;

	/**
	* Projects this vector onto the axis of another vector. This does not
	* assume that Vector is normalized, use ProjectedOnNormal
	* if Vector is already normalized.
	* @param Vector to project upon
	* @return The projected vector.
	*/
	TVector3<T> ProjectedOnVector(const TVector3<T>& Vector) const;

	/**
	* Projects this vector onto the axis of normalized vector. This assumes 
	* that Vector is normalized, use ProjectedOnVector if Vector
	* is not already normalized.
	* @param Vector to project upon
	* @return The projected vector.
	*/
	TVector3<T> ProjectedOnNormal(const TVector3<T>& Normal) const;

	/* Returns string representation of the vector */
	const std::string ToString() const;

	/**
	* Calculates the dot product of two vectors.
	* @param lhs - Left operand
	* @param rhs - Right operand
	* @return
	*/
	static T Dot(const TVector3<T>& lhs, const TVector3<T>& rhs);

	/**
	* Calculates the cross product of two vectors.
	* @param lhs - Left operand
	* @param rhs - Right operand
	* @return The cross product vector.
	*/
	static TVector3<T> Cross(const TVector3<T>& lhs, const TVector3<T>& rhs);

	/**
	* Checks if two directions are perpendicular, or orthogonal, to one another,
	* within a certain threshold. The normals given are assumed to be unit length.
	* @param Normal1 Direction to check
	* @param Normal2 Direction to check
	* @param CosineThreshold Threshold to test against
	* @return True if the cosine of the angle between the direction is within the threshold
	*/
	static bool Perpendicular(const TVector3<T>& Normal1, const TVector3<T>& Normal2, const float CosineThreshold);

	/**
	* Checks if two directions are parallel, to one another.  The normals given are assumed to be unit length.
	* This test ignores checking if the directions point in this same direction,
	* it checks if the angle between them is within a certain threshold.
	* @param Normal1 Direction to check
	* @param Normal2 Direction to check
	* @param CosineThreshold Threshold to test against
	* @return True if the cosine of the angle between the direction is within the threshold
	*/
	static bool Parallel(const TVector3<T>& Normal1, const TVector3<T>& Normal2, const float CosineThreshold);

public:
	T x; /* X coordinate of the vector */
	T y; /* Y coordinate of the vector */
	T z; /* Z coordinate of the vector */
};

using Vector3i = TVector3<int32_t>;	/* Vector type for 32 bit integers */
using Vector3ui = TVector3<uint32_t>; /* Vector type for 32 bit unsigned integers */
using Vector3f = TVector3<float>; /* Vector type for floats */

#include "Vector4.h"