#ifndef CVFUNCTIONPLOT_H
#define CVFUNCTIONPLOT_H

#include "CvCanvas.h"
#include <opencv/cv.h> // includes OpenCV definitions

// Control-class fo plotting one or multiple functions
// on a canvas

class CvFunctionPlot : public CvCanvas
{
public:
    CvFunctionPlot();
    virtual ~CvFunctionPlot();

    // Drawing-functions

    void PlotFunction(const std::vector<unsigned int>& FunctionData,const CvScalar& PlotColour,const CvRect& ROI,float ScaleFactor);
    void PlotLine(const CvPoint& Point1,const CvPoint& Point2,const CvScalar& PlotColour);
};

#endif // CVFUNCTIONPLOT_H
