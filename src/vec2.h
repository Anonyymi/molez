#ifndef VEC2_H
#define VEC2_H

#include <string>
#include <cmath>
#include <cstdio>

namespace Math
{

	struct vec2;
	extern vec2 operator+(const float f, const vec2 &other);
	extern vec2 operator-(const float f, const vec2 &other);
	extern vec2 operator*(const float f, const vec2 &other);
	extern vec2 operator/(const float f, const vec2 &other);

	struct vec2
	{
		float x, y;

		vec2(
			float x_ = 0.0f,
			float y_ = 0.0f
		) :
			x(x_),
			y(y_)
		{

		}

		std::string toString() const
		{
			char buffer[255];
			snprintf(buffer, 255, "vec2[%+.2f %+.2f]", x, y);
			return std::string(buffer);
		}

		bool operator==(const vec2 &other) const
		{
			return x == other.x && y == other.y;
		}

		bool operator!=(const vec2 &other) const
		{
			return !(*this == other);
		}

		vec2 &operator+=(const vec2 &other)
		{
			x += other.x;
			y += other.y;
			return *this;
		}

		vec2 &operator+=(const float &f)
		{
			x += f;
			y += f;
			return *this;
		}

		vec2 &operator-=(const vec2 &other)
		{
			x -= other.x;
			y -= other.y;
			return *this;
		}

		vec2 &operator-=(const float &f)
		{
			x -= f;
			y -= f;
			return *this;
		}

		vec2 &operator*=(const float &f)
		{
			x *= f;
			y *= f;
			return *this;
		}

		vec2 &operator/=(const float &f)
		{
			x /= f;
			y /= f;
			return *this;
		}

		vec2 operator+(const vec2 &other) const
		{
			return vec2(x + other.x, y + other.y);
		}

		vec2 operator+(const float &f) const
		{
			return vec2(x + f, y + f);
		}

		vec2 operator-(const vec2 &other) const
		{
			return vec2(x - other.x, y - other.y);
		}

		vec2 operator-(const float &f) const
		{
			return vec2(x - f, y - f);
		}

		vec2 operator*(const float &f) const
		{
			return vec2(x * f, y * f);
		}

		vec2 operator/(const float &f) const
		{
			return vec2(x / f, y / f);
		}

		float length() const
		{
			return std::sqrt(x * x + y * y);
		}

		vec2 normalize() const
		{
			float l = 1.0f / length();
			return vec2(x * l, y * l);
		}

		vec2 negate() const
		{
			return vec2(-x, -y);
		}

		vec2 round() const
		{
			return vec2(std::round(x), std::round(y));
		}

		vec2 floor() const
		{
			return vec2(std::floor(x), std::floor(y));
		}

	};

	inline vec2 operator+(const float f, const vec2 &other)
	{
		return other + f;
	}

	inline vec2 operator-(const float f, const vec2 &other)
	{
		return other - f;
	}

	inline vec2 operator*(const float f, const vec2 &other)
	{
		return other * f;
	}

	inline vec2 operator/(const float f, const vec2 &other)
	{
		return vec2(f / other.x, f / other.y);
	}

}

#endif // VEC2_H