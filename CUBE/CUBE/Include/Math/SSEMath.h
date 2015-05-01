#pragma once

#include <emmintrin.h>
#include "Memory\MemoryUtil.h"

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

#define LoadVector(Vec4) _mm_load_ps(Vec4)
#define LoadVectori(Vec4i) _mm_load_si128((__m128i const*)Vec4i)

#define StoreVector(Memory, Vector) _mm_store_ps(Memory, Vector)
#define StoreVectori(Memory, Vector) _mm_store_si128 ((__m128i*)Memory, Vector)

/**
* Matrix - Vector multiplication with SSE SIMD instructions.
* Matrix should be column-vector and stored as column-major.
* The FMatrix4 and Vector4f must be 16 byte aligned.
* @param Matrix - Column major 4x4 matrix to multiply
* @param Vector - Vector to multiply
* @param ResultOut - A 4 component vector where the resulting calculation will be placed.
*/
__forceinline void MultVectorMatrix(const float* Matrix4, const float* Vector4, float* ResultOut)
{
	const __m128 V = LoadVector(Vector4);
	const __m128 MCol0 = LoadVector(Matrix4);
	const __m128 MCol1 = LoadVector(Matrix4 + 4);
	const __m128 MCol2 = LoadVector(Matrix4 + 8);
	const __m128 MCol3 = LoadVector(Matrix4 + 12);

	// Multiply and add each column in the matrix with respective component of the vector
	__m128 Mult = _mm_mul_ps(MCol0, _mm_replicate_x_ps(V));
	Mult = _mm_madd_ps(MCol1, _mm_replicate_y_ps(V), Mult);
	Mult = _mm_madd_ps(MCol2, _mm_replicate_z_ps(V), Mult);
	Mult = _mm_madd_ps(MCol3, _mm_replicate_w_ps(V), Mult);

	// Store the result
	StoreVector(ResultOut, Mult);
}

/**
* Converts 4 32-bit floating-point values to 4 32-bit 
* integers.
* @param Floats - The value to convert.
* @param Results - Memory location to place the converted values.
*/
__forceinline void Vector4FloatToInt(const float* Floats, int32_t* Results)
{
	const __m128 FloatVec = LoadVector(Floats);
	const __m128i Ints = _mm_cvttps_epi32(FloatVec);
	StoreVectori(Results, Ints);
}

/**
* Converts 4 32-bit signied integer values to 4 32-bit
* floating-point values.
* @param Ints - The value to convert.
* @param Results - Memory location to place the converted values.
*/
__forceinline void Vector4IntToFloat(const int32_t* Ints, float* Results)
{
	const __m128i IntVec = LoadVectori(Ints);
	const __m128 Floats = _mm_cvtepi32_ps(IntVec);
	StoreVector(Results, Floats);
}

