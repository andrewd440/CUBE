#pragma once

#include <cstdint>
#include <cstdio>

#include "Math\Vector3.h"
#include "Misc\Assertions.h"
#include "FMath.h"

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
	TVector4(T X = 0, T Y = 0, T Z = 0, T W = 1);

	/**
	* Default copy constructor.
	*/
	TVector4(const TVector4<T>& Other) = default;

	/**
	* Default copy assignment.
	*/
	TVector4<T>& operator=(const TVector4<T>& Other) = default;

	/**
	* Constructs vector with first 3 components of
	* TVector3.
	* @param Vec3 Vector with x, y, z components.
	*/
	TVector4(const TVector3<T>& Vec3);

	template <typename U>
	/**
	* Performs vector-scalar multiplication.
	* @param scalar - Scalar to multiply by.
	* @return Reference to this vector.
	*/
	TVector4<T>& operator*=(const U& scalar);

	/**
	* Performs vector addition.
	* @param rhs - TVector4 to add.
	* @return Reference to this vector.
	*/
	TVector4<T>& operator+=(const TVector4<T>& rhs);

	/**
	* Performs vector subtraction.
	* @param rhs - TVector4 to subtract
	* @return Reference to this vector
	*/
	TVector4<T>& operator-=(const TVector4<T>& rhs);

	template <typename U>
	/**
	* Performs vector division.
	* @param rhs - Unit to divide by.
	* @return Reference to this vector
	*/
	TVector4<T>& operator/=(const U& scalar);

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

using Vector4i = TVector4<int32_t>;	/* Vector type for integers */
using Vector4f = TVector4<float>; /* Vector type for floats */


///////////////////////////////////////////////////////////////////////////
////////////////// Inlined Member Functions ///////////////////////////////
///////////////////////////////////////////////////////////////////////////

template <typename T>
TVector4<T>::TVector4(T X = 0.0f, T Y = 0.0f, T Z = 0.0f, T W = 1.0f)
	: x(X), y(Y), z(Z), w(W){}

template <typename T>
TVector4<T>::TVector4(const TVector3<T>& Vec3)
	: TVector4<T>(Vec3.x, Vec3.y, Vec3.z)
{
}

template <typename T>
template <typename U>
inline TVector4<T>& TVector4<T>::operator*=(const U& scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	w *= scalar;
	return *this;
}

template <typename T>
inline TVector4<T>& TVector4<T>::operator+=(const TVector4<T>& rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	w += rhs.w;
	return *this;
}

template <typename T>
inline TVector4<T>& TVector4<T>::operator-=(const TVector4<T>& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	w -= rhs.w;
	return *this;
}

template <typename T>
template <typename U>
inline TVector4<T>& TVector4<T>::operator/=(const U& scalar)
{
	x /= scalar;
	y /= scalar;
	z /= scalar;
	w /= scalar;
	return *this;
}

template <typename T>
inline bool TVector4<T>::operator==(const TVector4<T>& Rhs) const
{
	for (size_t i = 0; i < 4; i++)
		if (*this[i] != Rhs[i])
			return false;

	return true;
}

template <typename T>
inline bool TVector4<T>::operator!=(const TVector4<T>& Rhs) const
{
	for (size_t i = 0; i < 4; i++)
		if ((*this)[i] != Rhs[i])
			return true;

	return false;
}

template <typename T>
inline T& TVector4<T>::operator[](uint32_t idx)
{
	ASSERT(idx >= 0 && idx < 4 && "Vector4 index out of range.");
	if (idx == 0)
		return x;
	else if (idx == 1)
		return y;
	else if (idx == 2)
		return z;
	else
		return w;
}

template <typename T>
inline const T& TVector4<T>::operator[](uint32_t idx) const
{
	ASSERT(idx >= 0 && idx < 4 && "Vector4 index out of range.");
	if (idx == 0)
		return x;
	else if (idx == 1)
		return y;
	else if (idx == 2)
		return z;
	else
		return w;
}

template <typename T>
inline TVector4<T>& TVector4<T>::Normalize3()
{
	float invLength = 1.0f / Length3();
	return *this *= invLength;
}

template <typename T>
inline float TVector4<T>::Length4() const
{
	return sqrtf(x*x + y*y + z*z + w*w);
}

template <typename T>
inline float TVector4<T>::Length3() const
{
	return sqrtf(x*x + y*y + z*z);
}

template <typename T>
inline float TVector4<T>::LengthSquared4() const
{
	return (x*x + y*y + z*z + w*w);
}

template <typename T>
inline float TVector4<T>::LengthSquared3() const
{
	return (x*x + y*y + z*z);
}

template <typename T>
inline TVector4<T> TVector4<T>::Reflect3(const TVector4<T>& Normal) const
{
	return 2.0f * Dot3(*this, Normal) * Normal - *this;
}

