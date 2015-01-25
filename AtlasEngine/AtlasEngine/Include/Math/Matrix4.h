#pragma once

#include <initializer_list>
#include <algorithm>
#include <cstdint>

#include "SystemMath.h"
#include "..\Common.h"
#include "FMath.h"
#include "Math\Vector3.h"
#include "Math\Vector4.h"
#include "Misc\Assertions.h"


/**
*	4x4 floating-point colum-vector column-major matrix
*	Elements are accessed by M[col][row]
*/
WIN_ALIGN(16)
struct FMatrix4
{
	/** 
	* M[0][-] is column one in the matrix
	*/
	float M[4][4];

	/**
	* Constructs identity matrix.
	*/
	FMatrix4();

	/**
	* Constructs member-wise initialized matrix.
	* @param Mat - Matrix element values by row
	*/
	FMatrix4(const std::initializer_list<float>& Mat);

	/**
	* Constructs a matrix by basis vectors.
	* @param XBasis X vector
	* @param YBasis Y vector
	* @param ZBasis Z vector
	*/
	FMatrix4(const Vector3f& XBasis, const Vector3f& YBasis, const Vector3f& ZBasis);

	/**
	* Copy Constructor
	*/
	FMatrix4(const FMatrix4& Other);
	
	/**
	* Default destructor.
	*/
	~FMatrix4() = default;

	/**
	* Copy assignment
	*/
	FMatrix4& operator=(const FMatrix4& Other);

	/**
	* Performs matrix-matrix multiplication.
	* @param Rhs Matrix to multiply by.
	* @return Reference to this matrix.
	*/
	FMatrix4& operator*=(const FMatrix4& Rhs);

	/**
	* Performs matrix-scalar multiplication.
	* @param scalar - Scalar to multiply by.
	* @return Reference to this matrix.
	*/
	FMatrix4& operator*=(float Scalar);

	/**
	* Performs matrix component-wise addition.
	* @param Rhs Matrix to add.
	* @return Reference to this matrix.
	*/
	FMatrix4& operator+=(const FMatrix4& Rhs);

	/**
	* Checks matrix equality.
	*/
	bool operator==(const FMatrix4& Rhs) const;

	/**
	* Checks matrix inequality.
	*/
	bool operator!=(const FMatrix4& Rhs) const;

	/**
	* Get the transpose of the matrix.
	*/
	FMatrix4 Transpose() const;

	/**
	* Calculates the determinant of the matrix.
	*/
	float Determinant() const;

	/**
	* Used to transform a surface normal. The transformed normal
	* is robust against non-uniform scaling factors.
	* @param Normal to transform
	*/
	Vector3f TransformNormal(const Vector3f& Normal) const;

	/**
	* Transforms a vector without taking translation into account.
	* (For transforming normals with non-uniform scale, use TransformNormal)
	* @param Direction to transform
	*/
	Vector3f TransformDirection(const Vector3f& Direction) const;

	/**
	* Transforms a position, taking translation into account.
	*/
	Vector3f TransformPosition(const Vector3f& Position) const;

	/**
	* Transforms a vector with homogeneous coordinates.
	*/
	Vector4f TransformVector(const Vector4f& Vector) const;

	/**
	* Retrieve an axis vector from the matrix.
	*/
	Vector3f GetAxis(EAxis Axis) const;

	/**
	* Sets a basis vector in the matrix.
	*/
	void SetAxis(EAxis Axis, const Vector3f& Basis);

	/**
	* Gets the world origin of the matrix. (translation vector)
	*/
	Vector3f GetOrigin() const;

	/**
	* Sets the world origin of the matrix. (translation vector)
	*/
	void SetOrigin(const Vector3f& Origin);

	/**
	* Rotate, in degrees, around an axis.
	* @param Axis to rotate around.
	* @param Degrees to rotate by.
	*/
	void Rotate(EAxis Axis, float Degrees);

	/**
	* Rotate, in degrees, each asix.
	* @param Degrees to rotate by on each axis.
	*/
	void Rotate(Vector3f Degrees);

	/**
	* Sets a uniform scale from origin.
	* @param Scale Value to scale by.
	*/
	void Scale(float Scale);

