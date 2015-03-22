#pragma once

#include <cstdint>
#include <cstdio>

#include "Misc\Assertions.h"

template <typename T>
/**
* Class of manipulating a 2D vector.
*/
class TVector2
{
public:
	/**
	* Constructs vector with 2 components.
	* @param X - value of X component
	* @param Y - value of Y component
	*/
	explicit TVector2(T X = 0, T Y = 0);

	/**
	* Default copy constructor.
	*/
	TVector2(const TVector2<T>& Other) = default;

	/**
	* Default copy assignment.
	*/
	TVector2<T>& operator=(const TVector2<T>& Other) = default;

	template <typename U>
	TVector2<T>(const TVector2<U>& Other);

	template <typename U>
	TVector2<T>& operator=(const TVector2<U>& Other);

	template <typename U>
	/**
	* Performs vector-scalar multiplication.
	* @param Scalar - Scalar to multiply by.
	* @return Reference to this vector.
	*/
	TVector2<T>& operator*=(const U& Scalar);

	template <typename U>
	/**
	* Performs vector-scalar addition.
	* @param rhs - Vector2 to add.
	* @return Reference to this vector.
	*/
	TVector2<T>& operator+=(const U& Scalar);

	template <typename U>
	/**
	* Performs vector-scalar subtraction.
	* @param Scalar - Unit to divide by.
	* @return Reference to this vector
	*/
	TVector2<T>& operator-=(const U& Scalar);

	template <typename U>
	/**
	* Performs vector-scalar division.
	* @param Scalar - Unit to divide by.
	* @return Reference to this vector
	*/
	TVector2<T>& operator/=(const U& Scalar);

	/**
	* Performs vector addition.
	* @param rhs - Vector2 to add.
	* @return Reference to this vector.
	*/
	TVector2<T>& operator+=(const TVector2<T>& rhs);

	/**
	* Performs vector subtraction.
	* @param rhs - Vector2 to subtract
	* @return Reference to this vector
	*/
	TVector2<T>& operator-=(const TVector2<T>& rhs);

	/**
	* Performs componentwise multiplication.
	* @param Rhs - Vector to multiply by.
	* @return Reference to this vector.
	*/
	TVector2<T>& operator*=(const TVector2<T>& Rhs);

	/**
	* Checks vector equality.
	*/
	bool operator==(const TVector2<T>& Rhs) const;

	/**
	* Checks vector inequality.
	*/
	bool operator!=(const TVector2<T>& Rhs) const;

	/**
	* Overload of subscript operators for X = 0, Y = 1.
	*/
	T& operator[](uint32_t idx);

	/**
	* Overload of subscript operators for X = 0, Y = 1.
	*/
	const T& operator[](uint32_t idx) const;

	/**
	* Normalizes the vector.
	*/
	void Normalize();

	/**
	* Calculates the length/magnatude of the vector.
	* @return The length/magnatude
	*/
	float Length();

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
	TVector2<T> Reflect(const TVector2<T>& Normal) const;

	/**
	* Projects this vector onto the axis of another vector. This does not
	* assume that Vector is normalized, use ProjectedOnNormal
	* if Vector is already normalized.
	* @param Vector to project upon
	* @return The projected vector.
	*/
	TVector2<T> ProjectedOnVector(const TVector2<T>& Vector) const;

	/**
	* Projects this vector onto the axis of normalized vector. This assumes
	* that Vector is normalized, use ProjectedOnVector if Vector
	* is not already normalized.
	* @param Vector to project upon
	* @return The projected vector.
	*/
	TVector2<T> ProjectedOnNormal(const TVector2<T>& Normal) const;

	/* Returns string representation of the vector */
	const std::string ToString() const;

	/**
	* Calculates the dot product of two vectors.
	* @param lhs - Left operand
	* @param rhs - Right operand
	* @return The dot product
	*/
	static T Dot(const TVector2<T>& lhs, const TVector2<T>& rhs);

	/**
	* Checks if two directions are perpendicular, or orthogonal, to one another,
	* within a certain threshold. The normals given are assumed to be unit length.
	* @param Normal1 Direction to check
	* @param Normal2 Direction to check
	* @param CosineThreshold Threshold to test against
	* @return True if the cosine of the angle between the direction is within the threshold
	*/
	static bool Perpendicular(const TVector2<T>& Normal1, const TVector2<T>& Normal2, const float CosineThreshold);

