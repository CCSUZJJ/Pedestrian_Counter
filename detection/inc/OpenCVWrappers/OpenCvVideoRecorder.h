#ifndef OPENCVVIDEORECORDER_H
#define OPENCVVIDEORECORDER_H

#include <opencv/cv.h> // includes OpenCV definitions
#include <opencv/highgui.h> // includes highGUI definitions

// Class for recording a video from image-frames (depth = 3 RGB-frames)

class OpenCvVideoRecorder
{
public:
    OpenCvVideoRecorder();
    virtual ~OpenCvVideoRecorder();

    void Reset();
    void Configure(const char* NameVideo,const CvSize& FrameSize,int FrameRate);
    void AddFrame(const IplImage* FrameOI);
    bool IsConfigured() const { return VideoWriter != 0; }


    CvVideoWriter* VideoWriter;
};

#endif // OPENCVVIDEORECORDER_H
