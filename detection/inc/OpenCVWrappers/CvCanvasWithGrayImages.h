#ifndef CVCANVASWITHGRAYIMAGES_H
#define CVCANVASWITHGRAYIMAGES_H

#include <opencv/cv.h> // includes OpenCV definitions

class CvCanvasWithGrayImages
{
public:

    // Constructor/destructor

    CvCanvasWithGrayImages();
    virtual ~CvCanvasWithGrayImages();

    // Setup-Functions

    void Reset();
    void SetCanvas(const CvSize& SizeCanvas,unsigned char BackgroundValue);
    void PlotImage(const IplImage* ImageOI,const CvPoint& Location);

    // Display-functions

    void ShowCanvas(const char* Name);

private:

    IplImage* Canvas;
 };

#endif // CVCANVASWITHGRAYIMAGES_H
