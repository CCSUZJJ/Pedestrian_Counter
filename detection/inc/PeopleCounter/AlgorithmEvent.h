#ifndef ALGOEVENT_H
#define ALGOEVENT_H

#include <vector>
#include "DetectedBlob.h"

class AlgoEvent{
    public:
        AlgoEvent(){}
        ~AlgoEvent(){}

        int getStart();
        void setStart(int s);
        int getEnd();
        void setEnd(int e);
        cv::Rect getStartRect();
        void setStartRect(cv::Rect sr);
        cv::Rect getEndRect();
        void setEndRect(cv::Rect er);
        bool getPosToNeg();
        void setPosToNeg(bool v);
        bool getFoundInGT();
        void setFoundInGT(bool b);

    private:
        int start;
        int end;
        cv::Rect startRect;
        cv::Rect endRect;
        bool posToNeg;
        bool foundInGT = false;
};

#endif
