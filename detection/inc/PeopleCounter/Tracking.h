#ifndef TRACKING_H
#define TRACKING_H

#include "OpenCvUtilities.h"
#include <vector>
#include "DetectedBlob.h"
#include "DetectedObject.h"
#include "Track.h"

class Tracking{
    public:
        Tracking(){}
        ~Tracking(){}

        void simpleTracking(std::vector<Track>& finishedTracks, std::vector<Track>& currentTracks,
                            std::vector<DetectedBlob>& newBlobs, int framenr);

};

#endif
