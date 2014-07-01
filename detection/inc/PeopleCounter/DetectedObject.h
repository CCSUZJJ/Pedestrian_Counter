#ifndef DETECTEDOBJECT_H
#define DETECTEDOBJECT_H

#include <vector>
#include "DetectedBlob.h"

struct DetectedObject{
    int id;
    int catID;
    std::vector<DetectedBlob> blobs;
    int dx, dy;

    bool done = false;

    cv::Scalar color;
};

#endif
