#include "CvLine.h"

namespace vipnt
{

CvLine::CvLine()
{
}

void CvLine::Reset()
{
    Point1.x = 0;
    Point1.y = 0;
    Point2.x = 0;
    Point2.y = 0;
}

unsigned int CvLine::GetSquaredDistance() const
{
    return vipnt::GetSquaredDistance(Point1,Point2);
}

 void CvLine::SwapPoints()
 {
     CvPoint TempPoint = Point1;
     Point1 = Point2;
     Point2 = TempPoint;
 }

} // namespace vipnt
