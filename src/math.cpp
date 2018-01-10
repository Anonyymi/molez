#include "math.h"

namespace Math
{

	std::mt19937 RNG;
	std::uniform_int_distribution<> RNG_DIST8(0, 255);
	std::uniform_int_distribution<> RNG_DIST32;

}