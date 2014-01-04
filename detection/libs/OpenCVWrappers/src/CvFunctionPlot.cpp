#include "CvFunctionPlot.h"
#include <opencv/highgui.h> // includes highGUI definitions

CvFunctionPlot::CvFunctionPlot()
{}

CvFunctionPlot::~CvFunctionPlot()
{
}

void CvFunctionPlot::PlotLine(const CvPoint& Point1,const CvPoint& Point2,const CvScalar& PlotColour)
{
    cvLine(Canvas,Point1,Point2, PlotColour,1,8,0);
}

void CvFunctionPlot::PlotFunction(const std::vector<unsigned int>& FunctionData,const CvScalar& PlotColour,const CvRect& ROI,float ScaleFactor)
{
    if (Canvas == 0) return;    // No canvas

    const unsigned int WidthFunction = (unsigned int) FunctionData.size();
    if (WidthFunction == 0) return;     // nothing to plot ...
    CvSize SizeCanvas = cvGetSize(Canvas);

    // Check location of ROI

    if (ROI.x < 0) return;                                  // invalid ROI
    if ((ROI.x + ROI.width) >= SizeCanvas.width) return;    // invalid ROI
    if (ROI.width < 1) return;                  // invalid ROI
    if (ROI.y < 0) return;                                  // invalid ROI
    if (ROI.height < 1) return;                             // invalid ROI
    if ((ROI.y + ROI.height) >= SizeCanvas.height) return;  // invalid ROI

    // Scale-factor

    unsigned int PlotWidth = WidthFunction;
    if (PlotWidth > (unsigned int)ROI.width) PlotWidth = ROI.width;

    // Draw the function

    for (unsigned int i=0; i < PlotWidth;++i)
    {
        unsigned int Value = FunctionData[i];
        unsigned int ValuePixels = cvRound(ScaleFactor*Value);
        if (ValuePixels > (unsigned int)ROI.height) ValuePixels = ROI.height;

        CvPoint PlotPoint;
        PlotPoint.x = ROI.x + i;
        PlotPoint.y = ROI.y + ROI.height - 1 - ValuePixels;
        cvSet2D(Canvas,PlotPoint.y,PlotPoint.x,PlotColour);
    }
}
