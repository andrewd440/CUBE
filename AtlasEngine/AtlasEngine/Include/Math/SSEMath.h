#pragma once

#include <emmintrin.h>

struct FMatrix4;
class FQuaternion;
template <typename T>
class TVector4;
using Vector4f = TVector4<float>;
using Vector4i = TVector4<int32_t>;

#define SHUFFLE_PARAM(x, y, z, w) \
	((x) | ((y) << 2) | ((z) << 4) | ((w) << 6))

#define _mm_replicate_x_ps(v) \
	_mm_shuffle_ps((v), (v), SHUFFLE_PARAM(0, 0, 0, 0))

#define _mm_replicate_y_ps(v) \
	_mm_shuffle_ps((v), (v), SHUFFLE_PARAM(1, 1, 1, 1))

#define _mm_replicate_z_ps(v) \
	_mm_shuffle_ps((v), (v), SHUFFLE_PARAM(2, 2, 2, 2))

#define _mm_replicate_w_ps(v) \
	_mm_shuffle_ps((v), (v), SHUFFLE_PARAM(3, 3, 3, 3))

#define _mm_madd_ps(a, b, c) \
	_mm_add_ps(_mm_mul_ps((a), (b)), (c))

/**
* Matrix - Vector multiplication with SSE SIMD instructions.
* Matrix should be column-vector and stored as column-major.
* The FMatrix4 and Vector4f must be 16 byte aligned.
* @param Matrix to multiply
* @param Vector to multiply
* @param ResultOUt The resulting calculation will be placed in this variable.
*/
__forceinline void MultVectorMatrix(const FMatrix4& Matrix, const Vector4f& Vector, Vector4f& ResultOut)
{
	__m128* V = (__m128*)&Vector;
	const __m128* MCols = (const __m128*)&Matrix;

	// Multiply and add each column in the matrix with respective component of the vector
	__m128 Mult = _mm_mul_ps(MCols[0], _mm_replicate_x_ps(*V));
	Mult = _mm_madd_ps(MCols[1], _mm_replicate_y_ps(*V), Mult);
	Mult = _mm_madd_ps(MCols[2], _mm_replicate_z_ps(*V), Mult);
	Mult = _mm_madd_ps(MCols[3], _mm_replicate_w_ps(*V), Mult);

	// Store the result
	__m128* Result = (__m128*)&ResultOut;
	*Result = Mult;
}

/**
* 4x4 Matrix multiplication.
* @param Mat1 - First matrix
* @param Mat2 - Second matrix
* @param ResultOut - Location of the result.
*/
__forceinline void MultMatrixMatrix(const FMatrix4& Mat1, const FMatrix4& Mat2, FMatrix4& ResultOut)
{
	const __m128* M1 = (const __m128*)&Mat1;
	const __m128* M2 = (const __m128*)&Mat2;
	__m128* Result = (__m128*)&ResultOut;

	// Mat1 * Mat2[0]
	__m128 R0 = _mm_mul_ps(M1[0], _mm_replicate_x_ps(M2[0]));
	R0 = _mm_madd_ps(M1[1], _mm_replicate_y_ps(M2[0]), R0);
	R0 = _mm_madd_ps(M1[2], _mm_replicate_z_ps(M2[0]), R0);
	R0 = _mm_madd_ps(M1[3], _mm_replicate_w_ps(M2[0]), R0);

	// Mat1 * Mat2[1]
	__m128 R1 = _mm_mul_ps(M1[0], _mm_replicate_x_ps(M2[1]));
	R1 = _mm_madd_ps(M1[1], _mm_replicate_y_ps(M2[1]), R1);
	R1 = _mm_madd_ps(M1[2], _mm_replicate_z_ps(M2[1]), R1);
	R1 = _mm_madd_ps(M1[3], _mm_replicate_w_ps(M2[1]), R1);

	// Mat1 * Mat2[2]
	__m128 R2 = _mm_mul_ps(M1[0], _mm_replicate_x_ps(M2[2]));
	R2 = _mm_madd_ps(M1[1], _mm_replicate_y_ps(M2[2]), R2);
	R2 = _mm_madd_ps(M1[2], _mm_replicate_z_ps(M2[2]), R2);
	R2 = _mm_madd_ps(M1[3], _mm_replicate_w_ps(M2[2]), R2);

	// Mat1 * Mat2[3]
	__m128 R3 = _mm_mul_ps(M1[0], _mm_replicate_x_ps(M2[3]));
	R3 = _mm_madd_ps(M1[1], _mm_replicate_y_ps(M2[3]), R3);
	R3 = _mm_madd_ps(M1[2], _mm_replicate_z_ps(M2[3]), R3);
	R3 = _mm_madd_ps(M1[3], _mm_replicate_w_ps(M2[3]), R3);

	// Store the result
	Result[0] = R0;
	Result[1] = R1;
	Result[2] = R2;
	Result[3] = R3;
}

