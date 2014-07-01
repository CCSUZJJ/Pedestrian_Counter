#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "OpenCvUtilities.h"

struct LineSegment{
    cv::Point first;
    cv::Point second;

    LineSegment(cv::Point p1, cv::Point p2):first(p1), second(p2){}
};

class Geometry{
    public:
        Geometry(){}
        ~Geometry(){}

        static constexpr double EPSILON = 0.000001;

        static double crossProduct(cv::Point a, cv::Point b);
        static bool whatSideOfLine(LineSegment a, cv::Point b);

        static bool doBBoxesIntersect(cv::Rect a, cv::Rect b);
        static bool isPointOnLine(LineSegment a, cv::Point b);
        static bool isPointRightofLine(LineSegment a, cv::Point b);
        static bool lineSegmentTouchesOrCrossesLine(LineSegment a, LineSegment b);
        static bool doLinesIntersect(LineSegment a, LineSegment b);
};

#endif