	/**
	* Sets the scale about an axis from origin.
	* @param Axis to scale by.
	* @param Scale Value to scale by.
	*/
	void Scale(EAxis Axis, float Scale);

	/**
	* Sets the scale about each axis from a vector.
	* @param Scale Values to scale by.
	*/
	void Scale(const Vector3f& Scale);

	/**
	* Gets the scale vector from the matrix.
	*/
	Vector3f FMatrix4::GetScale() const;

	/**
	* Retreive a Column of the matrix. Includes translation component.
	* @param Row of the matrix, 0 = first row, 1 = second row ...
	*/
	Vector4f GetColumn(int Col) const;

	/**
	* Retreive a row of the matrix. Includes translation component.
	* @param Row of the matrix, 0 = first row, 1 = second row ...
	*/
	Vector4f GetRow(int Row) const;

	/**
	* Calculates the inverse of the matrix.
	*/
	FMatrix4 GetInverse() const;

	/**
	* Calculates the inverse of an affine 4x4 matrix.
	*/
	FMatrix4 GetInverseAffine() const;
};

///////////////////////////////////////////////////////////////////////////
////////////////// Non Member Functions ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////

inline FMatrix4 operator*(const FMatrix4& Lhs, const float Scalar)
{
	FMatrix4 Result;
	FMatrix4 Scale
	{
		Scalar, Scalar, Scalar, Scalar,
		Scalar, Scalar, Scalar, Scalar,
		Scalar, Scalar, Scalar, Scalar,
		Scalar, Scalar, Scalar, Scalar
	};

	MultComponentMatrixMatrix(Lhs, Scale, Result);
	return Result;
}

inline FMatrix4 operator*(const float Scalar, const FMatrix4& Lhs)
{
	return Lhs * Scalar;
}

inline FMatrix4 operator+(const FMatrix4& Lhs, const FMatrix4& Rhs)
{
	FMatrix4 Result;
	AddMatrixMatrix(Lhs, Rhs, Result);
	return Result;
}

inline FMatrix4 operator*(const FMatrix4& Lhs, const FMatrix4& Rhs)
{
	FMatrix4 Result;
	MultMatrixMatrix(Lhs, Rhs, Result);
	return Result;
}

///////////////////////////////////////////////////////////////////////////
////////////////// Inlined Member Functions ///////////////////////////////
///////////////////////////////////////////////////////////////////////////

inline FMatrix4::FMatrix4()
	: FMatrix4({ 
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f })
{}

inline FMatrix4::FMatrix4(const std::initializer_list<float>& Mat)
{
	ASSERT(Mat.size() == 16 && "FMatrix4 not fully initialized");

	auto itr = Mat.begin();

	M[0][0] = *(itr++); M[1][0] = *(itr++); M[2][0] = *(itr++); M[3][0] = *(itr++);
	M[0][1] = *(itr++); M[1][1] = *(itr++); M[2][1] = *(itr++); M[3][1] = *(itr++);
	M[0][2] = *(itr++); M[1][2] = *(itr++); M[2][2] = *(itr++); M[3][2] = *(itr++);
	M[0][3] = *(itr++); M[1][3] = *(itr++); M[2][3] = *(itr++); M[3][3] = *(itr++);
}

inline FMatrix4::FMatrix4(const Vector3f& XBasis, const Vector3f& YBasis, const Vector3f& ZBasis)
{
	for (int row = 0; row < 3; row++)
	{
		M[0][row] = XBasis[row];
		M[1][row] = YBasis[row];
		M[2][row] = ZBasis[row];
	}

	M[3][0] = M[3][1] = M[3][2] = M[0][3] = M[1][3] = M[2][3] = 0.0f;
	M[3][3] = 1.0f;
}

inline FMatrix4::FMatrix4(const FMatrix4& Other)
{
	for (int col = 0; col < 4; col++)
	{
		for (int row = 0; row < 4; row++)
		{
			M[col][row] = Other.M[col][row];
		}
	}
}

inline FMatrix4& FMatrix4::operator=(const FMatrix4& Other)
{
	for (int col = 0; col < 4; col++)
	{
		for (int row = 0; row < 4; row++)
		{
			M[col][row] = Other.M[col][row];
		}
	}

	return *this;
}

