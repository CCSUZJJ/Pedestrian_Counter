#include "CvCanvas.h"
#include <opencv/highgui.h> // includes highGUI definitions

CvCanvas::CvCanvas()
    : Canvas(0)
{}

CvCanvas::~CvCanvas()
{
    Reset();
}

void CvCanvas::Reset()
{
    if (Canvas != 0)
    {
        cvReleaseImage(&Canvas);
        Canvas = 0;
    }
}

bool CvCanvas::HasBeenSetUp() const
{
    return (Canvas != 0);
}

void CvCanvas::ShowCanvas(const char* Name)
{
    if (Canvas != 0)
    {
        cvShowImage(Name,Canvas);
    }
}

void CvCanvas::SetCanvas(const CvSize& SizeCanvas,const CvScalar& CanvasColour)
{
    Reset();

    // Canvas defined ?

    if ((SizeCanvas.width == 0) || (SizeCanvas.height == 0)) return;

    // Create canvas

    Canvas = cvCreateImage(SizeCanvas, IPL_DEPTH_32F, 3);
    if (Canvas == 0) return;     // Not enough memory to create image

    // Set canvas-colour

    EraseCanvas(CanvasColour);
}

// Clear with background-colour

void CvCanvas::EraseCanvas(const CvScalar& CanvasColour)
{
    if (Canvas == 0) return;     // No canvas
    cvSet(Canvas,CanvasColour);
}

void CvCanvas::WriteTextOnCanvas(const std::string& StringOI,const CvScalar& TextColour,const CvPoint& Location)
{
    if (Canvas == 0) return;    // No canvas

    int fontFace = CV_FONT_HERSHEY_SCRIPT_SIMPLEX;
    double fontScale = 0.5;
    int thickness = 1;
    int baseline = 0;

    CvFont font;
    cvInitFont(&font, fontFace,fontScale, fontScale, 0, thickness, CV_AA);
    cv::Size TextSize = cv::getTextSize(StringOI, fontFace, fontScale, thickness, &baseline);

    CvPoint LocationOI = Location;

    if ((LocationOI.x+TextSize.width) > Canvas->width) LocationOI.x = Canvas->width - TextSize.width;
    if ((LocationOI.y-TextSize.height) < 0) LocationOI.y = TextSize.height;
    if (LocationOI.y >= Canvas->height) LocationOI.y = Canvas->height-1;
    cvPutText(Canvas, StringOI.c_str(), LocationOI, &font,TextColour);
}
