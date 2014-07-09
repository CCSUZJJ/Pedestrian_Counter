#include "gtEvent.h"


int gtEvent::getStart(){
    return start;
}

void gtEvent::setStart(int s){
    start = s;
}

int gtEvent::getEnd(){
    return end;
}

void gtEvent::setEnd(int e){
     end = e;
}

cv::Rect gtEvent::getStartRect(){
    return startRect;
}

void gtEvent::setStartRect(cv::Rect sr){
    startRect = sr;
}

cv::Rect gtEvent::getEndRect(){
    return endRect;
}

void gtEvent::setEndRect(cv::Rect er){
    endRect = er;
}

bool gtEvent::getPosToNeg(){
    return posToNeg;
}

void gtEvent::setPosToNeg(bool v){
    posToNeg = v;
}

bool gtEvent::getFoundMatch(){
    return foundMatch;
}

void gtEvent::setFoundMatch(bool v){
    foundMatch = v;
}

bool gtEvent::isMatch(AlgoEvent e){
    if((start+30 >= e.getStart()) && (start-30 <= e.getStart()) &&  //start within margin
       (end+30 >= e.getEnd()) && (end-30 <= e.getEnd())){           //end within margin
        return true;
    }
    else{
        return false;
    }
}
