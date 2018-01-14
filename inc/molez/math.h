#ifndef MATH_H
#define MATH_H

#include <algorithm>
#include <random>
#include <vector>
#include <cstdint>
#include <cmath>

namespace Math
{

	// ------------------------------------------------------------------------
	// -- GLOBAL RANDOM NUMBER GENERATOR
	// ------------------------------------------------------------------------
	extern std::mt19937 RNG;									// RNG instance
	extern std::uniform_int_distribution<> RNG_DIST8;	// RNG distribution, unsigned 8bit
	extern std::uniform_int_distribution<> RNG_DIST32;	// RNG distribution, signed 32bit

	// ------------------------------------------------------------------------
	// -- SIMPLEX NOISE IMPLEMENTATION
	// ------------------------------------------------------------------------
	class SimplexGen
	{
	public:
		SimplexGen(
			uint32_t seed = NULL
		) :
			m_p(256),
			m_rng(),
			m_dist(0, 255)
		{
			reseed(seed);
		}

		inline void reseed(int32_t seed) {
			m_rng.seed(seed);

			for (size_t i = 0; i < 256; i++)
			{
				m_p[i] = m_dist(m_rng);
			}
		}

		inline uint8_t hash(int32_t i) {
			return m_p[static_cast<uint8_t>(i)];
		}

		inline float grad(int32_t hash, float x, float y) {
			int32_t h = hash & 0x3F;  // Convert low 3 bits of hash code
			float u = h < 4 ? x : y;  // into 8 simple gradient directions,
			float v = h < 4 ? y : x;  // and compute the dot product with (x,y).
			return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f*v : 2.0f*v);
		}

		inline float noise(float x, float y)
		{
			// Noise contributions from the three simplex corners
			float n0, n1, n2;

			// Skew coordinates (_), determine hypercubes (b_), hypercube internal coords (i_)
			float F = 0.366025403f;						// F = (sqrt(n + 1) - 1) / n
			float G = 0.211324865f;						// G = ((n + 1) - sqrt(n + 1)) / 6 = F / (1 + 2 * K)
			float x_ = x + (x + y) * F;					// An lattice honeycomb vertex arrangement coords
			float y_ = y + (x + y) * F;
			int xb_ = static_cast<int>(std::floor(x_));	// Skewed unit hypercube
			int yb_ = static_cast<int>(std::floor(y_));

			// Unskew cell origin back to (x,y) space
			float t = static_cast<float>(xb_ + yb_) * G;
			float XI_ = xb_ - t;
			float YI_ = yb_ - t;
			float x0 = x - XI_;							// x,y distances from cell origin
			float y0 = y - YI_;

			// Simplical subdivision, determine which simplex we are in
			int xi1_ = (x0 > y0) ? 1 : 0;
			int yi1_ = (x0 > y0) ? 0 : 1;

			float x1 = x0 - xi1_ + G;					// Offsets for middle corner in (x,y) unskewed coords
			float y1 = y0 - yi1_ + G;
			float x2 = x0 - 1.0f + 2.0f * G;			// Offsets for last corner in (x,y) unskewed coords
			float y2 = y0 - 1.0f + 2.0f * G;

			// Contribution from 1st corner
			float t0 = 0.5f - x0 * x0 - y0 * y0;
			if (t0 < 0.0f)
			{
				n0 = 0.0f;
			}
			else
			{
				t0 *= t0;
				n0 = t0 * t0 * grad(hash(xb_ + hash(yb_)), x0, y0);
			}

			// Contribution from 2nd corner
			float t1 = 0.5f - x1 * x1 - y1 * y1;
			if (t1 < 0.0f)
			{
				n1 = 0.0f;
			}
			else
			{
				t1 *= t1;
				n1 = t1 * t1 * grad(hash(xb_ + xi1_ + hash(yb_ + yi1_)), x1, y1);
			}

			// Contribution from 3rd corner
			float t2 = 0.5f - x2 * x2 - y2 * y2;
			if (t2 < 0.0f)
			{
				n2 = 0.0f;
			}
			else
			{
				t2 *= t2;
				n2 = t2 * t2 * grad(hash(xb_ + 1 + hash(yb_ + 1)), x2, y2);
			}

			// Add contribution from each corner and return final noise value
			// Result is in the sector of -1 to +1
			return 45.23065f * (n0 + n1 + n2);
		}
	private:
		std::vector<uint8_t> m_p;						// Permutation vector
		std::mt19937 m_rng;								// RNG instance
		std::uniform_int_distribution<> m_dist;			// RNG distribution
	};



}

#endif // MATH_H