/**
* 4x4 Matrix component-wise addition.
* @param Mat1 - First matrix
* @param Mat2 - Second matrix
* @param ResultOut - Location of the result.
*/
__forceinline void AddMatrixMatrix(const FMatrix4& Mat1, const FMatrix4& Mat2, FMatrix4& ResultOut)
{
	const __m128* M1 = (const __m128*)&Mat1;
	const __m128* M2 = (const __m128*)&Mat2;

	const __m128 Col0 = _mm_add_ps(M1[0], M2[0]);
	const __m128 Col1 = _mm_add_ps(M1[1], M2[1]);
	const __m128 Col2 = _mm_add_ps(M1[2], M2[2]);
	const __m128 Col3 = _mm_add_ps(M1[3], M2[3]);

	__m128* Result = (__m128*)&ResultOut;
	Result[0] = Col0;
	Result[1] = Col1;
	Result[2] = Col2;
	Result[3] = Col3;
}

/**
* 4x4 Matrix component-wise multiplication.
* @param Mat1 - First matrix
* @param Mat2 - Second matrix
* @param ResultOut - Location of the result.
*/
__forceinline void MultComponentMatrixMatrix(const FMatrix4& Mat1, const FMatrix4& Mat2, FMatrix4& ResultOut)
{
	const __m128* M1 = (const __m128*)&Mat1;
	const __m128* M2 = (const __m128*)&Mat2;

	const __m128 Col0 = _mm_mul_ps(M1[0], M2[0]);
	const __m128 Col1 = _mm_mul_ps(M1[1], M2[1]);
	const __m128 Col2 = _mm_mul_ps(M1[2], M2[2]);
	const __m128 Col3 = _mm_mul_ps(M1[3], M2[3]);

	__m128* Result = (__m128*)&ResultOut;
	Result[0] = Col0;
	Result[1] = Col1;
	Result[2] = Col2;
	Result[3] = Col3;
}

