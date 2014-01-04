#ifndef CVCANVAS_H
#define CVCANVAS_H

#include <opencv/cv.h> // includes OpenCV definitions

// Elementary class to be used for drawing and plotting. This class
// wraps the IplImage-struct into a class-container such that the
// destructor will take care of the memory-release.

class CvCanvas
{
public:
    // Constructor/destructor

    CvCanvas();
    virtual ~CvCanvas();

    // Setup-Functions

    void Reset();

    // Setup a canvas with a uniform background-colour

    void SetCanvas(const CvSize& SizeCanvas,const CvScalar& CanvasColour);

    // Clear with background-colour

    void EraseCanvas(const CvScalar& CanvasColour);

    // Write text on the canvas

    void WriteTextOnCanvas(const std::string& StringOI,const CvScalar& TextColour,const CvPoint& Location);

    // Display-functions

    void ShowCanvas(const char* Name);

    // Has been Setup?

    bool HasBeenSetUp() const;

    // Get the image

    const IplImage* GetCanvas() const { return Canvas; }

    // Our canvas

protected:

    IplImage* Canvas;

private :
    CvCanvas(const CvCanvas&);      // prevent copy-constructor
};

#endif // CVCANVAS_H
