#include "aabb.h"
#include <algorithm>

AABB::AABB(Math::vec2 minP, Math::vec2 maxP) :
	m_minP(minP),
	m_maxP(maxP)
{
	m_minP = Math::vec2(std::min(minP.x, maxP.x), std::min(minP.y, maxP.y));
	m_maxP = Math::vec2(std::max(minP.x, maxP.x), std::max(minP.y, maxP.y));
}

bool AABB::collidesXRight(const AABB & other) const
{
	return  (m_minP.x <= other.getMaxP().x);
}

bool AABB::collidesXLeft(const AABB & other) const
{
	return (m_maxP.x >= other.getMinP().x);
}

bool AABB::collidesYUp(const AABB & other) const
{
	return (m_minP.y <= other.getMaxP().y);
}

bool AABB::collidesYDown(const AABB & other) const
{
	return (m_maxP.y >= other.getMinP().y);
}

bool AABB::collidesX(const AABB & other) const
{
	return (collidesXRight(other) && collidesXLeft(other));
}

bool AABB::collidesY(const AABB & other) const
{
	return (collidesYUp(other) && collidesYDown(other));
}

bool AABB::collides(const AABB & other) const
{
	return (collidesX(other) || collidesY(other));
}

void AABB::setMinP(const Math::vec2 & minP)
{
	m_minP = minP;
}

void AABB::setMaxP(const Math::vec2 & maxP)
{
	m_maxP = maxP;
}

Math::vec2 AABB::getCenterP() const
{
	return 0.5f * (m_minP + m_maxP);
}

Math::vec2 AABB::getMinP() const
{
	return m_minP;
}

Math::vec2 AABB::getMaxP() const
{
	return m_maxP;
}

AABB AABB::operator+(const Math::vec2 & other) const
{
	return AABB(m_minP + other, m_maxP + other);
}

AABB AABB::operator-(const Math::vec2 & other) const
{
	return AABB(m_minP - other, m_maxP - other);
}

AABB AABB::operator*(const float f) const
{
	return AABB(m_minP * f, m_maxP * f);
}

AABB AABB::operator/(const float f) const
{
	return AABB(m_minP / f, m_maxP / f);
}