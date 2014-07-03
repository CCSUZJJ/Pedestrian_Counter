#include "gtTrack.h"




void gtTrack::addBlob(DetectedBlob blob){
    boxes.push_back(blob);
}

std::vector<DetectedBlob> gtTrack::getBoxes(){
    return boxes;
}


cv::Scalar gtTrack::getColor(){
    return color;
}

void gtTrack::setColor(cv::Scalar col){
    color = col;
}

int gtTrack::getID(){
    return ID;
}

void gtTrack::setID(int id){
    ID = id;
}

int gtTrack::getCatID(){
    return catID;
}

void gtTrack::setCatID(int catid){
    catID = catid;
}

void gtTrack::setBLCrossed(bool val){
    BLCrossed = val;
}

bool gtTrack::getBLCrossed(){
    return BLCrossed;
}

void gtTrack::setBRCrossed(bool val){
    BRCrossed = val;
}

bool gtTrack::getBRCrossed(){
    return BRCrossed;
}

void gtTrack::setBLPosToNeg(bool val){
    BLPosToNeg = val;
}

bool gtTrack::getBLPosToNeg(){
    return BLPosToNeg;
}

void gtTrack::setBRPosToNeg(bool val){
    BRPosToNeg = val;
}

bool gtTrack::getBRPosToNeg(){
    return BRPosToNeg;
}
