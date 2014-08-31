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
        predict(bestMatch.BBox, 0.1);
        incrConfidence();
        classify(bestMatch);
        addBlob(bestMatch);
        setMatched(true);
    }
}

void Track::classify(DetectedBlob blob){
    LineSegment sv = LineSegment(cv::Point(10,2.4), cv::Point(20,0.4));
    cv::Point input = cv::Point(blob.compactness, blob.leanness);
    if(!(Geometry::whatSideOfLine(sv, input))){
        incrPedestrianCnt();
    }
    //else{
    //    decrPedestrianCnt();
    //}
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

void Track::decTTLBy(int i){
    ttl -= i;
}

void Track::condDecrTTL(){
    int minDec = 1;
    int maxDec = 10;
    int minConf = 15;
    int maxConf = 50;
    if(confidence < minConf){
        ttl -= maxDec;
    }
    else if(confidence >= minConf || confidence < maxConf){
        //decrement i.f.o. confidence
        double ratio = ((maxDec-minDec)*1.0)/(maxConf-minConf);   // *1.0 else integer division
        int decrement = maxDec - round((confidence - minConf)*ratio);
        decTTLBy(decrement);
    }
    else if(confidence >= maxConf){
        ttl -= minDec;
    }
}

int Track::getConfidence(){
    return confidence;
}

void Track::setConfidence(int i){
    confidence = i;
}

void Track::incrConfidence(){
    confidence++;
}

void Track::decrConfidence(){
    confidence--;
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

AlgoEvent Track::getEvent(){
    return event;
}

void Track::setEvent(AlgoEvent e){
    event = e;
}

bool Track::getCountedNegToPos(){
    return CountedNegToPos;
}

void Track::setCountedNegToPos(bool v){
    CountedNegToPos = v;
}

bool Track::getCountedPosToNeg(){
    return CountedPosToNeg;
}

void Track::setCountedPosToNeg(bool v){
    CountedPosToNeg = v;
}

void Track::setCounted(bool v){
    if(v=true){
        CountedPosToNeg = true;
    }
    else{
        CountedNegToPos = true;
    }
}

bool Track::getCounted(bool v){
    if(v=true){
        return CountedPosToNeg;
    }
    else{
        return CountedNegToPos;
    }
}

int Track::getPedestrianCnt(){
    return pedestrianCnt;
}

void Track::incrPedestrianCnt(){
    if(pedestrianCnt<40)
        pedestrianCnt++;
}

void Track::decrPedestrianCnt(){
    if(pedestrianCnt>0){
        pedestrianCnt--;
    }
}

void Track::setPedestrianCnt(int c){
    pedestrianCnt = c;
}

bool Track::isPedestrian(){
    return(pedestrianCnt >= 40);
}

