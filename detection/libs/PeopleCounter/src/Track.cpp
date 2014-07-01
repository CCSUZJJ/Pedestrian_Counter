#include "Track.h"

void Track::findBestMatch(std::vector<DetectedBlob>& blobs){
    setMatched(false);
    int maxDist = 50;
    int shortestDist = maxDist+1;
    DetectedBlob bestMatch;
    std::vector<DetectedBlob>::iterator blob;
    int matchedLabel = 0;
    for(blob = blobs.begin(); blob != blobs.end(); blob++){
        if(blob->foundMatch == false){
            cv::Point midBase = getBaseCenter(blob->BBox);
            cv::Point prediction = getPrediction();
            cv::Point diff = prediction - midBase;
            double distance = sqrt(diff.x*diff.x + diff.y*diff.y);
            if((distance < shortestDist) && (distance < maxDist)){
                bestMatch = *blob;
                shortestDist = distance;
                matchedLabel = blob->label;
            }
        }
    }
    if(matchedLabel != 0){
        for(blob = blobs.begin(); blob != blobs.end(); blob++){
            if(blob->label == matchedLabel){
                blob->foundMatch = true;
            }
        }
        predict(bestMatch.BBox, 1);
        addBlob(bestMatch);
        setMatched(true);
    }
}

cv::Point Track::getBaseCenter(cv::Rect rect){
    return cv::Point(rect.x+(0.5*rect.width), rect.y+rect.height);
}

cv::Point Track::predict(cv::Rect rect, double learningRate){
    cv::Point currPos = getBaseCenter(boxes.back().BBox);
    int diffX = getBaseCenter(rect).x - currPos.x;
    int diffY = getBaseCenter(rect).y - currPos.y;

    dx = round((1-learningRate)*dx + learningRate*diffX);
    dy = round((1-learningRate)*dy + learningRate*diffY);

    prediction = cv::Point(currPos.x+dx, currPos.y+dy);
    return prediction;
}

void Track::addBlob(DetectedBlob blob){
    boxes.push_back(blob);
}

std::vector<DetectedBlob> Track::getBoxes(){
    return boxes;
}

cv::Point Track::getPrediction(){
    return prediction;
}

cv::Scalar Track::getColor(){
    return color;
}

void Track::setColor(cv::Scalar col){
    color = col;
}

void Track::setPrediction(cv::Point pred){
    prediction = pred;
}

int Track::getDx(){
    return dx;
}

void Track::setDx(int x){
    dx = x;
}

int Track::getDy(){
    return dy;
}

void Track::setDy(int y){
    dx = y;
}

void Track::setMatched(bool val){
    matched = val;
}

bool Track::getMatched(){
    return matched;
}

int Track::getTTL(){
    return ttl;
}

void Track::setTTL(int i){
    ttl = i;
}

void Track::decrTTL(){
    ttl--;
}

void Track::setBLCrossed(bool val){
    BLCrossed = val;
}

bool Track::getBLCrossed(){
    return BLCrossed;
}

void Track::setBRCrossed(bool val){
    BRCrossed = val;
}

bool Track::getBRCrossed(){
    return BRCrossed;
}

void Track::setBLPosToNeg(bool val){
    BLPosToNeg = val;
}

bool Track::getBLPosToNeg(){
    return BLPosToNeg;
}

void Track::setBRPosToNeg(bool val){
    BRPosToNeg = val;
}

bool Track::getBRPosToNeg(){
    return BRPosToNeg;
}