template <typename T>
inline TVector4<T> TVector4<T>::ProjectedOnVector(const TVector4<T>& Vector) const
{
	return Vector * (TVector4<T>::Dot3(*this, Vector) / Vector.LengthSquared3());
}

template <typename T>
inline TVector4<T> TVector4<T>::ProjectedOnNormal(const TVector4<T>& Normal) const
{
	return Normal * TVector4<T>::Dot3(*this, Normal);
}

template <typename T>
inline bool TVector4<T>::Perpendicular(const TVector4<T>& Normal1, const TVector4<T>& Normal2, const float CosineThreshold)
{
	const float Dot = TVector4<T>::Dot3(Normal1, Normal2);
	return std::abs(Dot) <= CosineThreshold;
}

template <typename T>
inline bool TVector4<T>::Parallel(const TVector4<T>& Normal1, const TVector4<T>& Normal2, const float CosineThreshold)
{
	const float Dot = TVector4<T>::Dot3(Normal1, Normal2);
	return std::abs(Dot) >= CosineThreshold;
}

template <typename T>
inline const std::string TVector4<T>::ToString() const
{
	char buffer[100];
	int32_t n;
	if (typeid(T) == typeid(float))
		n = sprintf_s(buffer, "TVector4: %.3f %.3f %.3f %.3f", x, y, z, w);
	else
		n = sprintf_s(buffer, "TVector4: %d %d %d %d", x, y, z, w);
	ASSERT(n >= 0 && "sprintf failed to write string.");
	return std::string(buffer);
}

///////////////////////////////////////////////////////////////////////////
////////////////// Non-Member Functions ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////

template <typename T>
/**
* Calculates the dot product of two vectors with x, y, and z components.
* @param lhs - Left operand
* @param rhs - Right operand
* @return
*/
inline T TVector4<T>::Dot3(const TVector4<T>& lhs, const TVector4<T>& rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

template <typename T>
/**
* Calculates the dot product of two vectors with all components.
* @param lhs - Left operand
* @param rhs - Right operand
* @return
*/
inline T TVector4<T>::Dot4(const TVector4<T>& lhs, const TVector4<T>& rhs)
{
	return Dot3(lhs, rhs) + lhs.w * rhs.w;
}

template <typename T>
/**
* Calculates the cross product of two vectors with x, y, z components.
* @param lhs - Left operand
* @param rhs - Right operand
* @return The cross product vector.
*/
inline TVector4<T> TVector4<T>::Cross(const TVector4<T>& lhs, const TVector4<T>& rhs)
{
	return TVector4<T>(	lhs.y * rhs.z - lhs.z * rhs.y,
						lhs.z * rhs.x - lhs.x * rhs.z,
						lhs.x * rhs.y - lhs.y * rhs.x,
						0.0f);
}

template <typename T>
/**
* Adds two vectors.
* @param lhs - Left operand
* @param rhs - Right operand
* @return Addition of the two vectors.
*/
inline TVector4<T> operator+(const TVector4<T>& lhs, const TVector4<T>& rhs)
{
	return TVector4<T>(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
}

template <typename T>
/**
* Calculates the difference of two vectors.
* @param lhs - Left operand
* @param rhs - Right operand
* @return Difference of the two vectors.
*/
inline TVector4<T> operator-(const TVector4<T>& lhs, const TVector4<T>& rhs)
{
	return TVector4<T>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
}

template <typename T>
/**
* Negates the vector components.
* @param lhs - Left operand
* @return The negated vector.
*/
inline TVector4<T> operator-(const TVector4<T>& lhs)
{
	return TVector4<T>(-lhs.x, -lhs.y, -lhs.z, -lhs.w);
}

template <typename T, typename U>
/**
* Performs vector-scalar multiplication of two vectors.
* @param Vec - Left operand (vector)
* @param Scalar - Right operand (scalar)
* @return Memberwise multiplied vector
*/
inline TVector4<T> operator*(const TVector4<T>& Vec, const U& Scalar)
{
	return TVector4<T>(Vec.x * Scalar, Vec.y * Scalar, Vec.z * Scalar, Vec.w * Scalar);
}

template <typename T, typename U>
/**
* Performs vector-scalar multiplication of two vectors.
* @param scalar - Left operand (scalar)
* @param vector - Right operand (vector)
* @return Memberwise multiplied vector
*/
inline TVector4<T> operator*(const U& scalar, const TVector4<T>& vector)
{
	return vector * scalar;
}

template <typename T, typename U>
/**
* Performs vector division.
* @param Vec - Vector to divide.
* @param Scalar - Unit to divide by.
* @return Memberwise divided vector
*/
inline TVector4<T> operator/(const TVector4<T>& Vec, const U& Scalar)
{
	return TVector4<T>(Vec.x / Scalar, Vec.y / Scalar, Vec.z / Scalar, Vec.w / Scalar);
}