inline FMatrix4& FMatrix4::operator*=(const FMatrix4& Rhs)
{
	MultMatrixMatrix(*this, Rhs, *this);
	return *this;
}

inline FMatrix4& FMatrix4::operator*=(float Scalar)
{
	*this = *this * Scalar;
	return *this;
}

inline FMatrix4& FMatrix4::operator+=(const FMatrix4& Rhs)
{
	*this = *this + Rhs;
	return *this;
}

inline bool FMatrix4::operator==(const FMatrix4& Rhs) const
{
	for (size_t i = 0; i < 4; i++)
		for (size_t j = 0; j < 4; j++)
			if (M[i][j] != Rhs.M[i][j])
				return false;

	return true;
}

inline bool FMatrix4::operator!=(const FMatrix4& Rhs) const
{
	return !(*this == Rhs);
}

inline FMatrix4 FMatrix4::Transpose() const
{
	return FMatrix4{
		M[0][0], M[1][0], M[2][0], M[3][0],
		M[0][1], M[1][1], M[2][1], M[3][1],
		M[0][2], M[1][2], M[2][2], M[3][2],
		M[0][3], M[1][3], M[2][3], M[3][3]
	};
}

inline float FMatrix4::Determinant() const
{
	return M[0][0] * (M[1][1] * M[2][2] - M[2][1] * M[1][2])
		- M[1][0] * (M[0][1] * M[2][2] - M[2][1] * M[0][2])
		- M[2][0] * (M[0][1] * M[1][2] - M[1][1] * M[0][2]);
}

inline Vector3f FMatrix4::TransformNormal(const Vector3f& Normal) const
{
	return GetInverse().Transpose().TransformDirection(Normal);
}

inline Vector3f FMatrix4::TransformDirection(const Vector3f& Direction) const
{
	Vector4f vec(Direction);
	vec.w = 0.0f;
	vec = TransformVector(vec);
	return Vector3f(vec.x, vec.y, vec.z);
}

inline Vector3f FMatrix4::TransformPosition(const Vector3f& Position) const
{
	Vector4f vec(TransformVector(Position));
	return Vector3f(vec.x, vec.y, vec.z);
}

inline Vector4f FMatrix4::TransformVector(const Vector4f& Vector) const
{
	Vector4f Transformed;
	MultVectorMatrix(*this, Vector, Transformed);
	return Transformed;
}

inline Vector3f FMatrix4::GetAxis(EAxis Axis) const
{
	switch (Axis)
	{
	case EAxis::X:
		return Vector3f(M[0][0], M[0][1], M[0][2]);
	case EAxis::Y:
		return Vector3f(M[1][0], M[1][1], M[1][2]);
	case EAxis::Z:
		return Vector3f(M[2][0], M[2][1], M[2][2]);
	}
}

inline void FMatrix4::SetAxis(EAxis Axis, const Vector3f& Basis)
{
	M[Axis][0] = Basis.x; M[Axis][1] = Basis.y; M[Axis][2] = Basis.z;
}

inline void FMatrix4::Rotate(EAxis Axis, float Degrees)
{
	float radians = (float)(_PI * Degrees / 180.0f);
	FMatrix4 rotationMat;

	switch (Axis)
	{
	case EAxis::X:
		rotationMat = FMatrix4{ 
			1.0f,			0.0f,					0.0f,					0.0f,
			0.0f,			(float)cos(radians),	(float)-sin(radians),	0.0f,
			0.0f,			(float)sin(radians),	(float)cos(radians),	0.0f,
			0.0f,			 0.0f,					0.0f,					1.0f };
		break;
	case EAxis::Y:
		rotationMat = FMatrix4{
			(float)cos(radians),	0.0f,	(float)sin(radians),	0.0f,
			0.0f,					1.0f,	0.0f,					0.0f,
			(float)-sin(radians),	0.0f,	(float)cos(radians),	0.0f,
			0.0f,					0.0f,	0.0f,					1.0f };
		break;
	case EAxis::Z:
		rotationMat = FMatrix4{
			(float)cos(radians),	(float)-sin(radians),	0.0f,	0.0f,
			(float)sin(radians),	(float)cos(radians),	0.0f,	0.0f,
			0.0f,					0.0f,					1.0f,	0.0f,
			0.0f,					0.0f,					0.0f,	1.0f };
		break;
	default:
		return;
	}

	// save and remove translation
	Vector3f translation = GetOrigin();
	M[3][0] = M[3][1] = M[3][2] = 0.0f;

	*this = rotationMat * (*this);
	SetOrigin(translation);
}

