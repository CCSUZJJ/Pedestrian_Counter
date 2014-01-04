#include "CvCanvasWithGrayImages.h"
#include <opencv/highgui.h> // includes highGUI definitions

CvCanvasWithGrayImages::CvCanvasWithGrayImages()
    : Canvas(0)
{}

CvCanvasWithGrayImages::~CvCanvasWithGrayImages()
{
    Reset();
}

void CvCanvasWithGrayImages::Reset()
{
    if (Canvas != 0)
    {
        cvReleaseImage(&Canvas);
        Canvas = 0;
    }
}

void CvCanvasWithGrayImages::ShowCanvas(const char* Name)
{
    if (Canvas != 0)
    {
        cvShowImage(Name,Canvas);
    }
}

void CvCanvasWithGrayImages::SetCanvas(const CvSize& SizeCanvas,unsigned char BackgroundValue)
{
    Reset();
    Canvas = cvCreateImage(SizeCanvas, IPL_DEPTH_8U, 1);
    if (Canvas == 0) return;     // Not enough memory to create image

    // Set canvas-colour

    unsigned char* RowData = (unsigned char*)Canvas->imageData;
    for (int i=0;i < SizeCanvas.height;++i)
    {
        unsigned char* DestinationData = RowData;
        for (int j=0;j < SizeCanvas.width; ++j) *DestinationData++ = BackgroundValue;
        RowData += Canvas->widthStep;
    }
}

void CvCanvasWithGrayImages::PlotImage(const IplImage* ImageOI,const CvPoint& Location)
{
    if (Canvas == 0) return;    // No canvas
    if (ImageOI == 0) return;

    CvSize SizeCanvas = cvGetSize(Canvas);
    CvSize SizeImage = cvGetSize(ImageOI);

    if (Location.x >= SizeCanvas.width) return; // Out of bounds
    if (Location.y >= SizeCanvas.height) return; // Out of bounds

    unsigned int RowsToPlot = SizeCanvas.height-Location.y;
    if ((int)RowsToPlot > SizeImage.height) RowsToPlot = SizeImage.height;

    unsigned int ColumnsToPlot = SizeCanvas.width-Location.x;
    if ((int)ColumnsToPlot > SizeImage.width) ColumnsToPlot = SizeImage.width;

    unsigned char* RowCanvasData = (unsigned char*)Canvas->imageData;
    RowCanvasData += (Location.y * Canvas->widthStep) + Location.x;
    const unsigned char* RowImageData = (const unsigned char*)ImageOI->imageData;

    for (unsigned int i = 0;i < RowsToPlot;++i)
    {
        unsigned char* DestinationData = RowCanvasData;
        const unsigned char* SourceData = RowImageData;

        for (unsigned int j= 0;j < ColumnsToPlot;++j) *DestinationData++ = *SourceData++;

        RowCanvasData += Canvas->widthStep;
        RowImageData += ImageOI->widthStep;
    }
}

