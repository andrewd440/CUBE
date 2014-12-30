#pragma once

#include "Matrix4.h"
#include <emmintrin.h>

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
}

