#ifndef CVBOUNDINGBOX_H
#define CVBOUNDINGBOX_H

#include "CvRect.h"
#include "CvPoint.h"

namespace vipnt
{

// Remark : This class differs from the opencv CvRect in such a way that
// width and height are set to zero @construction, where as CvRect is
// not instantiated at all.

class CvBoundingBox : public CvRect
{
public:
    CvBoundingBox();
    CvBoundingBox(const CvRect& RectOI);
    CvBoundingBox(const CvPoint& Point1,const CvPoint& Point2);
    CvBoundingBox(const CvPoint& Point1,int Width, int Height);
    CvBoundingBox(int x, int y,int Width, int Height);

    // Functions

    void Reset();
    void Set(const CvPoint& Point1,const CvPoint& Point2);
    void Set(const CvPoint& Point1,int Width, int Height);
    void Set(int x, int y,int Width, int Height);
    void Set(const CvBoundingBox& BoxOI);

    void Add(int x, int y);
    void Add(const CvPoint& PointOI);
    void Add(const CvBoundingBox& BoxOI);

    // Operators

    bool operator == (const CvRect& BoxOI) const;
    bool operator != (const CvRect& BoxOI) const;

    // Features

    bool IsDefined() const { return (width > 0) && (height > 0); }
    inline int GetWidth() const { return width; }
    inline int GetHeight() const { return height; }

    // Area in pixels = width * height

    inline int GetArea() const { return width * height; }

    // Coordinates of interest
    // Top = y-value of the top)linepiece
    // Bottom = y-value of the bottom-linepiece
    // Left = x-value of the left-linepiece
    // Right = x-value of the right-linepiece

    inline int GetTop() const { return y;}
    inline int GetBottom() const { return y+height-1; }
    inline int GetLeft() const { return x;}
    inline int GetRight() const { return x+width-1; }

    // Specific Points of the bounding-box

    CvPoint GetCenter() const;
    CvPoint GetLeftTop() const;
    CvPoint GetRightTop() const;
    CvPoint GetLeftBottom() const;
    CvPoint GetRightBottom() const;

    // Returns true if the given point is in the box

    bool Contains(const CvPoint& PointOI) const;

    // Returns true if the given box is included into my box

    bool Contains(const CvBoundingBox& BoxOI) const;

    // Orientation-functions

    bool HasHorizontalOverlap(const CvBoundingBox& BoxOI) const;
    bool IsInHorizontalRange(int x) const;
    bool HasVerticalOverlap(const CvBoundingBox& BoxOI) const;
    bool IsInVerticalRange(int y) const;

    // Calculate the intersection (Will result in an undefined box if there
    // is no intersection at all)

    void GetIntersection(const CvBoundingBox& BoxOI,CvBoundingBox& Intersection) const;

    // Returns true if the box overlaps (= has an intersection) with the given box

    bool HasOverlap(const CvBoundingBox& BoxOI) const;

    // Get the minimum-box that contains both this bounding-box
    // and the specified boxOI.
    // Is some kind of union of the 2 boundingboxes

    void GetMinimum(const CvBoundingBox& BoxOI,CvBoundingBox& MinimumBox) const;
};

} // namespace vipnt
#endif // CVBOUNDINGBOX_H
