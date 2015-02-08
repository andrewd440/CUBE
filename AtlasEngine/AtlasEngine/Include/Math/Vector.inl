#pragma once

#include "SystemMath.h"
#include "Misc\Assertions.h"

/////////////////////////////////////////////////////
////////// TVector4 inline implementation ///////////
/////////////////////////////////////////////////////

template <typename T>
inline TVector4<T>::TVector4(const T X, const T Y, const T Z, const T W)
	: x(X), y(Y), z(Z), w(W){}

template <typename T>
inline TVector4<T>::TVector4(const TVector3<T>& Vec3, const T W)
	: TVector4<T>(Vec3.x, Vec3.y, Vec3.z, W)
{
}

template <typename T>
template <typename U>
inline TVector4<T>& TVector4<T>::operator+=(const U Scalar)
{
	TVector4<T> ScalarField{ Scalar, Scalar, Scalar, Scalar };
	AddVectorVector(*this, ScalarField, *this);
	return *this;
}

template <typename T>
template <typename U>
inline TVector4<T>& TVector4<T>::operator-=(const U Scalar)
{
	TVector4<T> ScalarField{ Scalar, Scalar, Scalar, Scalar };
	SubtractVectorVector(*this, ScalarField, *this);
	return *this;
}

template <typename T>
template <typename U>
inline TVector4<T>& TVector4<T>::operator*=(const U Scalar)
{
	TVector4<T> ScalarField{ Scalar, Scalar, Scalar, Scalar };
	MultVectorVector(*this, ScalarField, *this);
	return *this;
}

template <typename T>
template <typename U>
inline TVector4<T>& TVector4<T>::operator/=(const U Scalar)
{
	TVector4<T> ScalarField{ Scalar, Scalar, Scalar, Scalar };
	DivideVectorVector(*this, ScalarField, *this);
	return *this;
}

template <typename T>
inline TVector4<T>& TVector4<T>::operator+=(const TVector4<T>& rhs)
{
	AddVectorVector(*this, rhs, *this);
	return *this;
}

template <typename T>
inline TVector4<T>& TVector4<T>::operator-=(const TVector4<T>& rhs)
{
	SubtractVectorVector(*this, rhs, *this);
	return *this;
}

template <typename T>
inline TVector4<T>& TVector4<T>::operator*=(const TVector4<T>& Rhs)
{
	MultVectorVector(*this, Rhs, *this);
	return *this;
}

template <typename T>
inline TVector4<T>& TVector4<T>::operator/=(const TVector4<T>& Rhs)
{
	DivideVectorVector(*this, Rhs, *this);
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
	const float OldW = w;
	*this *= invLength;
	w = OldW;
	return *this;
}

template <typename T>
inline float TVector4<T>::Length4() const
{
	return sqrt(x*x + y*y + z*z + w*w);
}

