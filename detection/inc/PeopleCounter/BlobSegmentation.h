#ifndef BLOBSEGMENTATION_H
#define BLOBSEGMENTATION_H

#include "OpenCvUtilities.h"
#include "DetectedBlob.h"

struct Run{
    cv::Point start;
    int length;
    int label;
};

class BlobSegmentation {
    public:
        BlobSegmentation(){}
        ~BlobSegmentation(){}

        std::vector<cv::Rect> contourSegment(cv::Mat fg);

        void /*std::vector<cv::Rect>*/ intensitySegment(cv::Mat fg);

        std::vector<DetectedBlob> connectedComponentSegment(cv::Mat fg, int frameNumber);
};

#endif
