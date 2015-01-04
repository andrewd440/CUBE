#pragma once

#include "Matrix4.h"
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
	* The FMatrix4 and Vector4f must be 16 byte aligned.
	* @param Matrix to multiply
	* @param Vector to multiply
	* @param ResultOUt The resulting calculation will be placed in this variable.
	*/
	__forceinline void MultVectorMatrix(const FMatrix4& Matrix, const Vector4f& Vector, Vector4f& ResultOut)
	{	
		const __m128 V = _mm_load_ps(&Vector.x);
		const __m128 MRows[4] = {
			_mm_load_ps(Matrix.M[0]),
			_mm_load_ps(Matrix.M[1]),
			_mm_load_ps(Matrix.M[2]),
			_mm_load_ps(Matrix.M[3])
		};

		// Multiply each row with the vector
		const __m128 V0 = _mm_mul_ps(MRows[0], V);
		const __m128 V1 = _mm_mul_ps(MRows[1], V);
		const __m128 V2 = _mm_mul_ps(MRows[2], V);
		const __m128 V3 = _mm_mul_ps(MRows[3], V);

		// Add the totals for each vector component
		const __m128 Sum0 = _mm_hadd_ps(V0, V1);
		const __m128 Sum1 = _mm_hadd_ps(V2, V3);
		const __m128 Result = _mm_hadd_ps(Sum0, Sum1);

		// Store the result
		_mm_store_ps(&ResultOut.x, Result);
	}

	__forceinline void MultMatrixMatrix(const FMatrix4& Mat1, const FMatrix4& Mat2, FMatrix4& ResultOut)
	{
		const __m128* M1 = (const __m128*)&Mat1;
		const __m128* M2 = (const __m128*)&Mat2;
		__m128* Result = (__m128*)&ResultOut;

		// Mat1[0] * Mat2
		__m128 R0 = _mm_mul_ps(_mm_replicate_x_ps(M1[0]), M2[0]);
		R0 = _mm_madd_ps(_mm_replicate_y_ps(M1[0]), M2[1], R0);
		R0 = _mm_madd_ps(_mm_replicate_z_ps(M1[0]), M2[2], R0);
		R0 = _mm_madd_ps(_mm_replicate_w_ps(M1[0]), M2[3], R0);

		// Mat1[1] * Mat2
		__m128 R1 = _mm_mul_ps(_mm_replicate_x_ps(M1[1]), M2[0]);
		R1 = _mm_madd_ps(_mm_replicate_y_ps(M1[1]), M2[1], R1);
		R1 = _mm_madd_ps(_mm_replicate_z_ps(M1[1]), M2[2], R1);
		R1 = _mm_madd_ps(_mm_replicate_w_ps(M1[1]), M2[3], R1);

		// Mat1[2] * Mat2
		__m128 R2 = _mm_mul_ps(_mm_replicate_x_ps(M1[2]), M2[0]);
		R2 = _mm_madd_ps(_mm_replicate_y_ps(M1[2]), M2[1], R2);
		R2 = _mm_madd_ps(_mm_replicate_z_ps(M1[2]), M2[2], R2);
		R2 = _mm_madd_ps(_mm_replicate_w_ps(M1[2]), M2[3], R2);

		// Mat1[3] * Mat2
		__m128 R3 = _mm_mul_ps(_mm_replicate_x_ps(M1[3]), M2[0]);
		R3 = _mm_madd_ps(_mm_replicate_y_ps(M1[3]), M2[1], R3);
		R3 = _mm_madd_ps(_mm_replicate_z_ps(M1[3]), M2[2], R3);
		R3 = _mm_madd_ps(_mm_replicate_w_ps(M1[3]), M2[3], R3);

		// Store the result
		Result[0] = R0;
		Result[1] = R1;
		Result[2] = R2;
		Result[3] = R3;
	}
}

