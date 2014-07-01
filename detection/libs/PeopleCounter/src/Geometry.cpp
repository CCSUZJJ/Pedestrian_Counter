#include "Geometry.h"
#include <math.h>

double Geometry::crossProduct(cv::Point a, cv::Point b){
    return a.x*b.y - b.x*a.y;
}

bool Geometry::whatSideOfLine(LineSegment a, cv::Point b){
    double d = (b.x-a.first.x)*(a.second.y-a.first.y)-(b.y-a.first.y)*(a.second.x-a.first.x);
    return (d > 0);
}

bool Geometry::doBBoxesIntersect(cv::Rect a, cv::Rect b){
    return (a.x <= (b.x+b.width))
        && ((a.x+a.width) >= b.x)
        && (a.y <= (b.y+b.height))
        && ((a.y+a.height) >= b.y);
}

bool Geometry::isPointOnLine(LineSegment a, cv::Point b){
    LineSegment aTmp = LineSegment(cv::Point(0,0),
                                   cv::Point(a.second.x-a.first.x,a.second.y-a.first.y));
    cv::Point bTmp = cv::Point(b.x-a.first.x, b.y-a.first.y);
    double r = crossProduct(aTmp.second, bTmp);
    return (std::abs(r) < EPSILON);
}

bool Geometry::isPointRightofLine(LineSegment a, cv::Point b){
    LineSegment aTmp = LineSegment(cv::Point(0,0),
                                   cv::Point(a.second.x-a.first.x,a.second.y-a.first.y));
    cv::Point bTmp = cv::Point(b.x-a.first.x, b.y-a.first.y);
    return (crossProduct(aTmp.second, bTmp)<0);
}

bool Geometry::lineSegmentTouchesOrCrossesLine(LineSegment a, LineSegment b){
    return ((isPointOnLine(a, b.first))
            || (isPointOnLine(a, b.second))
            || ((isPointRightofLine(a, b.first) ^ isPointRightofLine(a, b.second))));
}

bool Geometry::doLinesIntersect(LineSegment a, LineSegment b){
    //x,y,w,h
    int x1 = std::min(a.first.x,a.second.x);
    int y1 = std::min(a.first.y,a.second.y);
    int w1 = std::abs(a.first.x-a.second.x);
    int h1 = std::abs(a.first.y-a.second.y);
    cv::Rect box1 = cv::Rect(x1,y1,w1,h1);
    int x2 = std::min(b.first.x,b.second.x);
    int y2 = std::min(b.first.y,b.second.y);
    int w2 = std::abs(b.first.x-b.second.x);
    int h2 = std::abs(b.first.y-b.second.y);
    cv::Rect box2 = cv::Rect(x2,y2,w2,h2);

    return ((doBBoxesIntersect(box1,box2))
            && (lineSegmentTouchesOrCrossesLine(a,b))
            && (lineSegmentTouchesOrCrossesLine(b,a)));
}
