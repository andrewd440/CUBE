
#include "..\..\Include\Math\Quaternion.h"
#include "..\..\Include\Math\Vector3.h"
#include "Math\Matrix4.h"
#include "Common.h"

FQuaternion::FQuaternion(const float X, const float Y, const float Z)
{
	// Translate degrees to radians
	const float HalfXRadians = FMath::ToRadians(X/2.0f);
	const float HalfYRadians = FMath::ToRadians(Y/2.0f);
	const float HalfZRadians = FMath::ToRadians(Z/2.0f);

	// Cache reused calculations
	const float HalfXCos = cos(HalfXRadians);
	const float HalfXSin = sin(HalfXRadians);

	const float HalfYCos = cos(HalfYRadians);
	const float HalfYSin = sin(HalfYRadians);

	const float HalfZCos = cos(HalfZRadians);
	const float HalfZSin = sin(HalfZRadians);

	w = HalfYCos * HalfXCos * HalfZCos + HalfYSin * HalfXSin * HalfZSin;
	x = HalfYCos * HalfXSin * HalfZCos + HalfYSin * HalfXCos * HalfZSin;
	y = HalfYSin * HalfXCos * HalfZCos - HalfYCos * HalfXSin * HalfZSin;
	z = HalfYCos * HalfXCos * HalfZSin - HalfYSin * HalfXSin * HalfZCos;
}

Vector3f FQuaternion::EulerAngles(const FQuaternion& Quat)
{
	Vector3f Euler;

	// From 3D Math Primer for Graphics and Game Development
	// Extract sin(pitch)
	const float sp = -2.0f * (Quat.y * Quat.z - Quat.w * Quat.x);

	// Check for Gimbal lock
	if (abs(sp) > 1.0f - _EPSILON)
	{
		// Looking straight up or down
		Euler.x = 1.570796f * sp; // pi/2

		// Compute heading, slam back to zero
		Euler.y = atan2(-Quat.x * Quat.z + Quat.w * Quat.y, 0.5f - Quat.y * Quat.y - Quat.z * Quat.z);
		Euler.z = 0.0f;
	}
	else
	{
		// Compute angles
		Euler.x = asin(sp);
		Euler.y = atan2(Quat.x * Quat.z + Quat.w * Quat.y, 0.5f - Quat.x * Quat.x - Quat.y * Quat.y);
		Euler.z = atan2(Quat.x * Quat.y + Quat.w * Quat.z, 0.5f - Quat.x * Quat.x - Quat.z * Quat.z);
	}

	return Euler;
}

FQuaternion FQuaternion::Slerp(const FQuaternion& From, const FQuaternion& To, const float Delta)
{
	// From 3D Math Primer for Graphics and Game Development
	float Dot = FQuaternion::Dot(From, To);
	int32_t Sign = 1;
	// If Dot is negative, negate a quaternion to take the shorter
	// arc
	if (Dot < 0.0f)
	{
		Sign = -1;
		Dot = -Dot;
	}

	// Check if they are close together to protect against divide by zero.
	float k0, k1;
	if (Dot > 1.0f - _EPSILON)
	{
		// Very close, use linear interpolation
		k0 = 1.0f - Delta;
		k1 = Delta;
	}
	else
	{
		// Compute sin of angle using trig identity sin^2(omega) + cos^2(omega) = 1
		const float SinOmega = sqrt(1.0f - Dot*Dot);

		// Compute angle from its sine and cosine
		const float Omega = atan2(SinOmega, Dot);

		// Compute inverse of denom
		const float OneOverSinOmega = 1.0f / SinOmega;
		
		// Compute interpolation params
		k0 = sin((1.0f - Delta) * Omega) * OneOverSinOmega;
		k1 = sin(Delta * Omega) * OneOverSinOmega;
	}

	// interpolate
	return FQuaternion
	{
		Sign * From.w * k0 + To.w * k1,
		Sign * From.x * k0 + To.x * k1,
		Sign * From.y * k0 + To.y * k1,
		Sign * From.z * k0 + To.z * k1
	};
}

FQuaternion FQuaternion::Lerp(const FQuaternion& From, const FQuaternion& To, const float Delta)
{
	FQuaternion Quat{ (1 - Delta) * From + Delta * To };

	// normalize quat
	const float InvLength = 1.0f / Quat.Length();
	Quat *= InvLength;

	ASSERT(abs(1.0f - Quat.Length()) < _EPSILON && "Quaternion should be unit length.");

	return Quat;
}

FMatrix4 FQuaternion::ToMatrix4() const
{
	return FMatrix4
	{
		1 - 2*y*y - 2*z*z,	2*x*y - 2*w*z,			2*x*z + 2*w*y,		0,
		2*x*y + 2*w*z,		1 - 2*x*x - 2*z*z,		2*y*z - 2*w*x,		0,
		2*x*z - 2*w*y,		2*y*z + 2*w*x,			1 - 2*x*x - 2*y*y,	0,
		0,					0,						0,					1
	};
}

FQuaternion FQuaternion::LookAt(const Vector3f& Source, const Vector3f& Destination)
{
	Vector3f TargetDirection = (Destination - Source).Normalize();

	float Dot = Vector3f::Dot(Vector3f::Forward, TargetDirection);

	if (abs(Dot + 1.0f) < _EPSILON)
	{
		return FQuaternion{ 0, 1, 0, _PI };
	}
	if (abs(Dot - 1.0f) < _EPSILON)
	{
		return FQuaternion{};
	}

	float RotationAngle = acos(Dot);
	Vector3f RotationAxis = Vector3f::Cross(Vector3f::Forward, TargetDirection);
	return FQuaternion{ RotationAxis, RotationAngle };
}