inline void FMatrix4::Rotate(Vector3f Degrees)
{
	Rotate(EAxis::X, Degrees.x);
	Rotate(EAxis::Y, Degrees.y);
	Rotate(EAxis::Z, Degrees.z);
}

inline void FMatrix4::Scale(float Scale)
{
	FMatrix4::Scale(Vector3f(Scale, Scale, Scale));
}

inline void FMatrix4::Scale(EAxis Axis, float Scale)
{	
	// Get old scale values and modify
	Vector3f newScale = GetScale();
	newScale[Axis] = Scale;
	FMatrix4::Scale(newScale);

}

inline void FMatrix4::Scale(const Vector3f& Scale)
{
	FMatrix4 scaleMat;

	// set new scale values
	for (int i = 0; i < 3; i++)
		scaleMat.M[i][i] = Scale[i];

	// save translation and go to origin
	const Vector3f& oldTranslation(GetOrigin());
	SetOrigin(Vector3f());

	// scale and translate back into place
	*this = *this * scaleMat;
	SetOrigin(oldTranslation);
}

inline Vector3f FMatrix4::GetScale() const
{
	return Vector3f(M[0][0], M[1][1], M[2][2]);
}

inline Vector4f FMatrix4::GetColumn(int Col) const
{
	return Vector4f(M[Col][0], M[Col][1], M[Col][2], M[Col][3]);
}

inline Vector4f FMatrix4::GetRow(int Row) const
{
	return Vector4f(M[0][Row], M[1][Row], M[2][Row], M[3][Row]);
}

inline Vector3f FMatrix4::GetOrigin() const
{
	return Vector3f(M[3][0], M[3][1], M[3][2]);
}

inline void FMatrix4::SetOrigin(const Vector3f& Origin)
{
	M[3][0] = Origin.x; M[3][1] = Origin.y; M[3][2] = Origin.z;
}

