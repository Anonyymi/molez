#ifndef AABB_H
#define AABB_H

#include "math.h"

class AABB
{
public:
	AABB(Math::vec2 minP = Math::vec2(-1, -1), Math::vec2 maxP = Math::vec2(1, 1));

	bool collidesXRight(const AABB & other) const;
	bool collidesXLeft(const AABB & other) const;
	bool collidesYUp(const AABB & other) const;
	bool collidesYDown(const AABB & other) const;
	bool collidesY(const AABB & other) const;
	bool collidesX(const AABB & other) const;
	bool collides(const AABB & other) const;

	void setMinP(const Math::vec2 & minP);
	void setMaxP(const Math::vec2 & maxP);
	Math::vec2 getCenterP() const;
	Math::vec2 getMinP() const;
	Math::vec2 getMaxP() const;

	AABB operator+(const Math::vec2 & other) const;
	AABB operator-(const Math::vec2 & other) const;
	AABB operator*(const float f) const;
	AABB operator/(const float f) const;
private:
	Math::vec2 m_minP;
	Math::vec2 m_maxP;
};

#endif // AABB_H