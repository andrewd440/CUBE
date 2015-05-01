#pragma once

#include "../ThirdParty/SFMT/SFMT.h"
#include "../ThirdParty/SFMT/SFMT.c"

/**
* Wrapper class for SFMT (SIMD-oriented Fast Mersenne Twister)
* random number generator.
*/
struct FRandom
{
	/** State for random generator */
	static SFMT_T RandomState;

	/** 
	* Sets the seed for the random number
	* generator. This function must be called
	* before generating any random numbers.
	* @param Seed
	*/
	static void Seed(uint32_t Seed);

	/**
	* Generates a random unsigned 32 bit integer.
	* The random engine must be seeded before making
	* this call with FRandom::Seed()
	* @return A random unsigned 32 bit integer.
	*/
	static uint32_t GenRand();
};

SFMT_T FRandom::RandomState;

__forceinline void FRandom::Seed(uint32_t Seed)
{
	sfmt_init_gen_rand(&RandomState, Seed);
}

__forceinline uint32_t FRandom::GenRand()
{
	return sfmt_genrand_uint32(&RandomState);
}