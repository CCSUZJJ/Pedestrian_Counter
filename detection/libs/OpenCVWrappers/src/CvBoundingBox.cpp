#include "CvBoundingBox.h"
#include <algorithm>

namespace vipnt
{

CvBoundingBox::CvBoundingBox()
{
    x = 0;
    y = 0;
    width = 0;
    height = 0;
}

CvBoundingBox::CvBoundingBox(const CvRect& RectOI)
{
    x = RectOI.x;
    y = RectOI.y;
    width = RectOI.width;
    height = RectOI.height;
}

CvBoundingBox::CvBoundingBox(const CvPoint& Point1,const CvPoint& Point2)
{
    Set(Point1,Point2);
}

CvBoundingBox::CvBoundingBox(const CvPoint& Point1,int Width, int Height)
{
    Set(Point1,Width,Height);
}

CvBoundingBox::CvBoundingBox(int x, int y,int Width, int Height)
{
    this->x = x;
    this->y = y;
    width = Width;
    height = Height;
}

void CvBoundingBox::Reset()
{
    x = 0;
    y = 0;
    width = 0;
    height = 0;
}

bool CvBoundingBox::operator == (const CvRect& BoxOI) const
{
    return ((x == BoxOI.x)
            && (y == BoxOI.y)
            && (width == BoxOI.width)
            && (height == BoxOI.height));
}

bool CvBoundingBox::operator != (const CvRect& BoxOI) const
{
    return ((x != BoxOI.x)
            || (y != BoxOI.y)
            || (width != BoxOI.width)
            || (height != BoxOI.height));
}

void  CvBoundingBox::Set(const CvPoint& Point1,const CvPoint& Point2)
{
    x = std::min(Point1.x,Point2.x);
    y = std::min(Point1.y,Point2.y);

    if (Point1.x > x)  width =  Point1.x - x + 1;
    else width =  Point2.x - x + 1;

    if (Point1.y > y)  height =  Point1.y - y + 1;
    else height =  Point2.y - y + 1;
}

void CvBoundingBox::Set(const CvPoint& Point1,int Width, int Height)
{
    x = Point1.x;
    y = Point1.y;
    width = Width;
    height = Height;
}

void CvBoundingBox::Set(int x, int y,int Width, int Height)
{
    this->x = x;
    this->y = y;
    width = Width;
    height = Height;
}

void CvBoundingBox::Set(const CvBoundingBox& BoxOI)
{
    *this = BoxOI;
}

void CvBoundingBox::Add(int x, int y)
{
    if (!IsDefined())
    {
        this->x = x;
        this->y = y;
        width = 1;
        height = 1;
    }
    else
    {
        if (x < this->x)
        {
            width += this->x - x;
            this->x = x;
        } else if (x > (this->x + width-1)) width = x - this->x + 1;

        if (y < this->y)
        {
            height += this->y - y;
            this->y = y;
        } else if (y > (this->y + height-1)) height = y - this->y + 1;
    }
}

void CvBoundingBox::Add(const CvPoint& PointOI)
{
    Add(PointOI.x,PointOI.y);
}

void CvBoundingBox::Add(const CvBoundingBox& BoxOI)
{
    if (!IsDefined()) *this = BoxOI;
    else if (BoxOI.IsDefined())
    {
        Add(BoxOI.GetLeftTop());
        Add(BoxOI.GetRightBottom());
    }
}

CvPoint CvBoundingBox::GetCenter() const
{
    CvPoint PointOI;
    PointOI.x = x + (width + 1)/2;
    PointOI.y = y + (height + 1)/2;
    return PointOI;
}

CvPoint CvBoundingBox::GetLeftTop() const
{
    CvPoint PointOI;
    PointOI.x = x;
    PointOI.y = y;
    return PointOI;
}

CvPoint CvBoundingBox::GetRightTop() const
{
    CvPoint PointOI;
    PointOI.x = x + width - 1;
    PointOI.y = y;
    return PointOI;
}

CvPoint CvBoundingBox::GetLeftBottom() const
{
    CvPoint PointOI;
    PointOI.x = x;
    PointOI.y = y + height - 1;
    return PointOI;
}

CvPoint CvBoundingBox::GetRightBottom() const
{
    CvPoint PointOI;
    PointOI.x = x + width - 1;
    PointOI.y = y + height - 1;
    return PointOI;
}

bool CvBoundingBox::Contains(const CvPoint& PointOI) const
{
    return ((PointOI.x >= x)
            && (PointOI.x <= (x+width-1))
            && (PointOI.y >= y)
            && (PointOI.y <= (y+height-1)));
}

bool CvBoundingBox::Contains(const CvBoundingBox& BoxOI) const
{
     return ((BoxOI.x >= x)
            && ((BoxOI.x + BoxOI.width) <= (x + width))
            && (BoxOI.y >= y)
            && ((BoxOI.y + BoxOI.height) <= (y + height)));
}


