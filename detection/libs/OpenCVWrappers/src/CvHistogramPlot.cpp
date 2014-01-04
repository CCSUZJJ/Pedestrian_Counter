#include "CvHistogramPlot.h"
#include <opencv/highgui.h> // includes highGUI definitions


CvHistogramPlot::CvHistogramPlot() 
{}

CvHistogramPlot::~CvHistogramPlot()
{
    Reset();
}

void CvHistogramPlot::PlotHistogram(const std::vector<unsigned int>& Histogram,const CvScalar PlotColour,const CvRect& ROI,unsigned int BinWidth,float ScaleFactor)
{
    if (Canvas == 0) return;    // No canvas
    const unsigned int SizeHistogram = (unsigned int) Histogram.size();
    if (SizeHistogram == 0) return;     // nothing to plot ...
    if (BinWidth == 0) return;          // At least 1 pixel/bin is required

    CvSize SizeCanvas = cvGetSize(Canvas);

    // Check location of ROI

    if (ROI.x < 0) return;                                  // invalid ROI
    if ((ROI.x + ROI.width) >= SizeCanvas.width) return;    // invalid ROI
    if (ROI.width < (int)BinWidth) return;                  // invalid ROI
    if (ROI.y < 0) return;                                  // invalid ROI
    if (ROI.height < 1) return;                             // invalid ROI
    if ((ROI.y + ROI.height) >= SizeCanvas.height) return;  // invalid ROI

    // Scale-factor

    unsigned int PlotWidth = SizeHistogram*BinWidth;
    if (PlotWidth > (unsigned int)ROI.width) PlotWidth = ROI.width;
    const unsigned int BinsToPlot = PlotWidth/BinWidth;

    // Draw the bins

    for (unsigned int i=0; i < BinsToPlot;++i)
    {
        unsigned int Value = Histogram[i];
        unsigned int ValuePixels = cvRound(ScaleFactor*Value);
        if (ValuePixels > (unsigned int)ROI.height) ValuePixels = ROI.height;

        CvPoint LeftTop;
        CvPoint RightBottom;

        LeftTop.x = ROI.x + i*BinWidth;
        RightBottom.x = LeftTop.x + BinWidth;
        LeftTop.y = ROI.y + ROI.height - 1;
        RightBottom.y = LeftTop.y - ValuePixels;
        cvRectangle(Canvas,LeftTop,RightBottom,PlotColour,CV_FILLED);
    }
}

void CvHistogramPlot::PlotHistogram(const std::vector<unsigned int>& Histogram,const CvScalar PlotColour,const CvRect& ROI)
{
    if (Canvas == 0) return;    // No canvas
    const unsigned int SizeHistogram = (unsigned int) Histogram.size();
    if (SizeHistogram == 0) return;     // nothing to plot ...

    CvSize SizeCanvas = cvGetSize(Canvas);

    // Check plot-constraints

    const unsigned int BinWidth = 1;
    unsigned int MaximumValue = (unsigned int) *std::max_element(Histogram.begin(), Histogram.end());
    if (MaximumValue == 0) MaximumValue = 1;                // Prevent zero-divide in case of all zeroes

    // Check location of ROI

    if (ROI.x < 0) return;                                  // invalid ROI
    if ((ROI.x + ROI.width) >= SizeCanvas.width) return;    // invalid ROI
    if (ROI.width < (int)BinWidth) return;                  // invalid ROI
    if (ROI.y < 0) return;                                  // invalid ROI
    if (ROI.height < 1) return;                             // invalid ROI
    if ((ROI.y + ROI.height) >= SizeCanvas.height) return;  // invalid ROI

    // Scale-factor

    unsigned int PlotWidth = SizeHistogram*BinWidth;
    if (PlotWidth > (unsigned int)ROI.width) PlotWidth = ROI.width;
    unsigned int BinsToPlot = PlotWidth/BinWidth;

    // Draw the bins

    for (unsigned int i=0; i < BinsToPlot;++i)
    {
        unsigned int Value = Histogram[i];
        unsigned int ValuePixels = cvRound(((float)(Value*ROI.height))/MaximumValue);
        if (ValuePixels > (unsigned int)ROI.height) ValuePixels = ROI.height;

        CvPoint LeftTop;
        CvPoint RightBottom;

        LeftTop.x = ROI.x + i*BinWidth;
        RightBottom.x = LeftTop.x + BinWidth;
        LeftTop.y = ROI.y + ROI.height - 1;
        RightBottom.y = LeftTop.y - ValuePixels;
        cvRectangle(Canvas,LeftTop,RightBottom,PlotColour,CV_FILLED);
    }
}
