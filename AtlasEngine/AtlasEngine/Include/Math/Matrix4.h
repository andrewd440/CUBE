#pragma once

#include <initializer_list>

#include "Common.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Ray.h"

/**
*	4x4 floating-point column-matrix
*	Elements are access by [row][col]
*/
struct FMatrix4
{
	/** 
	* Each row in the matrix is a vector.
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
	* Transforms a vector without taking translation into account.
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

inline FMatrix4 operator*(FMatrix4 Lhs, float Scalar)
{
	return Lhs *= Scalar;
}

inline FMatrix4 operator*(float Scalar, FMatrix4 Lhs)
{
	return Lhs *= Scalar;
}

inline FMatrix4 operator+(FMatrix4 Lhs, const FMatrix4& Rhs)
{
	return Lhs += Rhs;
}

inline FMatrix4 operator*(FMatrix4 Lhs, const FMatrix4& Rhs)
{
	return (Lhs *= Rhs);
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

	M[0][0] = *(itr++); M[0][1] = *(itr++); M[0][2] = *(itr++); M[0][3] = *(itr++);
	M[1][0] = *(itr++); M[1][1] = *(itr++); M[1][2] = *(itr++); M[1][3] = *(itr++);
	M[2][0] = *(itr++); M[2][1] = *(itr++); M[2][2] = *(itr++); M[2][3] = *(itr++);
	M[3][0] = *(itr++); M[3][1] = *(itr++); M[3][2] = *(itr++); M[3][3] = *(itr++);
}

inline FMatrix4::FMatrix4(const Vector3f& XBasis, const Vector3f& YBasis, const Vector3f& ZBasis)
{
	for (int col = 0; col < 3; col++)
	{
		M[0][col] = XBasis[col];
		M[1][col] = YBasis[col];
		M[2][col] = ZBasis[col];
	}

	M[3][0] = M[3][1] = M[3][2] = M[0][3] = M[1][3] = M[2][3] = 0.0f;
	M[3][3] = 1.0f;
}

inline FMatrix4::FMatrix4(const FMatrix4& Other)
{
	for (int row = 0; row < 4; row++)
	{
		for (int col = 0; col < 4; col++)
		{
			M[row][col] = Other.M[row][col];
		}
	}
}

inline FMatrix4& FMatrix4::operator=(const FMatrix4& Other)
{
	for (int row = 0; row < 4; row++)
	{
		for (int col = 0; col < 4; col++)
		{
			M[row][col] = Other.M[row][col];
		}
	}

	return *this;
}

inline FMatrix4& FMatrix4::operator*=(const FMatrix4& Rhs)
{
	float tempM[4][4];
	for (int row = 0; row < 4; row++)
	{
		for (int col = 0; col < 4; col++)
		{
			tempM[row][col] = Vector4f::Dot4(GetRow(row), Rhs.GetColumn(col));
		}
	}

	for (int row = 0; row < 4; row++)
	{
		for (int col = 0; col < 4; col++)
		{
			M[row][col] = tempM[row][col];
		}
	}


	return *this;
}

inline FMatrix4& FMatrix4::operator*=(float Scalar)
{
	for (int row = 0; row < 4; row++)
	{
		for (int col = 0; col < 4; col++)
		{
			M[row][col] *= Scalar;
		}
	}

	return *this;
}

inline FMatrix4& FMatrix4::operator+=(const FMatrix4& Rhs)
{
	for (int row = 0; row < 4; row++)
	{
		for (int col = 0; col < 4; col++)
		{
			M[row][col] += Rhs.M[row][col];
		}
	}

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
	Vector4f transformed;
	transformed.x = Vector4f::Dot4(GetRow(0), Vector);
	transformed.y = Vector4f::Dot4(GetRow(1), Vector);
	transformed.z = Vector4f::Dot4(GetRow(2), Vector);
	transformed.w = Vector4f::Dot4(GetRow(3), Vector);
	return transformed;
}

inline Vector3f FMatrix4::GetAxis(EAxis Axis) const
{
	switch (Axis)
	{
	case EAxis::X:
		return Vector3f(M[0][0], M[1][0], M[2][0]);
	case EAxis::Y:
		return Vector3f(M[0][1], M[1][1], M[2][1]);
	case EAxis::Z:
		return Vector3f(M[0][2], M[1][2], M[2][2]);
	}
}

inline void FMatrix4::SetAxis(EAxis Axis, const Vector3f& Basis)
{
	int col = -1;
	switch (Axis)
	{
	case EAxis::X:
			col = 0;
			break;
	case EAxis::Y:
			col = 1;
			break;
	case EAxis::Z:
			col = 2;
			break;
		default:
			return;
	}

	M[0][col] = Basis.x; M[1][col] = Basis.y; M[2][col] = Basis.z;
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
	M[0][3] = M[1][3] = M[2][3] = 0.0f;

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
	int intAxis = -1;
	switch (Axis)
	{
	case EAxis::X:
		intAxis = 0;
		break;
	case EAxis::Y:
		intAxis = 1;
		break;
	case EAxis::Z:
		intAxis = 2;
		break;
	default:
		return;
	}

	
	// set old scale values
	Vector3f newScale = GetScale();
	newScale[intAxis] = Scale;
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
	return Vector4f(M[0][Col], M[1][Col], M[2][Col], M[3][Col]);
}

inline Vector4f FMatrix4::GetRow(int Row) const
{
	return Vector4f(M[Row][0], M[Row][1], M[Row][2], M[Row][3]);
}

inline Vector3f FMatrix4::GetOrigin() const
{
	return Vector3f(M[0][3], M[1][3], M[2][3]);
}

inline void FMatrix4::SetOrigin(const Vector3f& Origin)
{
	M[0][3] = Origin.x; M[1][3] = Origin.y; M[2][3] = Origin.z;
}

inline FMatrix4 FMatrix4::GetInverse() const
{
	FMatrix4 inv;
	float* invM = &(inv.M[0][0]);
	const float	m0 = M[0][0],	m1 = M[0][1],	m2 = M[0][2],	m3 = M[0][3],
				m4 = M[1][0],	m5 = M[1][1],	m6 = M[1][2],	m7 = M[1][3],
				m8 = M[2][0],	m9 = M[2][1],	m10 = M[2][2],	m11 = M[2][3],
				m12 = M[3][0],	m13 = M[3][1],	m14 = M[3][2],	m15 = M[3][3];

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
	for (int row = 0; row < 3; row++)
	{
		for (int col = 0; col < 3; col++)
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
	Vector3f N = -(LookLocation - Eye).Normalize();
	Vector3f U = Vector3f::Cross(N, UpDirection).Normalize();
	Vector3f V = Vector3f::Cross(U, N).Normalize();

	for (int col = 0; col < 3; col++)
	{
		M[0][col] = U[col];
		M[1][col] = V[col];
		M[2][col] = N[col];
		M[3][col] = 0.0f;
	}

	M[0][3] = -Vector3f::Dot(Eye, U);
	M[1][3] = -Vector3f::Dot(Eye, V);
	M[2][3] = -Vector3f::Dot(Eye, N);
	M[3][3] = 1.0f;
}

inline LookAtMatrix& LookAtMatrix::operator=(const FMatrix4& Other)
{
	for (int row = 0; row < 4; row++)
	{
		for (int col = 0; col < 4; col++)
		{
			M[row][col] = Other.M[row][col];
		}
	}

	return *this;
}