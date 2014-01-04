#include <iostream>
#include "OpenCvVideo.h"


OpenCvVideo::OpenCvVideo(const std::string& FileName):
    m_FileName(FileName)
  , m_Capture(0)
  , m_FrameSize()
  , m_FrameRate()
  , m_FrameRateDouble()
  , m_NumFrames()
{}

OpenCvVideo::OpenCvVideo():
    m_FileName()
  , m_Capture(0)
  , m_FrameSize()
  , m_FrameRate()
  , m_FrameRateDouble()
  , m_NumFrames()
{}

OpenCvVideo::~OpenCvVideo()
{
    Reset();
}

void OpenCvVideo::Reset()
{
    if(m_Capture)
    {
        cvReleaseCapture( &m_Capture);
        m_Capture = 0;
    }
    m_VideoCap.release();
}


void OpenCvVideo::SetName(const std::string& FileName)
{
    m_FileName = FileName;
}


bool OpenCvVideo::Init()
{
    if (m_FileName.empty())
    {
        return false;  // DetectionFrameworkExitCodes::InvalidVideoFilename;
    }

    if (!m_VideoCap.open(m_FileName.c_str()))
    {
        std::cout << "Error opening video " << m_FileName.c_str() << std::endl;
        return false;  // DetectionFrameworkExitCodes::NoVideoCapture;
    }

    m_NumFrames =  m_VideoCap.get(CV_CAP_PROP_FRAME_COUNT);
    m_FrameSize = cvSize((int)m_VideoCap.get(CV_CAP_PROP_FRAME_WIDTH),
                         (int)m_VideoCap.get(CV_CAP_PROP_FRAME_HEIGHT));
    if ( (m_FrameSize.width == 0) || (m_FrameSize.height == 0) )
    {
        return false; // DetectionFrameworkExitCodes::NoFrameSize;
    }

    double m_FrameRateDouble = m_VideoCap.get(CV_CAP_PROP_FPS);
    m_FrameRate =  m_VideoCap.get(CV_CAP_PROP_FPS);

  //  char MyWebCamCodec;
//    MyWebCamCodec = m_VideoCap.get(CV_CAP_PROP_FOURCC);
//    m_VideoCap.set(CV_CAP_PROP_FOURCC,78);

   // m_VideoCap.set(CV_CAP_PROP_FOURCC,0.1);

//    cvSetCaptureProperty(capture, CV_CAP_POS_FRAMES, i);

/*
     if (m_Capture != 0)
    {
        cvReleaseCapture( &m_Capture );
        m_Capture = 0;
    }
    m_Capture = cvCreateFileCapture(m_FileName.c_str());
    if (m_Capture == 0)
    {
        std::cout << "Error opening video " << m_FileName.c_str() << std::endl;
        return false;  // DetectionFrameworkExitCodes::NoVideoCapture;
    }

    m_NumFrames = (int)(cvGetCaptureProperty( m_Capture, CV_CAP_PROP_FRAME_COUNT ));
    m_FrameSize = cvSize((int)cvGetCaptureProperty( m_Capture, CV_CAP_PROP_FRAME_WIDTH),
                         (int)cvGetCaptureProperty( m_Capture, CV_CAP_PROP_FRAME_HEIGHT));
    if ( (m_FrameSize.width == 0) || (m_FrameSize.height == 0) )
    {
        return false; // DetectionFrameworkExitCodes::NoFrameSize;
    }

    m_FrameRateDouble = cvGetCaptureProperty( m_Capture, CV_CAP_PROP_FPS );
    m_FrameRate = (unsigned int)cvGetCaptureProperty( m_Capture, CV_CAP_PROP_FPS );
    //if (FrameRate < 25) FrameRate = 25; // It looks as if OpenCV has problems with low framerates...
/**/
    return true;
}
