#ifndef OPENCV_VIDEO_H
#define OPENCV_VIDEO_H

#include <opencv/cv.h> // includes OpenCV definitions
#include <opencv/highgui.h> // includes highGUI definitions

#include "cv.h"
#include "highgui.h"

// Class for playing a video

class OpenCvVideo
{
public:
    OpenCvVideo(const std::string& FileName);

    OpenCvVideo();

    void SetName(const std::string& FileName);

    ~OpenCvVideo();
    void Reset();

    bool Init();
    unsigned int GetFrameRate() { return m_FrameRate; }
    CvSize GetFrameSize()       { return m_FrameSize; }
/**/
    cv::Mat GetNextFrame()
    {
        m_VideoCap >> m_CurrFrame;
        return m_CurrFrame;
    }
/**/
    /*
    IplImage* GetNextFrame()
    {
//        double INfo = cvGetCaptureProperty(m_Capture, CV_CAP_PROP_POS_AVI_RATIO );
        return m_Capture !=0 ?cvQueryFrame(m_Capture) : 0;
    }*/

private:
    std::string m_FileName;
    CvCapture*  m_Capture;
    cv::VideoCapture  m_VideoCap;
    CvSize m_FrameSize;
    unsigned int m_FrameRate;
    double m_FrameRateDouble;
    int m_NumFrames;
    cv::Mat m_CurrFrame;
};

#endif // OPENCV_FRAME_H
