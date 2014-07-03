#include "AlgorithmEvent.h"


int AlgoEvent::getStart(){
    return start;
}

void AlgoEvent::setStart(int s){
    start = s;
}

int AlgoEvent::getEnd(){
    return end;
}

void AlgoEvent::setEnd(int e){
     end = e;
}

cv::Rect AlgoEvent::getStartRect(){
    return startRect;
}

void AlgoEvent::setStartRect(cv::Rect sr){
    startRect = sr;
}

cv::Rect AlgoEvent::getEndRect(){
    return endRect;
}

void AlgoEvent::setEndRect(cv::Rect er){
    endRect = er;
}

bool AlgoEvent::getPosToNeg(){
    return posToNeg;
}

void AlgoEvent::setPosToNeg(bool v){
    posToNeg = v;
}
