#include "Point.h"

namespace vipnt
{

CPoint::CPoint(int xInit, int yInit)
{
    Set(xInit, yInit);
} // end of constructor

void CPoint::Set(int xSet,  int ySet)
{
    m_x  = xSet;
    m_y  = ySet;
} //end of Set

CPoint& CPoint::operator+=(const CPoint& Other)
{
	m_x += Other.m_x;
	m_y += Other.m_y;
	return *this;
} // end of operator+=

CPoint& CPoint::operator-=(const CPoint& Other)
{
	m_x -= Other.m_x;
	m_y -= Other.m_y;
	return *this;
} // end of operator-=

CPoint& CPoint::operator*=(const int& v)
{
	m_x *= v;
	m_y *= v;
	return *this;
} // end of operator*=

/*!
	Since dividing by 0 is not allowed, the result is undefined.
	Therefor in such case the function call will return the original value, as if we divided by 1.
*/
CPoint& CPoint::operator/=(const int& v)
{
    int val = v?v:static_cast<int>(1);
	m_x /= val;
	m_y /= val;

	return *this;
} // end of operator/=

const CPoint operator+(const CPoint& lhs, const CPoint& rhs)
{
	CPoint ret(lhs);
	ret += rhs;
	return ret;
} // end of operator+

const CPoint operator-(const CPoint& lhs, const CPoint& rhs)
{
	CPoint ret(lhs);
	ret -= rhs;
	return ret;
} // end of operator-

const CPoint operator*(const CPoint& lhs, const int& rhs)
{
	CPoint ret(lhs);
	ret *= rhs;
	return ret;
} // end of operator*

const CPoint operator*(const int& lhs, const CPoint& rhs)
{
	return rhs * lhs;
} // end of operator*

/*!
    Since dividing by 0 is not allowed, the result is undefined.
    Therefor in such case the function call will return the original value, as if we divided by 1.
*/
const CPoint operator/(const CPoint& lhs, const int& v)
{
    CPoint ret(lhs);
    ret /= v;
    return ret;
} // end of operator/

} // namespace vipnt

