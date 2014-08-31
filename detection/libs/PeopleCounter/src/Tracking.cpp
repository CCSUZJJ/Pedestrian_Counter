#include "Tracking.h"
#include <vector>
#include "DetectedObject.h"
#include "DetectedBlob.h"
#include <math.h>

void::Tracking::simpleTracking(std::vector<Track>& finishedTracks, std::vector<Track>& currentTracks,
                               std::vector<DetectedBlob>& newBlobs, int framenr){
    std::vector<Track>::iterator currTrack;
    for(currTrack = currentTracks.begin(); currTrack != currentTracks.end(); ){
        currTrack->findBestMatch(newBlobs);
        if(currTrack->getMatched() == false){
            currTrack->decrTTL();
            //currTrack->condDecrTTL();   //in function of the confidence (high confidence => low decrement)
            if(currTrack->getTTL() <= 0){
                finishedTracks.push_back(*currTrack);
                currentTracks.erase(currTrack);
            }
            else {
                //Update prediction
                int currDx = currTrack->getDx();
                int currDy = currTrack->getDy();
                cv::Point currPred = currTrack->getPrediction();
                cv::Point newPred = cv::Point(currPred.x+currDx, currPred.y+currDy);
                currTrack->setPrediction(newPred);
                currTrack++; //iterator update
            }
        }
        else {
            currTrack++;
        }
    }

    time_t rawtime;
    time(&rawtime);
    cv::RNG rng(rawtime);
    std::vector<DetectedBlob>::iterator blob;
    for(blob = newBlobs.begin(); blob != newBlobs.end(); blob++){
        if(blob->foundMatch == false){
            Track newTrack;
            newTrack.addBlob(*blob);
            newTrack.setPrediction(cv::Point(blob->BBox.x, blob->BBox.y));
            int icolor = (unsigned) rng;
            //cv::Scalar clr = cv::Scalar(icolor&255, (icolor>>8)&255, (icolor>>16)&255);
            cv::Scalar clr = cv::Scalar(0,255,0);
            newTrack.setColor(clr);
            currentTracks.push_back(newTrack);
        }
    }

}