template <typename T>
inline float TVector4<T>::Length3() const
{
	return sqrt(x*x + y*y + z*z);
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
	return 2.0f * TVector4<T>::Dot3(*this, Normal) * Normal - *this;
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
	float Result;
	auto L = lhs;
	auto R = rhs;
	L.z = R.z = 0;
	Dot4Product(L, R, Result);
	return Result;
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
	float Result;
	Dot4Product(lhs, rhs, Result);
	return Result;
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
	return TVector4<T>(lhs.y * rhs.z - lhs.z * rhs.y,
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
	TVector4<T> Result;
	AddVectorVector(lhs, rhs, Result);
	return Result;
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
	TVector4<T> Result;
	SubtractVectorVector(lhs, rhs, Result);
	return Result;
}

template <typename T>
/**
* Component-wise multiplication of two vectors.
* @param lhs - Left operand
* @param rhs - Right operand
*/
inline TVector4<T> operator*(const TVector4<T>& lhs, const TVector4<T>& rhs)
{
	TVector4<T> Result;
	MultVectorVector(lhs, rhs, Result);
	return Result;
}

template <typename T>
/**
* Component-wise division of two vectors.
* @param lhs - Left operand
* @param rhs - Right operand
*/
inline TVector4<T> operator/(const TVector4<T>& lhs, const TVector4<T>& rhs)
{
	TVector4<T> Result;
	DivideVectorVector(lhs, rhs, Result);
	return Result;
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
inline TVector4<T> operator+(const TVector4<T>& Vec, const U Scalar)
{
	TVector4<T> Result;
	TVector4<T> ScalarField{ Scalar, Scalar, Scalar, Scalar };
	AddVectorVector(Vec, ScalarField, Result);
	return Result;
}

template <typename T, typename U>
/**
* Performs vector-scalar multiplication.
* @param Vec - Left operand (vector)
* @param Scalar - Right operand (scalar)
* @return Memberwise multiplied vector
*/
inline TVector4<T> operator+(const U Scalar, const TVector4<T>& Vec)
{
	return Vec + Scalar;
}

template <typename T, typename U>
/**
* Performs vector-scalar subtraction.
* @param Vec - Left operand (vector)
* @param Scalar - Right operand (scalar)
* @return Memberwise multiplied vector
*/
inline TVector4<T> operator-(const TVector4<T>& Vec, const U Scalar)
{
	TVector4<T> Result;
	TVector4<T> ScalarField{ Scalar, Scalar, Scalar, Scalar };
	SubtractVectorVector(Vec, ScalarField, Result);
	return Result;
}

template <typename T, typename U>
/**
* Performs vector-scalar multiplication.
* @param Vec - Left operand (vector)
* @param Scalar - Right operand (scalar)
* @return Memberwise multiplied vector
*/
inline TVector4<T> operator*(const TVector4<T>& Vec, const U Scalar)
{
	TVector4<T> Result;
	TVector4<T> ScalarField{ Scalar, Scalar, Scalar, Scalar };
	MultVectorVector(Vec, ScalarField, Result);
	return Result;
}

template <typename T, typename U>
/**
* Performs vector-scalar multiplication.
* @param scalar - Left operand (scalar)
* @param vector - Right operand (vector)
* @return Memberwise multiplied vector
*/
inline TVector4<T> operator*(const U Scalar, const TVector4<T>& Vec)
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
inline TVector4<T> operator/(const TVector4<T>& Vec, const U Scalar)
{
	TVector4<T> Result;
	TVector4<T> ScalarField{ Scalar, Scalar, Scalar, Scalar };
	DivideVectorVector(Vec, ScalarField, Result);
	return Result;
}


/////////////////////////////////////////////////////
////////// TVector3 inline implementation ///////////
/////////////////////////////////////////////////////

template <typename T>
const TVector3<T> TVector3<T>::Forward = TVector3<T>{0, 0, -1};

template <typename T>
const TVector3<T> TVector3<T>::Backward = TVector3<T>{0, 0, 1};

template <typename T>
const TVector3<T> TVector3<T>::Left = TVector3<T>{-1, 0, 0};

template <typename T>
const TVector3<T> TVector3<T>::Right = TVector3<T>{1, 0, 0};

template <typename T>
const TVector3<T> TVector3<T>::Up = TVector3<T>{0, 1, 0};

template <typename T>
const TVector3<T> TVector3<T>::Down = TVector3<T>{0, -1, 0};

template <typename T>
inline T TVector3<T>::Dot(const TVector3<T>& lhs, const TVector3<T>& rhs)
{
	// forward to vec4 with sse
	TVector4<T> L{ lhs, 0 };
	TVector4<T> R{ rhs, 0 };
	return 	TVector4<T>::Dot4(L, R);
}

template <typename T>
template <typename U>
inline TVector3<T>& TVector3<T>::operator*=(const U Scalar)
{
	// forward to vec4 with sse
	TVector4<T> This{ *this, 0 };
	This *= Scalar;

	// assign results
	x = This.x;
	y = This.y;
	z = This.z;

	return *this;
}

template <typename T>
inline TVector3<T>& TVector3<T>::operator*=(const TVector3<T>& Rhs)
{
	// forward to vec4 with sse
	TVector4<T> This{ *this, 0 };
	TVector4<T> Other{ Rhs, 0 };
	This *= Other;

	// assign results
	x = This.x;
	y = This.y;
	z = This.z;

	return *this;
}

template <typename T>
inline TVector3<T>& TVector3<T>::operator/=(const TVector3<T>& Rhs)
{
	// forward to vec4 with sse
	TVector4<T> This{ *this, 1 };
	TVector4<T> Other{ Rhs, 1 };
	This /= Other;

	// assign results
	x = This.x;
	y = This.y;
	z = This.z;

	return *this;
}

template <typename T>
template <typename U>
inline TVector3<T>& TVector3<T>::operator/=(const U Scalar)
{
	// forward to vec4 with sse
	TVector4<T> This{ *this, 0 };
	This /= Scalar;

	// assign results
	x = This.x;
	y = This.y;
	z = This.z;

	return *this;
}

template <typename T>
inline TVector3<T>& TVector3<T>::operator+=(const TVector3<T>& Rhs)
{
	// forward to vec4 with sse
	TVector4<T> This{ *this, 0 };
	TVector4<T> Other{ Rhs, 0 };
	This += Other;

	// assign results
	x = This.x;
	y = This.y;
	z = This.z;

	return *this;
}

template <typename T>
inline TVector3<T>& TVector3<T>::operator-=(const TVector3<T>& Rhs)
{
	// forward to vec4 with sse
	TVector4<T> This{ *this, 0 };
	TVector4<T> Other{ Rhs, 0 };
	This -= Other;

	// assign results
	x = This.x;
	y = This.y;
	z = This.z;

	return *this;
}

template <typename T>
template <typename U>
inline TVector3<T>& TVector3<T>::operator-=(const U Scalar)
{
	// forward to vec4 with sse
	TVector4<T> This{ *this, 0 };
	This -= Scalar;

	// assign results
	x = This.x;
	y = This.y;
	z = This.z;

	return *this;
}

template <typename T>
template <typename U>
TVector3<T>& TVector3<T>::operator+=(const U Scalar)
{
	// forward to vec4 with sse
	TVector4<T> This{ *this, 0 };
	This += Scalar;

	// assign results
	x = This.x;
	y = This.y;
	z = This.z;

	return *this;
}


/////////////////////////////////////////////////////
//////////// Inlined Member Functions ///////////////
/////////////////////////////////////////////////////

template <typename T>
inline TVector3<T>::TVector3(T X, T Y, T Z)
	: x(X), y(Y), z(Z) {}


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
inline TVector3<T> TVector3<T>::Cross(const TVector3<T>& lhs, const TVector3<T>& rhs)
{
	return TVector3<T>((lhs.y * rhs.z) - (lhs.z * rhs.y),
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
inline TVector3<T> operator+(TVector3<T> lhs, const TVector3<T>& rhs)
{
	return lhs += rhs;
}

template <typename T>
/**
* Calculates the difference of two vectors.
* @param lhs - Left operand
* @param rhs - Right operand
* @return Difference of the two vectors.
*/
inline TVector3<T> operator-(TVector3<T> lhs, const TVector3<T>& rhs)
{
	lhs -= rhs;
	return lhs;
}

template <typename T>
/**
* Performs componentwise multiplication.
* @return The multiplied vector result.
*/
TVector3<T> operator*(TVector3<T> Lhs, const TVector3<T>& Rhs)
{
	return Lhs *= Rhs;;
}

template <typename T>
/**
* Performs componentwise division.
* @return The multiplied vector result.
*/
TVector3<T> operator/(TVector3<T> Lhs, const TVector3<T>& Rhs)
{
	return Lhs /= Rhs;;
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
* Performs vector-scalar subtraction.
* @param Lhs - Vector operand
* @param Rhs - Value to subtract
*/
inline TVector3<T> operator-(TVector3<T> Lhs, const U Scalar)
{
	return Lhs -= Scalar;
}

template <typename T, typename U>
/**
* Performs vector-scalar addition.
* @param Lhs - Vector operand
* @param Rhs - Value to subtract
*/
inline TVector3<T> operator+(TVector3<T> Lhs, const U Scalar)
{
	return Lhs += Scalar;
}

template <typename T, typename U>
/**
* Performs vector-scalar addition.
* @param Lhs - Vector operand
* @param Rhs - Value to subtract
*/
inline TVector3<T> operator+(const U Scalar, const TVector3<T>& Lhs)
{
	return Lhs + Scalar;
}

template <typename T, typename U>
/**
* Performs vector-scalar multiplication of two vectors.
* @param Vec - Left operand (vector)
* @param Scalar - Right operand (scalar)
* @return Memberwise multiplied vector
*/
inline TVector3<T> operator*(TVector3<T> Vec, const U Scalar)
{
	return Vec *= Scalar;
}

template <typename T, typename U>
/**
* Performs vector-scalar multiplication of two vectors.
* @param scalar - Left operand (scalar)
* @param vector - Right operand (vector)
* @return Memberwise multiplied vector
*/
inline TVector3<T> operator*(const U Scalar, TVector3<T> Vector)
{
	return Vector * Scalar;
}

template <typename T, typename U>
/**
* Performs vector component-wise division.
* @param Vec - Vector to divide.
* @param Scalar - Unit to divide by.
* @return Memberwise divided vector
*/
inline TVector3<T> operator/(TVector3<T> Vec, const U Scalar)
{
	return Vec /= Scalar;
}