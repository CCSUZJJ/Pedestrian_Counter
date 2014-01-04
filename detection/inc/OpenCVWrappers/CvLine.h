#ifndef CVLINE_H
#define CVLINE_H

#include "CvPointUtilities.h"

// TODO : Rename class to LinePiece


namespace vipnt
{

class CvLine
{
public:
    CvLine();
    void Reset();
    unsigned int GetSquaredDistance() const;
    void SwapPoints();

    bool operator == (const CvLine& ObjectOI) const
    {
        return (Point1 == ObjectOI.Point1) && (Point2 == ObjectOI.Point2);
    }

    CvPoint Point1;
    CvPoint Point2;
};

} // namespace vipnt

#endif // CVLINE_H