	/**
	* Checks if two directions are parallel, to one another.  The normals given are assumed to be unit length.
	* This test ignores checking if the directions point in this same direction,
	* it checks if the angle between them is within a certain threshold.
	* @param Normal1 Direction to check
	* @param Normal2 Direction to check
	* @param CosineThreshold Threshold to test against
	* @return True if the cosine of the angle between the direction is within the threshold
	*/
	static bool Parallel(const TVector2<T>& Normal1, const TVector2<T>& Normal2, const float CosineThreshold);

public:
	T x; /* X coordinate of the vector */
	T y; /* Y coordinate of the vector */
};

using Vector2i = TVector2<int32_t>;		/* Vector type for 32-bit integers */
using Vector2ui = TVector2<uint32_t>;	/* Vector type for unsigned 32-bit integers */
using Vector2f = TVector2<float>;		/* Vector type for floats */

/////////////////////////////////////////////////////
//////////// Inlined Member Functions ///////////////
/////////////////////////////////////////////////////

template <typename T>
inline TVector2<T>::TVector2(T X = 0, T Y = 0)
	: x(X), y(Y) {}

template <typename T>
template <typename U>
inline TVector2<T>::TVector2(const TVector2<U>& Other)
	: x((T)Other.x)
	, y((T)Other.y)
{
}

template <typename T>
template <typename U>
inline TVector2<T>& TVector2<T>::operator=(const TVector2<U>& Other)
{
	x = (T)Other.x;
	y = (T)Other.y;
	return *this;
}

template <typename T>
template <typename U>
inline TVector2<T>& TVector2<T>::operator*=(const U& Scalar)
{
	x *= Scalar;
	y *= Scalar;

	return *this;
}


template <typename T>
template <typename U>
TVector2<T>& TVector2<T>::operator/=(const U& Scalar)
{
	x /= Scalar;
	y /= Scalar;

	return *this;
}


template <typename T>
inline TVector2<T>& TVector2<T>::operator+=(const TVector2<T>& rhs)
{
	x += rhs.x;
	y += rhs.y;

	return *this;
}

template <typename T>
template <typename U>
inline TVector2<T>& TVector2<T>::operator+=(const U& Scalar)
{
	x += Scalar;
	y += Scalar;

	return *this;
}

template <typename T>
template <typename U>
TVector2<T>& TVector2<T>::operator-=(const U& Scalar)
{
	x -= Scalar;
	y -= Scalar;

	return *this;
}

template <typename T>
inline TVector2<T>& TVector2<T>::operator-=(const TVector2<T>& rhs)
{
	x -= rhs.x;
	y -= rhs.y;

	return *this;
}

template <typename T>
TVector2<T>& TVector2<T>::operator*=(const TVector2<T>& Rhs)
{
	x *= rhs.x;
	y *= rhs.y;

	return *this;
}

template <typename T>
inline bool TVector2<T>::operator==(const TVector2<T>& Rhs) const
{
	for (size_t i = 0; i < 2; i++)
		if (*this[i] != Rhs[i])
			return false;

	return true;
}

template <typename T>
inline bool TVector2<T>::operator!=(const TVector2<T>& Rhs) const
{
	for (size_t i = 0; i < 2; i++)
		if ((*this)[i] != Rhs[i])
			return true;

	return false;
}

template <typename T>
T& TVector2<T>::operator[](uint32_t idx)
{
	ASSERT(idx >= 0 && idx < 2 && "Vector2 index out of range.");
	if (idx == 0)
		return x;
	else
		return y;
}


template <typename T>
const T& TVector2<T>::operator[](uint32_t idx) const
{
	ASSERT(idx >= 0 && idx < 2 && "Vector2 index out of range.");
	if (idx == 0)
		return x;
	else
		return y;
}


template <typename T>
inline void TVector2<T>::Normalize()
{
	*this /= Length();
}


template <typename T>
inline float TVector2<T>::Length()
{
	return std::sqrt(x*x + y*y);
}

template <typename T>
inline float TVector2<T>::LengthSquared() const
{
	return (x*x + y*y);
}

