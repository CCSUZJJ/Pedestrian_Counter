#ifndef _POINT_H_INCLUDE
#define _POINT_H_INCLUDE


/*!
 \file  Point.h
 \brief	Header file for Point class.
	\author Traficon
 */

namespace vipnt
{


/*!
	\brief The class CPoint models a 2 dimensional point (x,y).

	Coordinates can have both negative and positive values.
	No range limitations are imposed, except for the implementation limitations (signed 32 bit).
*/

class CPoint
{
private:
    int m_x; //!< the x coordinate of the point
    int m_y; //!< the y coordinate of the point
public:
    explicit CPoint(int xInit = 0, int yInit = 0); //!< constructor, default value is (0,0)
    void Set(int xSet,  int ySet);    //!< Set both coordinates of the point
    int GetX() const {return m_x;}  //!< retrieve the x coordinate
    int GetY() const {return m_y;}  //!< retrieve the y coordinate
    void SetX(int x) {m_x = x;}     //!< set the x coordinate
    void SetY(int y) {m_y = y;}     //!< set the y coordinate
	bool operator!=(const CPoint& p) const {return ((m_x != p.m_x) || (m_y != p.m_y));} //!< compare two points for inequality
	bool operator==(const CPoint& p) const {return ((m_x == p.m_x) && (m_y == p.m_y));} //!< compare two points for equality
	CPoint& operator +=(const CPoint& Other); //!< add another point to the current one
	CPoint& operator -=(const CPoint& Other); //!< subtract another point from the current one
    CPoint& operator *=(const int& v); //!< multiply both coordinates of the point by a certain value (operator *=)
    CPoint& operator /=(const int& v); //!< divide both coordinates of the point by a certain value (operator /=)
};

const CPoint operator+(const CPoint& lhs, const CPoint& rhs); 		//!< add 2 points
const CPoint operator-(const CPoint& lhs, const CPoint& rhs);		//!< subtract 2 points
const CPoint operator*(const CPoint& lhs, const int& rhs);	//!< multiply both coordinates of the point by a certain value
const CPoint operator*(const int& lhs, const CPoint& rhs);	//!< multiply both coordinates of the point by a certain value
const CPoint operator/(const CPoint& lhs, const int& rhs);	//!< divide both coordinates of the point by a certain value

} // namespace vipnt

#endif //_POINT_H_INCLUDE
