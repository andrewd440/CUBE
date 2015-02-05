#pragma once

#include "..\Common.h"
#include "Matrix4.h"
#include "Vector3.h"
#include "SystemMath.h"

/** Represents a unit length 3D rotation quaternion */
WIN_ALIGN(16)
class FQuaternion
{
public:
	/**
	* Default ctor.
	* Constructs identity quaternion.
	*/
	FQuaternion();
	
	/**
	* Constructs quaternion with specified w,
	* x, y, z components.
	*/
	FQuaternion(const float w, const float x, const float y, const float z);

	/**
	* Constructs a quaternion with a rotation around an arbitrary axis
	* of a specified degrees.
	* @param Axis of rotation.
	* @oaram Degrees of rotation.
	*/
	FQuaternion(const Vector3f& Axis, const float Degrees);

	/**
	* Constructs a quaternion from euler angles.
	* @param X - Rotation, in degrees, around the x axis.
	* @param Y - Rotation, in degrees, around the y axis.
	* @param Z - Rotation, in degrees, around the z axis.
	*/
	FQuaternion(const float X, const float Y, const float Z);

	/**
	* Default copy ctor 
	*/
	FQuaternion(const FQuaternion& Other) = default;

	/** 
	* Default copy assignment 
	*/
	FQuaternion& operator=(const FQuaternion& Other) = default;
	
	/**
	* Default destructor 
	*/
	~FQuaternion() = default;

	/**
	* Quaternion multiplication assignment.
	* @param Other Quaternion to concat.
	*/
	FQuaternion& operator*=(const FQuaternion& Other);

	/**
	* Scalar multiplication assignment.
	* @param Scaler to multiply.
	*/
	FQuaternion& operator*=(const float Scalar);

	/**
	* Set each component of the quaternion.
	*/
	void Set(const float w, const float x, const float y, const float z);

	/**
	* Gets the inverse of this quaternion.
	*/
	FQuaternion Inverse() const;

	/**
	* Construct the 4x4 matrix representation of this
	* rotation quaternion.
	*/
	FMatrix4 ToMatrix4() const;

	/**
	* Calculate the magnitude of the quaternion.
	*/
	float Length() const;

	/**
	* Computes a quaternion dot product.
	* This can be used to check how similar to quaternion rotations'
	* angular displacements are. The larger the return values' absolute
	* value, the more similar the angular displacements of the two quaternions
	* are.
	*/
	static float Dot(const FQuaternion& Lhs, const FQuaternion& Rhs);

	/**
	* Computes the euler angle rotational representation of the quaternion, in radians.
	* @return Euler angles of the rotation in radians.
	*/
	static Vector3f EulerAngles(const FQuaternion& Quat);

	/**
	* Spherical linear interpolation starting at From, going to To.
	*/
	static FQuaternion Slerp(const FQuaternion& From, const FQuaternion& To, const float Delta);

	/**
	* Linear interpolation starting at From, going to To.
	*/
	static FQuaternion Lerp(const FQuaternion& From, const FQuaternion& To, const float Delta);

	/**
	* Constructs a quaternion that orients a rotation toward a target position.
	* @param Source - The source position.
	* @param Destination - The target position.
	*/
	static FQuaternion LookAt(const Vector3f& Source, const Vector3f& Destination);

	float w;
	float x;
	float y;
	float z;
};

/////////////////////////////////////////////////////
//////////// Inlined Non-Member Functions ///////////
/////////////////////////////////////////////////////

inline FQuaternion operator*(const FQuaternion& Lhs, const FQuaternion& Rhs)
{
	FQuaternion Result;
	MultQuatQuat(Lhs, Rhs, Result);
	return Result;
}

inline FQuaternion operator*(const FQuaternion& Lhs, const float Scale)
{
	return FQuaternion(
		Lhs.w * Scale,
		Lhs.x * Scale,
		Lhs.y * Scale,
		Lhs.z * Scale);
}

inline FQuaternion operator*(const float Scale, const FQuaternion& Lhs )
{
	return Lhs * Scale;
}

inline FQuaternion operator+(const FQuaternion& Lhs, const FQuaternion& Rhs)
{
	return FQuaternion(
		Lhs.w + Rhs.w,
		Lhs.x + Rhs.x,
		Lhs.y + Rhs.y,
		Lhs.z + Rhs.z);
}

template <typename T>
/**
* Rotates a vector by the quaternion rotation.
* @param Lhs Rotation quaternion
* @param Rhs Vector to rotate.
* @return The rotated vector.
*/
inline TVector3<T> operator*(const FQuaternion& Lhs, const TVector3<T>& Rhs)
{
	FQuaternion Point(0, Rhs.x, Rhs.y, Rhs.z);
	Point = Lhs * Point * Lhs.Inverse();
	return TVector3<T>(Point.x, Point.y, Point.z);
}

inline bool operator==(const FQuaternion& Lhs, const FQuaternion& Rhs)
{
	return 
		Lhs.w == Rhs.w &&
		Lhs.x == Rhs.x &&
		Lhs.y == Rhs.y &&
		Lhs.z == Rhs.z;
}

inline bool operator!=(const FQuaternion& Lhs, const FQuaternion& Rhs)
{
	return 
		Lhs.w != Rhs.w ||
		Lhs.x != Rhs.x ||
		Lhs.y != Rhs.y ||
		Lhs.z != Rhs.z;
}

/**
* Negates the Quaternion components.
* @param lhs - Left operand
* @return The negated Quaternion.
*/
inline FQuaternion operator-(const FQuaternion& lhs)
{
	return FQuaternion(-lhs.z, -lhs.x, -lhs.y, -lhs.z);
}

/////////////////////////////////////////////////////
//////////// Inlined Member Functions ///////////////
/////////////////////////////////////////////////////

inline FQuaternion::FQuaternion()
	: w(1), x(0), y(0), z(0)
{

}

inline FQuaternion::FQuaternion(const float w, const float x, const float y, const float z)
	: w(w), x(x), y(y), z(z)
{
	
}

inline FQuaternion::FQuaternion(const Vector3f& Axis, const float Degrees)
{
	const float HalfRadians = FMath::ToRadians(Degrees / 2.0f);
	w = std::cos(HalfRadians);

	const float SinHalfRadians = std::sin(HalfRadians);
	x = Axis.x * SinHalfRadians;
	y = Axis.y * SinHalfRadians;
	z = Axis.z * SinHalfRadians;
}

inline FQuaternion& FQuaternion::operator*=(const FQuaternion& Other)
{
	*this = *this * Other;
	return *this;
}

inline FQuaternion& FQuaternion::operator*=(const float Scalar)
{
	w *= Scalar;
	x *= Scalar;
	y *= Scalar;
	z *= Scalar;
	return *this;
}

inline void FQuaternion::Set(const float w, const float x, const float y, const float z)
{
	this->w = w;
	this->x = x;
	this->y = y;
	this->z = z;
}

inline FQuaternion FQuaternion::Inverse() const
{
	return FQuaternion(-w, x, y, z);
}

inline float FQuaternion::Length() const
{
	return sqrt(w*w + x*x + y*y + z*z);
}

inline float FQuaternion::Dot(const FQuaternion& Lhs, const FQuaternion& Rhs)
{
	float Result;
	Vector4f L{ Lhs.w, Lhs.x, Lhs.y, Lhs.z }, R{ Rhs.w, Rhs.x, Rhs.y, Rhs.z };
	Dot4Product(L, R, Result);
	return Result;
}