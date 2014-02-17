#ifndef FOREGROUNDSEGMENTATION_H
#define FOREGROUNDSEGMENTATION_H

#include "OpenCvUtilities.h"

class ForegroundSegmentation {
    public:
        ForegroundSegmentation(){}

        void runningAvgBackground(cv::Mat frame, cv::Mat bg, cv::Mat fg);

        void sigmaDeltaBackground(cv::Mat frame, cv::Mat bg, cv::Mat fg);

        void sigmaDeltaCMBackground(cv::Mat frame, cv::Mat bg, cv::Mat fg, cv::Mat frameCount,
                                    cv::Mat confidence, cv::Mat variance, cv::Mat detectionCount,
                                    bool toUpdate);
};

#endif
