#ifndef FOREGROUNDSUBTRACTION_H
#define FOREGROUNDSUBTRACTION_H

#include "OpenCvUtilities.h"

class ForegroundSegmentation {
    public:
        ForegroundSegmentation(){}
        ~ForegroundSegmentation(){}

        void runningAvgBackground(cv::Mat frame, cv::Mat bg, cv::Mat fg);

        void medianBackground(cv::Mat frame, cv::Mat bg, cv::Mat fg);

        void sigmaDeltaCMBackground(cv::Mat frame, cv::Mat bg, cv::Mat fg, cv::Mat frameCount,
                                    cv::Mat confidence, cv::Mat variance, cv::Mat detectionCount,
                                    bool toUpdate);
};

#endif
