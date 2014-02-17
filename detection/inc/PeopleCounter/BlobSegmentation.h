#ifndef BLOBSEGMENTATION_H
#define BLOBSEGMENTATION_H

#include "OpenCvUtilities.h"

class BlobSegmentation {
    public:
        BlobSegmentation(){}

        void intensitySegment(cv::Mat fg);
};

#endif
