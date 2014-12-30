#pragma once

#include <cstdint>
#include <cstdio>

#include "Misc\Assertions.h"

template <typename T>
/**
* Class of manipulating a vector with 3 components.
*/
class TVector3
{
public:
	/**
	* Constructs vector with x, y, z coordinates.
	* @param X - position of X coordinate
	* @param Y - position of Y coordinate
	* @param Z - position of Z coordinate
	*/
	TVector3(T X = 0, T Y = 0, T Z = 0);

	/**
	* Default copy constructor.
	*/
	TVector3(const TVector3<T>& Other) = default;

	/**
	* Default copy assignment.
	*/
	TVector3<T>& operator=(const TVector3<T>& Other) = default;

	template <typename U>
	/**
	* Performs vector-scalar multiplication.
	* @param scalar - Scalar to multiply by.
	* @return Reference to this vector.
	*/
	TVector3<T>& operator*=(const U& Scalar);

	/**
	* Performs componentwise multiplication.
	* @param Rhs - Vector to multiply by.
	* @return Reference to this vector.
	*/
	TVector3<T>& operator*=(const TVector3<T>& Rhs);

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

	template <typename U>
	/**
	* Performs vector division.
	* @param rhs - Unit to divide by.
	* @return Reference to this vector
	*/
	TVector3<T>& operator/=(const U& Scalar);

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

using Vector3i = TVector3<int32_t>;	/* Vector type for integers */
using Vector3f = TVector3<float>; /* Vector type for floats */


/////////////////////////////////////////////////////
//////////// Inlined Member Functions ///////////////
/////////////////////////////////////////////////////


template <typename T>
inline TVector3<T>::TVector3(T X, T Y, T Z)
	: x(X), y(Y), z(Z) {}


template <typename T>
template <typename U>
inline TVector3<T>& TVector3<T>::operator*=(const U& Scalar)
{
	x *= Scalar;
	y *= Scalar;
	z *= Scalar;

	return *this;
}

template <typename T>
TVector3<T>& TVector3<T>::operator*=(const TVector3<T>& Rhs)
{
	x *= Rhs.x;
	y *= Rhs.y;
	z *= Rhs.z;

	return *this;
}

template <typename T>
template <typename U>
TVector3<T>& TVector3<T>::operator/=(const U& Scalar)
{
	x /= Scalar;
	y /= Scalar;
	z /= Scalar;

	return *this;
}

template <typename T>
inline TVector3<T>& TVector3<T>::operator+=(const TVector3<T>& rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;

	return *this;
}

template <typename T>
inline TVector3<T>& TVector3<T>::operator-=(const TVector3<T>& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;

	return *this;
}

template <typename T>
inline bool TVector3<T>::operator==(const TVector3<T>& Rhs) const
{
	for (size_t i = 0; i < 3; i++)
		if ((*this)[i] != Rhs[i])
			return false;

	return true;
}

template <typename T>
inline bool TVector3<T>::operator!=(const TVector3<T>& Rhs) const
{
	for (size_t i = 0; i < 3; i++)
		if ((*this)[i] != Rhs[i])
			return true;

	return false;
}

template <typename T>
inline T& TVector3<T>::operator[](uint32_t idx)
{
	ASSERT(idx >= 0 && idx < 3 && "Vector3 index out of range.");
	if (idx == 0)
		return x;
	else if (idx == 1)
		return y;
	else
		return z;
}

template <typename T>
inline const T& TVector3<T>::operator[](uint32_t idx) const
{
	ASSERT(idx >= 0 && idx < 3 && "Vector3 index out of range.");
	if (idx == 0)
		return x;
	else if (idx == 1)
		return y;
	else
		return z;
}

template <typename T>
inline TVector3<T>& TVector3<T>::Normalize()
{
	return *this /= Length();
}


template <typename T>
inline float TVector3<T>::Length() const
{
	return std::sqrt(x*x + y*y + z*z);
}

template <typename T>
inline float TVector3<T>::LengthSquared() const
{
	return (x*x + y*y + z*z);
}

template <typename T>
inline TVector3<T> TVector3<T>::Reflect(const TVector3<T>& Normal) const
{
	return 2 * Dot(*this, Normal) * Normal - *this;
}

template <typename T>
inline TVector3<T> TVector3<T>::ProjectedOnVector(const TVector3<T>& Vector) const
{
	return Vector * (TVector3<T>::Dot(*this, Vector) / Vector.LengthSquared());
}

template <typename T>
inline TVector3<T> TVector3<T>::ProjectedOnNormal(const TVector3<T>& Normal) const
{
	return Normal * TVector3<T>::Dot(*this, Normal);
}

template <typename T>
inline const std::string TVector3<T>::ToString() const
{
	char buffer[100];
	int32_t n;
	if (typeid(T) == typeid(float))
		n = sprintf_s(buffer, "TVector3: %.3f %.3f %.3f", x, y, z);
	else
		n = sprintf_s(buffer, "TVector3: %d %d %d", x, y, z);
	ASSERT(n >= 0 && "sprintf failed to write string.");
	return std::string(buffer);
}

template <typename T>
inline T TVector3<T>::Dot(const TVector3<T>& lhs, const TVector3<T>& rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

template <typename T>
inline TVector3<T> TVector3<T>::Cross(const TVector3<T>& lhs, const TVector3<T>& rhs)
{
	return TVector3<T>(	(lhs.y * rhs.z) - (lhs.z * rhs.y),
						(lhs.z * rhs.x) - (lhs.x * rhs.z),
						(lhs.x * rhs.y) - (lhs.y * rhs.x));
}

template <typename T>
inline bool TVector3<T>::Perpendicular(const TVector3<T>& Normal1, const TVector3<T>& Normal2, const float CosineThreshold)
{
	const float Dot = TVector3<T>::Dot(Normal1, Normal2);
	return std::abs(Dot) <= CosineThreshold;
}

template <typename T>
inline bool TVector3<T>::Parallel(const TVector3<T>& Normal1, const TVector3<T>& Normal2, const float CosineThreshold)
{
	const float Dot = TVector3<T>::Dot(Normal1, Normal2);
	return std::abs(Dot) >= CosineThreshold;
}

/////////////////////////////////////////////////////////////////////
//////////////// Non-member Functions ///////////////////////////////
/////////////////////////////////////////////////////////////////////

template <typename T>
/**
* Adds two vectors.
* @param lhs - Left operand
* @param rhs - Right operand
* @return Addition of the two vectors.
*/
inline TVector3<T> operator+(const TVector3<T>& lhs, const TVector3<T>& rhs)
{
	return TVector3<T>(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

template <typename T>
/**
* Calculates the difference of two vectors.
* @param lhs - Left operand
* @param rhs - Right operand
* @return Difference of the two vectors.
*/
inline TVector3<T> operator-(const TVector3<T>& lhs, const TVector3<T>& rhs)
{
	return TVector3<T>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

template <typename T>
/**
* Negates the vector components.
* @param lhs - Left operand
* @return The negated vector.
*/
inline TVector3<T> operator-(const TVector3<T>& lhs)
{
	return TVector3<T>(-lhs.x, -lhs.y, -lhs.z);
}

template <typename T, typename U>
/**
* Performs vector-scalar multiplication of two vectors.
* @param Vec - Left operand (vector)
* @param Scalar - Right operand (scalar)
* @return Memberwise multiplied vector
*/
inline TVector3<T> operator*(const TVector3<T>& Vec, const U& Scalar)
{
	return TVector3<T>(Vec.x * Scalar, Vec.y * Scalar, Vec.z * Scalar);
}

template <typename T, typename U>
/**
* Performs vector-scalar multiplication of two vectors.
* @param scalar - Left operand (scalar)
* @param vector - Right operand (vector)
* @return Memberwise multiplied vector
*/
inline TVector3<T> operator*(const U& Scalar, const TVector3<T>& Vector)
{
	return Vector * Scalar;
}

template <typename T>
/**
* Performs componentwise multiplication.
* @return The multiplied vector result.
*/
TVector3<T> operator*(TVector3<T> Lhs, const TVector3<T>& Rhs)
{
	Lhs *= Rhs;
	return Lhs;
}

template <typename T, typename U>
/**
* Performs vector division.
* @param Vec - Vector to divide.
* @param Scalar - Unit to divide by.
* @return Memberwise divided vector
*/
inline TVector3<T> operator/(const TVector3<T>& Vec, const U& Scalar)
{
	return TVector3<T>(Vec.x / Scalar, Vec.y / Scalar, Vec.z / Scalar);
}