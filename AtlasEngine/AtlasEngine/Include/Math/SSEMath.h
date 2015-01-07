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
	* Matrix should be column-vector and stored as column-major.
	* The FMatrix4 and Vector4f must be 16 byte aligned.
	* @param Matrix to multiply
	* @param Vector to multiply
	* @param ResultOUt The resulting calculation will be placed in this variable.
	*/
	__forceinline void MultVectorMatrix(const FMatrix4& Matrix, const Vector4f& Vector, Vector4f& ResultOut)
	{	
		const __m128 V = _mm_load_ps(&Vector.x);
		const __m128 MCols[4] = {
			_mm_load_ps(Matrix.M[0]),
			_mm_load_ps(Matrix.M[1]),
			_mm_load_ps(Matrix.M[2]),
			_mm_load_ps(Matrix.M[3])
		};

		__m128 Result = _mm_mul_ps(MCols[0], _mm_replicate_x_ps(V));
		Result = _mm_madd_ps(MCols[1], _mm_replicate_y_ps(V), Result);
		Result = _mm_madd_ps(MCols[2], _mm_replicate_z_ps(V), Result);
		Result = _mm_madd_ps(MCols[3], _mm_replicate_w_ps(V), Result);

		// Store the result
		_mm_store_ps(&ResultOut.x, Result);
	}

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
}

