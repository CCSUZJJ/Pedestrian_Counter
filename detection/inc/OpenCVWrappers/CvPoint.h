#ifndef CVPOINT_H
#define CVPOINT_H

#include <opencv/cv.h> // includes OpenCV definitions

namespace vipnt
{
class CvPoint : public ::CvPoint
{
public :

    // Operators

    CvPoint& operator +=(const CvPoint& ObjectOI)
    {
        x += ObjectOI.x;
        y += ObjectOI.y;
        return *this;
    }

    CvPoint& operator -=(const CvPoint& ObjectOI)
    {
        x -= ObjectOI.x;
        y -= ObjectOI.y;
        return *this;
    }

    bool operator ==(const CvPoint& ObjectOI) const
    {
        return (x == ObjectOI.x) && (y == ObjectOI.y);
    }

    bool operator !=(const CvPoint& ObjectOI) const
    {
        return (x != ObjectOI.x) || (y != ObjectOI.y);
    }

};



} // namespace vipnt


#endif // CVPOINT_H
