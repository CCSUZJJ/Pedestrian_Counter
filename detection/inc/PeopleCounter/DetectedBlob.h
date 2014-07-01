#ifndef DETECTEDBLOB_H
#define DETECTEDBLOB_H

#include <vector>
#include "OpenCvUtilities.h"

struct DetectedBlob {
    cv::Rect BBox;
    int label;
    bool foundMatch = false;
    int frameNr;
};


#endif
