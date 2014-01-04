#include "LinePieceFunctions.h"
#include "CvPointUtilities.h"

// Determine which placement of the line-pieces results
// in a closest distance of the linepiece-extremities
//
//                    +------+  ReferenceLine
// +------+ LineOI
//        |<--------->|   Left placement
//
//  +------+  ReferenceLine
//                     +------+ LineOI
//         |<--------->|   Right placement
//
// Remark : LineOI will be swapped by the function if appropriate
//          Check that lines don't overlap !
//
// Exit : true  = Left placement  <LineOI><ReferenceLine>
//        false = Right placement <ReferenceLine><LineOI>
//        Distance = SquaredDistance between the 2 closest points (according the returned placement)

bool DetermineOptimalPlacement(const vipnt::CvLine& ReferenceLine,vipnt::CvLine& LineOI,unsigned int& Distance)
{
    bool LeftSwapped = false;
    unsigned int DistanceLeft = GetSquaredDistance(ReferenceLine.Point1,LineOI.Point2);
    unsigned int TempDistance = GetSquaredDistance(ReferenceLine.Point1,LineOI.Point1);
    if (TempDistance < DistanceLeft)
    {
        LeftSwapped = true;
        DistanceLeft = TempDistance;
    }

    bool RightSwapped = false;
    unsigned int DistanceRight = GetSquaredDistance(ReferenceLine.Point2,LineOI.Point1);
    TempDistance = GetSquaredDistance(ReferenceLine.Point2,LineOI.Point2);
    if (TempDistance < DistanceRight)
    {
        RightSwapped = true;
        DistanceRight = TempDistance;
    }

    bool LeftOrientation = (DistanceLeft < DistanceRight);
    if (LeftOrientation)
    {
        // Left

        Distance = DistanceLeft;
        if (LeftSwapped) LineOI.SwapPoints();
    }
    else
    {
        // Right

        Distance = DistanceRight;
        if (RightSwapped) LineOI.SwapPoints();
    }

    return LeftOrientation;
}
