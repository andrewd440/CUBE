#pragma once

#include "Matrix4.h"
#include "Quaternion.h"
#include "FMath.h"
#include <emmintrin.h>

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

namespace SSEMath
{
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
		const __m128 V = _mm_load_ps(&Vector.x);
		const __m128* MCols = (const __m128*)&Matrix;

		// Multiply and add each column in the matrix with respective component of the vector
		__m128 Result = _mm_mul_ps(MCols[0], _mm_replicate_x_ps(V));
		Result = _mm_madd_ps(MCols[1], _mm_replicate_y_ps(V), Result);
		Result = _mm_madd_ps(MCols[2], _mm_replicate_z_ps(V), Result);
		Result = _mm_madd_ps(MCols[3], _mm_replicate_w_ps(V), Result);

		// Store the result
		_mm_store_ps(&ResultOut.x, Result);
	}

	__forceinline void MultMatrixMatrix(const FMatrix4& Mat1, const FMatrix4& Mat2, FMatrix4& ResultOut)
	{
		const __m128 M1[4] = {
			_mm_load_ps(Mat1.M[0]),
			_mm_load_ps(Mat1.M[1]),
			_mm_load_ps(Mat1.M[2]),
			_mm_load_ps(Mat1.M[3])
		};

		const __m128 M2[4] = {
			_mm_load_ps(Mat2.M[0]),
			_mm_load_ps(Mat2.M[1]),
			_mm_load_ps(Mat2.M[2]),
			_mm_load_ps(Mat2.M[3])
		};
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
	* Gets the 4-component dot product of two vectors.
	* If a 3-component dot product is required, 0 the w componet
	* of the vectors.
	* @param Vec1 First vector
	* @param Vec2 Second vector
	* @ResultOut Location the result should be placed.
	*/
	__forceinline void Dot4Product(const Vector4f& Vec1, const Vector4f& Vec2, float& ResultOut)
	{
		const __m128 V1 = _mm_load_ps(&Vec1.x);
		const __m128 V2 = _mm_load_ps(&Vec2.x);

		__m128 Dot = _mm_mul_ps(V1, V2);
		Dot = _mm_hadd_ps(Dot, Dot);
		Dot = _mm_hadd_ps(Dot, Dot);

		ResultOut = Dot.m128_f32[0];
	}

	/**
	* Multiply two quaternions.
	* @param Lhs Left operand
	* @param Rhs Right operand
	* @param ResultOut Location the result should be placed.
	*/
	__forceinline void MultQuatQuat(const FQuaternion& Lhs, const FQuaternion& Rhs, FQuaternion& ResultOut)
	{
		__m128 MLhs = _mm_load_ps(&Lhs.w);
		__m128 MRhs = _mm_load_ps(&Rhs.w);


		// Lhs xxyz * Rhs xwww
		__m128 R1 = _mm_mul_ps(_mm_shuffle_ps(MLhs, MLhs, SHUFFLE_PARAM(1, 1, 2, 3)), 
								_mm_shuffle_ps(MRhs, MRhs, SHUFFLE_PARAM(1,0,0,0)));

		// Lhs yyzx * Rhs yzxy
		__m128 R2 = _mm_mul_ps(_mm_shuffle_ps(MLhs, MLhs, SHUFFLE_PARAM(2, 2, 3, 1)),
			_mm_shuffle_ps(MRhs, MRhs, SHUFFLE_PARAM(2, 3, 1, 2)));

		__m128 Sum1 = _mm_add_ps(R1, R2);
		Sum1.m128_f32[0] *= -1.0f; // negate first component


		// _mm_replicate_x_ps is w in quarternion
		__m128 R0 = _mm_mul_ps(_mm_replicate_x_ps(MLhs), MRhs);

		// Lhs zzxy * Rhs zyzx
		__m128 R3 = _mm_mul_ps(_mm_shuffle_ps(MLhs, MLhs, SHUFFLE_PARAM(3, 3, 1, 2)),
								_mm_shuffle_ps(MRhs, MRhs, SHUFFLE_PARAM(3, 2, 3, 1)));

		__m128 Sum2 = _mm_sub_ps(R0, R3);
		__m128 Result = _mm_add_ps(Sum1, Sum2);

		_mm_store_ps(&ResultOut.w, Result);
	}	


}

