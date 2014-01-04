#ifndef CVPOINTUTILITIES_H
#define CVPOINTUTILITIES_H

#include "CvPoint.h"

namespace vipnt
{
unsigned int GetSquaredDistance(const CvPoint& PointOI1,const CvPoint& PointOI2);

// Return the angle in the interval [-pi,+pi] radians of the vector defined
// by the 2 points and the x-axis.

float GetOrientedAngle(const CvPoint& StartPoint,const CvPoint& EndPoint);


const CvPoint operator+(const CvPoint& ObjectOI1, const CvPoint& ObjectOI2);
const CvPoint operator-(const CvPoint& ObjectOI1, const CvPoint& ObjectOI2);

} // namespace vipnt

#endif // CVPOINTUTILITIES_H