template <typename T>
/**
* Gets the 4-component dot product of two vectors.
* If a 3-component dot product is required, 0 the w componet
* of the vectors.
* @param Vec1 First vector
* @param Vec2 Second vector
* @ResultOut Location the result should be placed.
*/
__forceinline void Dot4Product(const TVector4<T>& Vec1, const TVector4<T>& Vec2, T& ResultOut)
{
	// default to FPU for integer types for now
	ResultOut = Vec1.x * Vec2.x + Vec1.y * Vec2.y + Vec1.z * Vec2.z + Vec1.w * Vec2.w;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////// Dot4Product specializations //////////////////////////////////////////////////////////////////////////

template <>
/**
* Gets the 4-component dot product of two vectors.
* If a 3-component dot product is required, 0 the w componet
* of the vectors.
* @param Vec1 First vector
* @param Vec2 Second vector
* @ResultOut Location the result should be placed.
*/
__forceinline void Dot4Product(const Vector4f& Vec1, const Vector4f& Vec2, float& ResultOut)
{
	const __m128* V1 = (const __m128*)&Vec1;
	const __m128* V2 = (const __m128*)&Vec2;

	__m128 Dot = _mm_mul_ps(V1[0], V2[0]);
	Dot = _mm_hadd_ps(Dot, Dot);
	Dot = _mm_hadd_ps(Dot, Dot);

	ResultOut = Dot.m128_f32[0];
}

__forceinline void Dot4Product(const float* Vec1, const float* Vec2, float& ResultOut)
{
	const __m128* V1 = (const __m128*)Vec1;
	const __m128* V2 = (const __m128*)Vec2;

	__m128 Dot = _mm_mul_ps(*V1, *V2);
	Dot = _mm_hadd_ps(Dot, Dot);
	Dot = _mm_hadd_ps(Dot, Dot);

	ResultOut = Dot.m128_f32[0];
}

template <>
__forceinline void Dot4Product(const Vector4i& Vec1, const Vector4i& Vec2, int32_t& ResultOut)
{
	const __m128i* V1 = (const __m128i*)&Vec1;
	const __m128i* V2 = (const __m128i*)&Vec2;

	__m128i Dot = _mm_mullo_epi32(*V1, *V2);
	Dot = _mm_hadd_epi32(Dot, Dot);
	Dot = _mm_hadd_epi32(Dot, Dot);

	ResultOut = Dot.m128i_i32[0];
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
/**
* Component-wise addition of two vectors.
* @param Vec1 First vector
* @param Vec2 Second vector
* @ResultOut Location the result should be placed.
*/
__forceinline void AddVectorVector(const TVector4<T>& Vec1, const TVector4<T>& Vec2, TVector4<T>& ResultOut)
{
	// default to FPU for integer types for now
	ResultOut.x = Vec1.x + Vec2.x;
	ResultOut.y = Vec1.y + Vec2.y;
	ResultOut.z = Vec1.z + Vec2.z;
	ResultOut.w = Vec1.w + Vec2.w;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////// AddVectorVector specializations //////////////////////////////////////////////////////////////////////

template <>
/**
* Component-wise addition of two vectors.
* @param Vec1 First vector
* @param Vec2 Second vector
* @ResultOut Location the result should be placed.
*/
__forceinline void AddVectorVector(const Vector4f& Vec1, const Vector4f& Vec2, Vector4f& ResultOut)
{
	const __m128* V1 = (const __m128*)&Vec1;
	const __m128* V2 = (const __m128*)&Vec2;

	__m128 Sum = _mm_add_ps(*V1, *V2);

	// Store the result
	__m128* Result = (__m128*)&ResultOut;
	*Result = Sum;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
/**
* Component-wise subtraction of two vectors.
* @param Vec1 First vector
* @param Vec2 Second vector
* @ResultOut Location the result should be placed.
*/
__forceinline void SubtractVectorVector(const TVector4<T>& Vec1, const TVector4<T>& Vec2, TVector4<T>& ResultOut)
{
	// default to FPU for integer types for now
	ResultOut.x = Vec1.x - Vec2.x;
	ResultOut.y = Vec1.y - Vec2.y;
	ResultOut.z = Vec1.z - Vec2.z;
	ResultOut.w = Vec1.w - Vec2.w;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////// SubtractVectorVector specializations ////////////////////////////////////////////////////////////////////

template <>
/**
* Component-wise subtraction of two vectors.
* @param Vec1 First vector
* @param Vec2 Second vector
* @ResultOut Location the result should be placed.
*/
__forceinline void SubtractVectorVector(const Vector4f& Vec1, const Vector4f& Vec2, Vector4f& ResultOut)
{
	const __m128* V1 = (const __m128*)&Vec1;
	const __m128* V2 = (const __m128*)&Vec2;

	__m128 Sum = _mm_sub_ps(*V1, *V2);

	// Store the result
	__m128* Result = (__m128*)&ResultOut;
	*Result = Sum;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
/**
* Component-wise multiplication of two vectors.
* @param Vec1 First vector
* @param Vec2 Second vector
* @ResultOut Location the result should be placed.
*/
__forceinline void MultVectorVector(const TVector4<T>& Vec1, const TVector4<T>& Vec2, TVector4<T>& ResultOut)
{
	// default to FPU for integer types for now
	ResultOut.x = Vec1.x * Vec2.x;
	ResultOut.y = Vec1.y * Vec2.y;
	ResultOut.z = Vec1.z * Vec2.z;
	ResultOut.w = Vec1.w * Vec2.w;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////// MultVectorVector specializations ////////////////////////////////////////////////////////////////////////

template <>
/**
* Component-wise multiplication of two vectors.
* @param Vec1 First vector
* @param Vec2 Second vector
* @ResultOut Location the result should be placed.
*/
__forceinline void MultVectorVector(const Vector4f& Vec1, const Vector4f& Vec2, Vector4f& ResultOut)
{
	const __m128* V1 = (const __m128*)&Vec1;
	const __m128* V2 = (const __m128*)&Vec2;

	__m128 Sum = _mm_mul_ps(*V1, *V2);

	// Store the result
	__m128* Result = (__m128*)&ResultOut;
	*Result = Sum;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
/**
* Component-wise division of two vectors.
* @param Vec1 First vector
* @param Vec2 Second vector
* @ResultOut Location the result should be placed.
*/
__forceinline void DivideVectorVector(const TVector4<T>& Vec1, const TVector4<T>& Vec2, TVector4<T>& ResultOut)
{
	// default to FPU for integer types for now
	ResultOut.x = Vec1.x / Vec2.x;
	ResultOut.y = Vec1.y / Vec2.y;
	ResultOut.z = Vec1.z / Vec2.z;
	ResultOut.w = Vec1.w / Vec2.w;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////// DivideVectorVector specializations //////////////////////////////////////////////////////////////////////

template <>
/**
* Component-wise division of two vectors.
* @param Vec1 First vector
* @param Vec2 Second vector
* @ResultOut Location the result should be placed.
*/
__forceinline void DivideVectorVector(const Vector4f& Vec1, const Vector4f& Vec2, Vector4f& ResultOut)
{
	const __m128* V1 = (const __m128*)&Vec1;
	const __m128* V2 = (const __m128*)&Vec2;

	__m128 Sum = _mm_div_ps(*V1, *V2);

	// Store the result
	__m128* Result = (__m128*)&ResultOut;
	*Result = Sum;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* Multiply two quaternions.
* @param Lhs Left operand
* @param Rhs Right operand
* @param ResultOut Location the result should be placed.
*/
__forceinline void MultQuatQuat(const FQuaternion& Lhs, const FQuaternion& Rhs, FQuaternion& ResultOut)
{
	const __m128* MLhs = (const __m128*)&Lhs;
	const __m128* MRhs = (const __m128*)&Rhs;

	// Lhs xxyz * Rhs xwww
	__m128 R1 = _mm_mul_ps(_mm_shuffle_ps(*MLhs, *MLhs, SHUFFLE_PARAM(1, 1, 2, 3)), 
							_mm_shuffle_ps(*MRhs, *MRhs, SHUFFLE_PARAM(1,0,0,0)));

	// Lhs yyzx * Rhs yzxy
	__m128 R2 = _mm_mul_ps(_mm_shuffle_ps(*MLhs, *MLhs, SHUFFLE_PARAM(2, 2, 3, 1)),
		_mm_shuffle_ps(*MRhs, *MRhs, SHUFFLE_PARAM(2, 3, 1, 2)));

	__m128 Sum1 = _mm_add_ps(R1, R2);
	Sum1.m128_f32[0] *= -1.0f; // negate first component


	// _mm_replicate_x_ps is w in quarternion
	__m128 R0 = _mm_mul_ps(_mm_replicate_x_ps(*MLhs), *MRhs);

	// Lhs zzxy * Rhs zyzx
	__m128 R3 = _mm_mul_ps(_mm_shuffle_ps(*MLhs, *MLhs, SHUFFLE_PARAM(3, 3, 1, 2)),
							_mm_shuffle_ps(*MRhs, *MRhs, SHUFFLE_PARAM(3, 2, 3, 1)));

	__m128 Sum2 = _mm_sub_ps(R0, R3);
	__m128 Result = _mm_add_ps(Sum1, Sum2);

	__m128* ResOut = (__m128*)&ResultOut;
	*ResOut = Result;
}	
