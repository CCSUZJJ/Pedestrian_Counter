#ifndef LINEPIECEFUNCTIONS_H
#define LINEPIECEFUNCTIONS_H

#include "CvLine.h"

// Determine which placement of the line-pieces results
// in a closest distance of the linepiece-extremities
//
//                    +------+  LineOI
// +------+
//        |<--------->|   Distance1
//
//  +------+  LineOI
//                     +------+
//         |<--------->|   Distance2
//
// Remark : LineOI will be swapped by the function if appropriate
//          Check that lines don't overlap !
//
// Exit : true  = Distance1 <LineOI><ReferenceLine>
//        false = Distance2 <ReferenceLine><LineOI>
//        Distance = SquaredDistance between the 2 closest points (according the returned placement)

bool DetermineOptimalPlacement(const vipnt::CvLine& ReferenceLine,vipnt::CvLine& LineOI,unsigned int& Distance);


#endif // LINEPIECEFUNCTIONS_H