    // Relative positioning-feature. Check if there is a horizontal overlap
    // Note that y is considered arbitrarly

bool CvBoundingBox::HasHorizontalOverlap(const CvBoundingBox& BoxOI) const
{
    if (!BoxOI.IsDefined() || !IsDefined()) return false;

    const int XRight1 = x + width - 1;
    const int XRight2 = BoxOI.x + BoxOI.width - 1;
    return ((x <= XRight2) && (BoxOI.x <= XRight1));
}

    // Relative positioning-feature. Check if a point(x,*) could
    // be contained into the boundingbox

bool CvBoundingBox::IsInHorizontalRange(int x) const
{
    return ((x >= this->x)
            && (x <= (this->x + width -1)));
}

    // Relative positioning-feature. Check if a point(*,y) could
    // be contained into the boundingbox

bool CvBoundingBox::IsInVerticalRange(int y) const
{
    return ((y >= this->y)
            && (y <= (this->y + height -1)));
}

    // Relative positioning-feature. Check if there is a vertical overlap
    // Note that x is considered arbitrarly

bool CvBoundingBox::HasVerticalOverlap(const CvBoundingBox& BoxOI) const
{
    if (!BoxOI.IsDefined() || !IsDefined()) return false;

    const int YBottom1 = y + height - 1;
    const int YBottom2 = BoxOI.y + BoxOI.height - 1;
    return ((y <= YBottom2) && (BoxOI.y <= YBottom1));
}

void CvBoundingBox::GetIntersection(const CvBoundingBox& BoxOI,CvBoundingBox& Intersection) const
{
    if (BoxOI.IsDefined() && IsDefined())
    {
        const int XLeft1 = x;
        const int XRight1 = x + width - 1;
        const int YTop1 = y;
        const int YBottom1 = y + height - 1;

        const int XLeft2 = BoxOI.x;
        const int XRight2 = BoxOI.x + BoxOI.width - 1;
        const int YTop2 = BoxOI.y;
        const int YBottom2 = BoxOI.y + BoxOI.height - 1;

        if ((XLeft1 <= XRight2)
                && (XLeft2 <= XRight1)
                && (YTop1 <= YBottom2)
                && (YTop2 <= YBottom1))
        {
            // Intersecting !

            int XLeft = (XLeft1 > XLeft2) ? XLeft1 : XLeft2;
            int XRight = (XRight1 < XRight2) ? XRight1 : XRight2;

            int YTop = (YTop1 > YTop2) ? YTop1 : YTop2;
            int YBottom = (YBottom1 < YBottom2) ? YBottom1 : YBottom2;

            Intersection.x = XLeft;
            Intersection.width = XRight - XLeft + 1;
            Intersection.y = YTop;
            Intersection.height = YBottom - YTop + 1;
        }  else Intersection.Reset();   // not intersecting
    } else Intersection.Reset();      // not intersecting (undefined boxes)
}

bool CvBoundingBox::HasOverlap(const CvBoundingBox& BoxOI) const
{
    CvBoundingBox Intersection;
    GetIntersection(BoxOI,Intersection);
    return Intersection.IsDefined();
}

    // Get the minimum ROI where both ROI's are part of

void CvBoundingBox::GetMinimum(const CvBoundingBox& BoxOI,CvBoundingBox& MinimumBox) const
{
    if (IsDefined())
    {
        if (BoxOI.IsDefined())
        {
        const int XLeft1 = x;
        const int XRight1 = x + width - 1;
        const int YTop1 = y;
        const int YBottom1 = y + height - 1;

        const int XLeft2 = BoxOI.x;
        const int XRight2 = BoxOI.x + BoxOI.width - 1;
        const int YTop2 = BoxOI.y;
        const int YBottom2 = BoxOI.y + BoxOI.height - 1;

        int XLeft = (XLeft1 < XLeft2) ? XLeft1 : XLeft2;
        int XRight = (XRight1 > XRight2) ? XRight1 : XRight2;

        int YTop = (YTop1 < YTop2) ? YTop1 : YTop2;
        int YBottom = (YBottom1 > YBottom2) ? YBottom1 : YBottom2;

        MinimumBox.x = XLeft;
        MinimumBox.width = XRight - XLeft + 1;
        MinimumBox.y = YTop;
        MinimumBox.height = YBottom - YTop + 1;
        } else MinimumBox = *this;
    } else MinimumBox = BoxOI;
}

} // namespace vipnt