/**
* 4x4 Matrix multiplication.
* @param Mat1 - First matrix
* @param Mat2 - Second matrix
* @param ResultOut - Location of the result.
*/
__forceinline void MultMatrixMatrix(const float* Mat1, const float* Mat2, float* ResultOut)
{
	const __m128* M1 = (const __m128*)Mat1;
	const __m128* M2 = (const __m128*)Mat2;


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
	__m128* Result = (__m128*)ResultOut;
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
__forceinline void AddMatrixMatrix(const float* Mat1, const float* Mat2, float* ResultOut)
{
	const __m128* M1 = (const __m128*)Mat1;
	const __m128* M2 = (const __m128*)Mat2;

	const __m128 Col0 = _mm_add_ps(M1[0], M2[0]);
	const __m128 Col1 = _mm_add_ps(M1[1], M2[1]);
	const __m128 Col2 = _mm_add_ps(M1[2], M2[2]);
	const __m128 Col3 = _mm_add_ps(M1[3], M2[3]);

	__m128* Result = (__m128*)ResultOut;
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
__forceinline void MultComponentMatrixMatrix(const float* Mat1, const float* Mat2, float* ResultOut)
{
	const __m128* M1 = (const __m128*)Mat1;
	const __m128* M2 = (const __m128*)Mat2;

	const __m128 Col0 = _mm_mul_ps(M1[0], M2[0]);
	const __m128 Col1 = _mm_mul_ps(M1[1], M2[1]);
	const __m128 Col2 = _mm_mul_ps(M1[2], M2[2]);
	const __m128 Col3 = _mm_mul_ps(M1[3], M2[3]);

	__m128* Result = (__m128*)ResultOut;
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
* @param Vec1 - A 4-component vector, must be a pointer to the first component
* @param Vec2 - A 4-component vector, must be a pointer to the first component
* @ResultOut - Location to place result
*/
__forceinline void Dot4Product(const T* Vec1, const T* Vec2, T* ResultOut)
{
	// default to FPU for unspecialized types for now
	ResultOut = Vec1[0] * Vec2[0] + Vec1[1] * Vec2[1] + Vec1[2] * Vec2[2] + Vec1[3] * Vec2[3];
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////// Dot4Product specializations //////////////////////////////////////////////////////////////////////////

template <>
__forceinline void Dot4Product(const float* Vec1, const float* Vec2, float* ResultOut)
{
	const __m128 V1 = LoadVector(Vec1);
	const __m128 V2 = LoadVector(Vec2);

	__m128 Dot = _mm_mul_ps(V1, V2);
	Dot = _mm_hadd_ps(Dot, Dot);
	Dot = _mm_hadd_ps(Dot, Dot);

	*ResultOut = Dot.m128_f32[0];
}

template <>
__forceinline void Dot4Product(const int32_t* Vec1, const int32_t* Vec2, int32_t* ResultOut)
{
	const __m128i V1 = LoadVectori(Vec1);
	const __m128i V2 = LoadVectori(Vec2);

	__m128i Dot = _mm_mullo_epi32(V1, V2);
	Dot = _mm_hadd_epi32(Dot, Dot);
	Dot = _mm_hadd_epi32(Dot, Dot);

	*ResultOut = Dot.m128i_i32[0];
}

template <uint32_t Count>
/**
* Computes the dot product for PrimaryVec with each of the SecondaryVecs passed in and adds all the
* results together and places them in ResultOut.
*/
__forceinline void Dot4ProductBatchAdd(const float* PrimaryVec, const float SecondaryVecs[Count][4], float* ResultOut)
{
	const register __m128 Primary = LoadVector(PrimaryVec);
	__m128 Secondaries[Count];

	for (uint32_t i = 0; i < Count; i++)
	{
		Secondaries[i] = LoadVector(SecondaryVecs[i]);
		Secondaries[i] = _mm_mul_ps(Primary, Secondaries[i]);
	}

	for (uint32_t i = 1; i < Count; i++)
	{
		Secondaries[0] = _mm_add_ps(Secondaries[0], Secondaries[i]);
	}

	__m128 Sum = _mm_hadd_ps(Secondaries[0], Secondaries[0]);
	Sum = _mm_hadd_ps(Secondaries[0], Secondaries[0]);

	*ResultOut = Sum.m128_f32[0];
}

template <uint32_t Count>
/**
* Computes the dot product for PrimaryVec with each of the SecondaryVecs passed in and places
* each result in ResultsOut.
*/
__forceinline void Dot4ProductBatch(const float* PrimaryVec, const float SecondaryVecs[Count][4], float ResultsOut[Count])
{
	const __m128 Primary = LoadVector(PrimaryVec);
	__m128 Secondaries[Count];

	for (uint32_t i = 0; i < Count; i++)
	{
		Secondaries[i] = LoadVector(SecondaryVecs[i]);
		Secondaries[i] = _mm_mul_ps(Primary, Secondaries[i]);
	}
	
	for (uint32_t i = 0; i < Count; i++)
	{
		Secondaries[i] = _mm_hadd_ps(Secondaries[i], Secondaries[i]);
		Secondaries[i] = _mm_hadd_ps(Secondaries[i], Secondaries[i]);
		ResultsOut[i] = Secondaries[i].m128_f32[0];
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
/**
* Component-wise addition of two vectors.
* @param Vec1 - A 4-component vector, must be a pointer to the first component
* @param Vec2 - A 4-component vector, must be a pointer to the first component
* @ResultOut  A 4-component vector where the result should be placed.
*/
__forceinline void AddVectorVector(const T* Vec1, const T* Vec2, T* ResultOut)
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
__forceinline void AddVectorVector(const float* Vec1, const float* Vec2, float* ResultOut)
{
	const __m128 V1 = LoadVector(Vec1);
	const __m128 V2 = LoadVector(Vec2);

	__m128 Sum = _mm_add_ps(V1, V2);

	// Store the result
	StoreVector(ResultOut, Sum);
}

template <>
__forceinline void AddVectorVector(const int32_t* Vec1, const int32_t* Vec2, int32_t* ResultOut)
{
	const __m128i V1 = LoadVectori(Vec1);
	const __m128i V2 = LoadVectori(Vec2);

	__m128i Sum = _mm_add_epi32(V1, V2);

	// Store the result
	StoreVectori(ResultOut, Sum);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
/**
* Component-wise subtraction of two vectors.
* @param Vec1 - A 4-component vector, must be a pointer to the first component
* @param Vec2 - A 4-component vector, must be a pointer to the first component
* @ResultOut  A 4-component vector where the result should be placed.
*/
__forceinline void SubtractVectorVector(const T* Vec1, const T* Vec2, T* ResultOut)
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
__forceinline void SubtractVectorVector(const float* Vec1, const float* Vec2, float* ResultOut)
{
	const __m128 V1 = LoadVector(Vec1);
	const __m128 V2 = LoadVector(Vec2);

	__m128 Sum = _mm_sub_ps(V1, V2);

	// Store the result
	StoreVector(ResultOut, Sum);
}

template <>
__forceinline void SubtractVectorVector(const int32_t* Vec1, const int32_t* Vec2, int32_t* ResultOut)
{
	const __m128i V1 = LoadVectori(Vec1);
	const __m128i V2 = LoadVectori(Vec2);

	__m128i Sum = _mm_sub_epi32(V1, V2);

	// Store the result
	StoreVectori(ResultOut, Sum);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
/**
* Component-wise multiplication of two vectors.
* @param Vec1 - A 4-component vector, must be a pointer to the first component
* @param Vec2 - A 4-component vector, must be a pointer to the first component
* @ResultOut  A 4-component vector where the result should be placed.
*/
__forceinline void MultVectorVector(const T* Vec1, const T* Vec2, T* ResultOutt)
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
__forceinline void MultVectorVector(const float* Vec1, const float* Vec2, float* ResultOut)
{
	const __m128 V1 = LoadVector(Vec1);
	const __m128 V2 = LoadVector(Vec2);

	__m128 Sum = _mm_mul_ps(V1, V2);

	// Store the result
	StoreVector(ResultOut, Sum);
}

template <>
__forceinline void MultVectorVector(const int32_t* Vec1, const int32_t* Vec2, int32_t* ResultOut)
{
	const __m128i V1 = LoadVectori(Vec1);
	const __m128i V2 = LoadVectori(Vec2);

	__m128i Sum = _mm_mullo_epi32(V1, V2);

	// Store the result
	StoreVectori(ResultOut, Sum);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
/**
* Component-wise division of two vectors.
* @param Vec1 - A 4-component vector, must be a pointer to the first component
* @param Vec2 - A 4-component vector, must be a pointer to the first component
* @ResultOut  A 4-component vector where the result should be placed.
*/
__forceinline void DivideVectorVector(const T* Vec1, const T* Vec2, T* ResultOut)
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
__forceinline void DivideVectorVector(const float* Vec1, const float* Vec2, float* ResultOut)
{
	const __m128 V1 = LoadVector(Vec1);
	const __m128 V2 = LoadVector(Vec2);

	const __m128 Sum = _mm_div_ps(V1, V2);

	// Store the result
	StoreVector(ResultOut, Sum);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class FQuaternion;
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
