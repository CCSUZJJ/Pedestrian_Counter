#include "CvPointUtilities.h"
#include <math.h>

namespace vipnt
{

unsigned int GetSquaredDistance(const CvPoint& PointOI1,const CvPoint& PointOI2)
{
    int XDistance = PointOI1.x - PointOI2.x;
    int YDistance = PointOI1.y - PointOI2.y;
    return (unsigned int)(XDistance*XDistance + YDistance*YDistance);
}

// Return the angle in the interval [-pi,+pi] radians of the vector defined
// by the 2 points and the x-axis.

float GetOrientedAngle(const CvPoint& StartPoint,const CvPoint& EndPoint)
{
    return atan2(static_cast<float>(EndPoint.y - StartPoint.y),static_cast<float>(EndPoint.x - StartPoint.x));
}

const CvPoint operator+(const CvPoint& ObjectOI1, const CvPoint& ObjectOI2)
{
    CvPoint PointOI = ObjectOI1;
    PointOI.x += ObjectOI2.x;
    PointOI.y += ObjectOI2.y;
    return PointOI;
}

const CvPoint operator-(const CvPoint& ObjectOI1, const CvPoint& ObjectOI2)
{
    CvPoint PointOI = ObjectOI1;
    PointOI.x -= ObjectOI2.x;
    PointOI.y -= ObjectOI2.y;
    return PointOI;
}

} // namespace vipnt
