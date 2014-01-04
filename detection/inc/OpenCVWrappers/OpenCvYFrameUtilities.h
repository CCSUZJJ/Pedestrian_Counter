#ifndef OPENCVYFRAMREUTILITIES_H
#define OPENCVYFRAMREUTILITIES_H

#include <opencv/cv.h> // includes OpenCV definitions

void GetYFromRGB(const IplImage* RGBFrame,IplImage* YFrame);
IplImage* GetROI(const IplImage* ImageOI,const CvRect& ROI);

#endif // OPENCVYFRAMREUTILITIES_H
