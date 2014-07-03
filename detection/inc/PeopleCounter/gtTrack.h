#ifndef GTTRACK_H
#define GTTRACK_H

#include <vector>
#include "DetectedBlob.h"

class gtTrack{
    public:
        gtTrack(){}
        ~gtTrack(){}

        std::vector<DetectedBlob> getBoxes();
        void addBlob(DetectedBlob blob);
        cv::Scalar getColor();
        void setColor(cv::Scalar col);
        int getID();
        void setID(int id);
        int getCatID();
        void setCatID(int catid);
        void setBLCrossed(bool val);
        bool getBLCrossed();
        void setBRCrossed(bool val);
        bool getBRCrossed();
        void setBLPosToNeg(bool val);
        bool getBLPosToNeg();
        void setBRPosToNeg(bool val);
        bool getBRPosToNeg();

    private:
        std::vector<DetectedBlob> boxes;
        cv::Scalar color;
        int ID;
        int catID;
        bool BLCrossed = false;
        bool BRCrossed = false;
        bool BLPosToNeg = false;
        bool BRPosToNeg = false;
};

#endif