inline FMatrix4 FMatrix4::GetInverse() const
{
	FMatrix4 inv;
	float* invM = &(inv.M[0][0]);
	const float	m0 = M[0][0],	m1 = M[1][0],	m2 = M[2][0],	m3 = M[3][0],
				m4 = M[0][1],	m5 = M[1][1],	m6 = M[2][1],	m7 = M[3][1],
				m8 = M[0][2],	m9 = M[1][2],	m10 = M[2][2],	m11 = M[3][2],
				m12 = M[0][3],	m13 = M[1][3],	m14 = M[2][3],	m15 = M[3][3];

	invM[0] = m5 * m10 * m15 - m5 * m11 * m14 - m9 * m6 * m15 + m9 * m7 * m14 + m13 * m6 * m11 - m13 * m7 * m10;
	invM[1] = -m1 * m10 * m15 + m1 * m11 * m14 + m9 * m2 * m15 - m9 * m3 * m14 - m13 * m2 * m11 + m13 * m3 * m10;
	invM[2] = m1 * m6  * m15 - m1 * m7  * m14 - m5 * m2 * m15 + m5 * m3 * m14 + m13 * m2 * m7 - m13 * m3 * m6;
	invM[3] = -m1 * m6  * m11 + m1 * m7  * m10 + m5 * m2 * m11 - m5 * m3 * m10 - m9  * m2 * m7 + m9  * m3 * m6;
	invM[4] = -m4 * m10 * m15 + m4 * m11 * m14 + m8 * m6 * m15 - m8 * m7 * m14 - m12 * m6 * m11 + m12 * m7 * m10;
	invM[5] = m0 * m10 * m15 - m0 * m11 * m14 - m8 * m2 * m15 + m8 * m3 * m14 + m12 * m2 * m11 - m12 * m3 * m10;
	invM[6] = -m0 * m6  * m15 + m0 * m7  * m14 + m4 * m2 * m15 - m4 * m3 * m14 - m12 * m2 * m7 + m12 * m3 * m6;
	invM[7] = m0 * m6  * m11 - m0 * m7  * m10 - m4 * m2 * m11 + m4 * m3 * m10 + m8  * m2 * m7 - m8  * m3 * m6;
	invM[8] = m4 * m9  * m15 - m4 * m11 * m13 - m8 * m5 * m15 + m8 * m7 * m13 + m12 * m5 * m11 - m12 * m7 * m9;
	invM[9] = -m0 * m9  * m15 + m0 * m11 * m13 + m8 * m1 * m15 - m8 * m3 * m13 - m12 * m1 * m11 + m12 * m3 * m9;
	invM[10] = m0 * m5  * m15 - m0 * m7  * m13 - m4 * m1 * m15 + m4 * m3 * m13 + m12 * m1 * m7 - m12 * m3 * m5;
	invM[11] = -m0 * m5  * m11 + m0 * m7  * m9 + m4 * m1 * m11 - m4 * m3 * m9 - m8  * m1 * m7 + m8  * m3 * m5;
	invM[12] = -m4 * m9  * m14 + m4 * m10 * m13 + m8 * m5 * m14 - m8 * m6 * m13 - m12 * m5 * m10 + m12 * m6 * m9;
	invM[13] = m0 * m9  * m14 - m0 * m10 * m13 - m8 * m1 * m14 + m8 * m2 * m13 + m12 * m1 * m10 - m12 * m2 * m9;
	invM[14] = -m0 * m5  * m14 + m0 * m6  * m13 + m4 * m1 * m14 - m4 * m2 * m13 - m12 * m1 * m6 + m12 * m2 * m5;
	invM[15] = m0 * m5  * m10 - m0 * m6  * m9 - m4 * m1 * m10 + m4 * m2 * m9 + m8  * m1 * m6 - m8  * m2 * m5;

	float det = m0 * invM[0] + m1 * invM[4] + m2 * invM[8] + m3 * invM[12];
	if (det != 0.0f)
	{
		det = 1.0f / det;
	}
	
	for (int i = 0; i < 16; i++)
	{
		invM[i] *= det;
	}

	return inv;
}

inline FMatrix4 FMatrix4::GetInverseAffine() const
{
	FMatrix4 result;
	const Vector3f& translateVector(GetOrigin());

	// transpose 3x3 portion
	for (int col = 0; col < 3; col++)
	{
		for (int row = 0; row < 3; row++)
		{
			result.M[row][col] = M[col][row];
		}
	}

	Vector3f inverseTranslate;
	inverseTranslate = -result.TransformPosition(translateVector);

	result.SetOrigin(inverseTranslate);
	return result;
}


/**
* A camera look-at matrix.
*/
struct LookAtMatrix : public FMatrix4
{
	/**
	* Constructs a look-at matrix that takes objects from view space to world space.
	* @param Eye Position of the eye.
	* @param LookLocation Position to look toward.
	* @param UpDirection Direction of up.
	*/
	LookAtMatrix(const Vector3f& Eye, const Vector3f& LookLocation, const Vector3f& UpDirection);

	LookAtMatrix& operator=(const FMatrix4& Other);
};

inline LookAtMatrix::LookAtMatrix(const Vector3f& Eye, const Vector3f& LookLocation, const Vector3f& UpDirection)
{
	Vector3f N = (LookLocation - Eye).Normalize();
	Vector3f U = Vector3f::Cross(N, UpDirection).Normalize();
	Vector3f V = Vector3f::Cross(U, N).Normalize();

	for (int col = 0; col < 3; col++)
	{
		M[col][0] = U[col];
		M[col][1] = V[col];
		M[col][2] = -N[col];
		M[col][3] = 0.0f;
	}

	M[3][0] = Vector3f::Dot(-Eye, U);
	M[3][1] = Vector3f::Dot(-Eye, V);
	M[3][2] = Vector3f::Dot(-Eye, -N);
	M[3][3] = 1.0f;
}

inline LookAtMatrix& LookAtMatrix::operator=(const FMatrix4& Other)
{
	for (int col = 0; col < 4; col++)
	{
		for (int row = 0; row < 4; row++)
		{
			M[col][row] = Other.M[col][row];
		}
	}

	return *this;
}