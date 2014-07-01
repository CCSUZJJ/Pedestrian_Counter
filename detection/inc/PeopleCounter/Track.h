#ifndef TRACK_H
#define TRACK_H

#include <vector>
#include "DetectedBlob.h"

class Track{
    public:
        Track(){}
        ~Track(){}

        std::vector<DetectedBlob> getBoxes();
        cv::Point getPrediction();
        void setPrediction(cv::Point pred);
        cv::Scalar getColor();
        void setColor(cv::Scalar col);
        int getDx();
        void setDx(int x);
        int getDy();
        void setDy(int y);
        bool getMatched();
        void setMatched(bool val);
        int getTTL();
        void setTTL(int i);
        void decrTTL();
        void setBLCrossed(bool val);
        bool getBLCrossed();
        void setBRCrossed(bool val);
        bool getBRCrossed();
        void setBLPosToNeg(bool val);
        bool getBLPosToNeg();
        void setBRPosToNeg(bool val);
        bool getBRPosToNeg();

        static cv::Point getBaseCenter(cv::Rect rect);
        cv::Point predict(cv::Rect rect, double learningRate);
        void addBlob(DetectedBlob blob);

        void findBestMatch(std::vector<DetectedBlob> &blobs);

    private:
        std::vector<DetectedBlob> boxes;
        cv::Point prediction;
        cv::Scalar color;

        int dx = 0;
        int dy = 0;
        bool matched = false;
        int ttl = 75;
        bool BLCrossed = false;
        bool BRCrossed = false;
        bool BLPosToNeg = false;
        bool BRPosToNeg = false;
};

#endif
