#include "CvPlotUtilities.h"
#include <opencv/highgui.h> // includes highGUI definitions
#include "CvLine.h"

void DrawROI(IplImage* ImageOI,const CvScalar Colour,const CvRect& ROI,unsigned int Thickness)
{
    if (ImageOI == 0) return;                           // No image to plot unto
    if (ImageOI->nChannels != 3) return;                // RGB expected
    if ((ROI.width <= 0) || (ROI.height <= 0)) return;  // undefined ROI

    CvPoint Point1;
    CvPoint Point2;
    Point1.x = ROI.x;
    Point1.y = ROI.y;
    Point2.x = ROI.x+ROI.width-1;
    Point2.y = ROI.y+ROI.height-1;
    cvRectangle(ImageOI,Point1,Point2,Colour,Thickness);
}

void DrawPolygonContours(IplImage* ImageOI,const CvScalar Colour,const std::vector<vipnt::CvLine>& Polygon,unsigned int Thickness)
{
    if (ImageOI == 0) return;               // No image to plot unto
    if (ImageOI->nChannels != 3) return;    // RGB expected

    unsigned int NumberOfLines = (unsigned int) Polygon.size();
    if (NumberOfLines == 0) return;         // Nothing to draw

    CvPoint Point1;
    CvPoint Point2;

    for (unsigned i = 0;i <NumberOfLines;i++)
    {
        Point1.x = Polygon[i].Point1.x;
        Point1.y = Polygon[i].Point1.y;
        Point2.x = Polygon[i].Point2.x;
        Point2.y = Polygon[i].Point2.y;
        cvLine(ImageOI,Point1,Point2,Colour,Thickness);
    }
}

/*
void DrawPolygonContours(IplImage* ImageOI,const CvScalar Colour,const vipnt::CPolygon& Polygon,unsigned int Thickness)
{
    if (ImageOI == 0) return;                           // No image to plot unto
    if (ImageOI->nChannels != 3) return;    // RGB expected
    if (Polygon.IsEmpty()) return;          // No polygon available
    std::vector<vipnt::CPoint> PolygonVertices = Polygon.GetVertices();
    const unsigned int NumberOfVertices = (unsigned int)PolygonVertices.size();
    if (NumberOfVertices != 4) return;      // Can't determine linesOI

    CvPoint Point1;
    CvPoint Point2;
    for (unsigned int i=0;i<NumberOfVertices;++i)
    {
        Point1.x = PolygonVertices[i].GetX();
        Point1.y = PolygonVertices[i].GetY();
        unsigned int j = (i+1)%NumberOfVertices;
        Point2.x = PolygonVertices[j].GetX();
        Point2.y = PolygonVertices[j].GetY();
        cvLine(ImageOI,Point1,Point2,Colour,Thickness);
    }
}
*/


