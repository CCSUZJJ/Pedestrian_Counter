#ifndef BLOBSEGMENTATION_H
#define BLOBSEGMENTATION_H

#include "OpenCvUtilities.h"

class BlobSegmentation {
    public:
        BlobSegmentation(){}

        std::vector<cv::Rect> contourSegment(cv::Mat fg);

        std::vector<cv::Rect> intensitySegment(cv::Mat fg);
};

#endif