template <typename T>
inline TVector2<T> TVector2<T>::ProjectedOnVector(const TVector2<T>& Vector) const
{
	return Vector * (TVector2<T>::Dot(*this, Vector) / Vector.LengthSquared());
}

template <typename T>
inline TVector2<T> TVector2<T>::ProjectedOnNormal(const TVector2<T>& Normal) const
{
	return Normal * TVector2<T>::Dot(*this, Normal);
}

template <typename T>
inline const std::string TVector2<T>::ToString() const
{
	char buffer[100];
	int32_t n;
	if (typeid(T) == typeid(float))
		n = sprintf_s(buffer, "TVector2: %.3f %.3f", x, y);
	else
		n = sprintf_s(buffer, "TVector2: %d %d", x, y);
	ASSERT(n >= 0 && "sprintf failed to write string.");
	return std::string(buffer);
}

template <typename T>
inline T Dot(const TVector2<T>& lhs, const TVector2<T>& rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y;
}

template <typename T>
inline bool TVector2<T>::Perpendicular(const TVector2<T>& Normal1, const TVector2<T>& Normal2, const float CosineThreshold)
{
	const float Dot = TVector2<T>::Dot(Normal1, Normal2);
	return std::abs(Dot) <= CosineThreshold;
}

template <typename T>
inline bool TVector2<T>::Parallel(const TVector2<T>& Normal1, const TVector2<T>& Normal2, const float CosineThreshold)
{
	const float Dot = TVector2<T>::Dot(Normal1, Normal2);
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
inline TVector2<T> operator+(const TVector2<T>& lhs, const TVector2<T>& rhs)
{
	return TVector2<T>(lhs.x + rhs.x, lhs.y + rhs.y);
}

template <typename T>
/**
* Calculates the difference of two vectors.
* @param lhs - Left operand
* @param rhs - Right operand
* @return Difference of the two vectors.
*/
inline TVector2<T> operator-(const TVector2<T>& lhs, const TVector2<T>& rhs)
{
	return TVector2<T>(lhs.x - rhs.x, lhs.y - rhs.y);
}

template <typename T>
/**
* Negates the vector components.
* @param lhs - Left operand
* @return The negated vector.
*/
inline TVector2<T> operator-(const TVector2<T>& lhs)
{
	return TVector2<T>(-lhs.x, -lhs.y);
}

template <typename T, typename U>
/**
* Performs vector-scalar addition.
* @param Vec - Left operand (vector)
* @param Scalar - Right operand (scalar)
* @return Memberwise multiplied vector
*/
inline TVector2<T> operator+(const TVector2<T>& Vec, const U& Scalar)
{
	return TVector2<T>(Vec.x + Scalar, Vec.y + Scalar);
}

template <typename T, typename U>
/**
* Performs vector-scalar subtraction.
* @param Vec - Left operand (vector)
* @param Scalar - Right operand (scalar)
* @return Memberwise multiplied vector
*/
inline TVector2<T> operator-(const TVector2<T>& Vec, const U& Scalar)
{
	return TVector2<T>(Vec.x - Scalar, Vec.y - Scalar);
}

template <typename T, typename U>
/**
* Performs vector-scalar multiplication.
* @param Vec - Left operand (vector)
* @param Scalar - Right operand (scalar)
* @return Memberwise multiplied vector
*/
inline TVector2<T> operator*(const TVector2<T>& Vec, const U& Scalar)
{
	return TVector2<T>(Vec.x * Scalar, Vec.y * Scalar);
}

template <typename T, typename U>
/**
* Performs vector-scalar multiplication of two vectors.
* @param Scalar - Left operand (scalar)
* @param vector - Right operand (vector)
* @return Memberwise multiplied vector
*/
inline TVector2<T> operator*(const U& Scalar, const TVector2<T>& Vec)
{
	return Vec * Scalar;
}

template <typename T, typename U>
/**
* Performs vector division.
* @param Vec - Vector to divide.
* @param Scalar - Unit to divide by.
* @return Memberwise divided vector
*/
inline TVector2<T> operator/(const TVector2<T>& Vec, const U& Scalar)
{
	return TVector2<T>(Vec.x / Scalar, Vec.y / Scalar);
}