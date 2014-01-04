#ifndef CVHISTOGRAMPLOT_H
#define CVHISTOGRAMPLOT_H

#include <opencv/cv.h> // includes OpenCV definitions
#include "CvCanvas.h"

// Control-class fo plotting one or multiple histograms
// on a canvas

class CvHistogramPlot : public CvCanvas
{
public:

    // Constructor/destructor

    CvHistogramPlot();
    virtual ~CvHistogramPlot();

    // Drawing-functions

    // Plot histogram with binwidth == 1, and scale values such that the maximum-value
    // equals the maximum window-height
    // y = histogram_height

    void PlotHistogram(const std::vector<unsigned int>& Histogram,const CvScalar PlotColour,const CvRect& ROI);

    // Plot histogram (values after scaling will be truncated to the window-height. Only the
    // number of bins will be plotted that fit into the window
    // y = histogram_height

    void PlotHistogram(const std::vector<unsigned int>& Histogram,const CvScalar PlotColour,const CvRect& ROI,unsigned int BinWidth,float ScaleFactor);
};


#endif // CVHISTOGRAMPLOT_H
