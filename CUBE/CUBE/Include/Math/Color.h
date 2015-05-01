#pragma once
#include <cstdint>

#include <algorithm>
#include <cstdio>

#include "Misc\Assertions.h"

/* Class for representing and manipulating RGB values */
class FColor
{
public:
	/**
	* Default constructor. Creates a black color.
	*/
	FColor();

	/**
	* Constructs a color with given r, g, B values.
	* @param R - Red component
	* @param G - Green component
	* @param B - Blue component
	*/
	FColor(float R, float G, float B);

	/**
	* Constructs a color with given r, g, B values.
	* @param R - Red component
	* @param G - Green component
	* @param B - Blue component
	* @param A - Transparency component
	*/
	FColor(float R, float G, float B, float A);

	/**
	* Default copy ctor.
	*/
	FColor(const FColor& Other) = default;

	/**
	* Default destructor.
	*/
	~FColor() = default;

	/**
	* Default copy assignment
	* @param rhs - FColor to be copied
	* @return The resulting color
	*/
	FColor& operator=(const FColor& rhs) = default;

	/**
	* Performs component-wise addition of colors.
	* @param rhs - FColor to be added
	* @return The resulting color
	*/
	FColor& operator+=(const FColor& rhs);

	/**
	* Performs component-wise multiplication of colors.
	* @param rhs - FColor to be multiplied
	* @return The resulting color
	*/
	FColor& operator*=(const FColor& rhs);

	/**
	* Performs component-wise multiplication of with a scalar.
	* @param scalar - Unit to multiply by
	* @return The resulting color
	*/
	FColor& operator*=(const float& scalar);

	/**
	* Performs component-wise division with a scalar.
	* @param scalar - Unit to divide by
	* @return The resulting color
	*/
	FColor& operator/=(const float& scalar);

	/**
	* Checks for color equality.
	*/
	bool operator==(const FColor& rhs) const;

	/**
	* Checks for color inequality.
	*/
	bool operator!=(const FColor& rhs) const;

	/**
	* Overload of subscript operators for R = 0, G = 1, B = 2.
	*/
	float& operator[](uint32_t idx);

	/**
	* Overload of subscript operators for R = 0, G = 1, B = 2.
	*/
	const float& operator[](uint32_t idx) const;

	/**
	* Clamps all color components larger than 1.0.
	*/
	FColor& Clamp();

	/**
	* Composes a string representation of this color.
	*/
	const std::string ToString() const;

	float R;
	float G;
	float B;
	float A;

	static const FColor Red;
	static const FColor Green;
	static const FColor Blue;
	static const FColor White;
	static const FColor Black;
};

/////////////////////////////////////////////////////////////////////
//////////////// Inlined Member Functions ///////////////////////////
/////////////////////////////////////////////////////////////////////

inline FColor::FColor()
	: R(0.0f), G(0.0f), B(0.0f), A(1.0f)
{

}

inline FColor::FColor(float R, float G, float B)
	: R(R), G(G), B(B), A(1.0f)
{

}

inline FColor::FColor(float R, float G, float B, float A)
	: R(R), G(G), B(B), A(A)
{

}

inline FColor& FColor::operator+=(const FColor& rhs)
{
	R += rhs.R;
	G += rhs.G;
	B += rhs.B;

	return *this;
}


inline FColor& FColor::operator*=(const FColor& rhs)
{
	R *= rhs.R;
	G *= rhs.G;
	B *= rhs.B;

	return *this;
}

inline FColor& FColor::operator*=(const float& scalar)
{
	R *= scalar;
	G *= scalar;
	B *= scalar;

	return *this;
}

inline FColor& FColor::operator /= (const float& scalar)
{
	R /= scalar;
	G /= scalar;
	B /= scalar;

	return *this;
}

inline bool FColor::operator==(const FColor& rhs) const
{
	for (uint8_t i = 0; i < 3; i++)
		if ((*this)[i] != rhs[i])
			return false;
	return true;
}

inline bool FColor::operator!=(const FColor& rhs) const
{
	return !(*this == rhs);
}

inline float& FColor::operator[](uint32_t idx)
{
	ASSERT(idx >= 0 && idx < 4 && "Index for FColor out of range");

	if (idx == 0)
		return R;
	else if (idx == 1)
		return G;
	else if (idx == 2)
		return B;
	else
		return A;
}

inline const float& FColor::operator[](uint32_t idx) const
{
	ASSERT(idx >= 0 && idx < 4 && "Index for FColor out of range");

	if (idx == 0)
		return R;
	else if (idx == 1)
		return G;
	else if (idx == 2)
		return B;
	else
		return A;
}

#ifdef min
#define newmin min
#undef min
#endif

inline FColor& FColor::Clamp()
{
	R = std::min(R, 1.f);
	G = std::min(G, 1.f);
	B = std::min(B, 1.f);
	
	return *this;
}

#ifdef newmin
#define min newmin
#undef newmin
#endif

inline const std::string FColor::ToString() const
{
	char buffer[100];

#if NDEBUG
	sprintf_s(buffer, "FColor: r:%.3f g:%.3f b:%.3f a:%.3f", R, G, B, A);
#else
	int32_t n = sprintf_s(buffer, "FColor: r:%.3f g:%.3f b:%.3f a:%.3f", R, G, B, A);
	ASSERT(n >= 0 && "sprintf failed to write string.");
#endif

	return std::string(buffer);
}

/////////////////////////////////////////////////////////////////////
//////////////// Non-member Functions ///////////////////////////////
/////////////////////////////////////////////////////////////////////

/**
* Performs component-wise multiplication of colors
* @param lhs - Left operand
* @param rhs - Right operand
* @return Resulting color
*/
inline FColor operator*(FColor lhs, const FColor& rhs)
{
	lhs *= rhs;
	return lhs;
}

/**
* Performs scalar multiplication of a color
* @param lhs - Left operand
* @param scalar - Right operand
* @return Resulting color
*/
inline FColor operator*(FColor lhs, const float& scalar)
{
	lhs *= scalar;
	return lhs;
}

/**
* Performs scalar multiplication of a color
* @param scalar - Right operand
* @param rhs - Left operand
* @return Resulting color
*/
inline FColor operator*(const float& scalar, FColor rhs)
{
	rhs *= scalar;
	return rhs;
}

/**
* Performs component-wise addition of colors
* @param lhs - Left operand
* @param rhs - Right operand
* @return Resulting color
*/
inline FColor operator+(FColor lhs, const FColor& rhs)
{
	lhs += rhs;
	return lhs;
}