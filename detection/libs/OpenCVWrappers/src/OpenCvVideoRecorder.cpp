#include "OpenCvVideoRecorder.h"

OpenCvVideoRecorder::OpenCvVideoRecorder()
{
    VideoWriter = 0;
}

OpenCvVideoRecorder::~OpenCvVideoRecorder()
{
    Reset();
}

void OpenCvVideoRecorder::Reset()
{
    if (VideoWriter != 0) cvReleaseVideoWriter(&VideoWriter);
    VideoWriter = 0;
}

void OpenCvVideoRecorder::Configure(const char* NameVideo,const CvSize& FrameSize,int FrameRate)
{
    Reset();        // Release current recording
    //	CV_FOURCC('P','I','M','1')    = MPEG-1 codec
    //	CV_FOURCC('M','J','P','G')    = motion-jpeg codec (does not work well)
    //	CV_FOURCC('M', 'P', '4', '2') = MPEG-4.2 codec
    //	CV_FOURCC('D', 'I', 'V', '3') = MPEG-4.3 codec
    //	CV_FOURCC('D', 'I', 'V', 'X') = MPEG-4 codec
    //	CV_FOURCC('U', '2', '6', '3') = H263 codec
    //	CV_FOURCC('I', '2', '6', '3') = H263I codec
    //	CV_FOURCC('F', 'L', 'V', '1') = FLV1 codec

    VideoWriter = cvCreateVideoWriter(NameVideo,CV_FOURCC('P','I','M','1'),
                                      FrameRate,FrameSize,1);
   // VideoWriter = cvCreateVideoWriter(NameVideo,-1,
   //                                   FrameRate,FrameSize,1);
}

void OpenCvVideoRecorder::AddFrame(const IplImage* FrameOI)
{
    if (VideoWriter != 0) cvWriteFrame(VideoWriter,FrameOI);
}
