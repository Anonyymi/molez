#include "math.h"

namespace Math
{

// ------------------------------------------------------------------------
// -- GLOBAL MATH FUNCTIONS
// ------------------------------------------------------------------------
vec2 rng_vec2(int xMax, int yMax)
{
	return vec2(
		static_cast<float>(RNG_DIST32(RNG) % xMax),
		static_cast<float>(RNG_DIST32(RNG) % yMax)
	);
}

// ------------------------------------------------------------------------
// -- GLOBAL RANDOM NUMBER GENERATOR
// ------------------------------------------------------------------------
std::mt19937 RNG;
std::uniform_int_distribution<> RNG_DIST8(0, 255);
std::uniform_int_distribution<> RNG_DIST32;

}