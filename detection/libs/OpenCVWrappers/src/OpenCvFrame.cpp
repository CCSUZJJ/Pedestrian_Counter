#include "OpenCvFrame.h"


    const PixelVal operator/(const PixelVal& lhs, const int& Value)
    {
        PixelVal RetVal(lhs.s_Red/Value, lhs.s_Green/Value, lhs.s_Blue/Value);
        return RetVal;

    }

    OpenCvFrame::OpenCvFrame( IplImage* Frame) :
        m_Frame(Frame)
      , m_Created(false)
    {
        m_Size.height = Frame->width;
        m_Size.width = Frame->height;
    }

    OpenCvFrame::OpenCvFrame(cv::Mat&  Frame) :
        m_Mat(Frame)
      , m_Created(false)
    {
        m_Size.height = Frame.cols;
        m_Size.width  = Frame.rows;
    }


    OpenCvFrame::OpenCvFrame() :
        m_Frame()
      , m_Created(false)
    {
        m_Size.height = 0;
        m_Size.width = 0;
    }

    OpenCvFrame::~OpenCvFrame()
    {
       if(m_Frame && m_Created)
       {
          cvReleaseImage(&m_Frame);
       }
    }

    PixelVal OpenCvFrame::GetPixel(const vipnt::CPoint& Pnt) const
    {
        return GetPixel(Pnt.GetX(), Pnt.GetY());
    }

    PixelVal OpenCvFrame::GetPixel(unsigned X, unsigned Y) const
    {
        PixelVal Ret;
        if(m_Frame->nChannels == 1)
        {
            Ret.s_Blue  = ((uchar*)(m_Frame->imageData + m_Frame->widthStep*Y))[X];
            Ret.s_Green = ((uchar*)(m_Frame->imageData + m_Frame->widthStep*Y))[X];
            Ret.s_Red   = ((uchar*)(m_Frame->imageData + m_Frame->widthStep*Y))[X];
        }
        else if(m_Frame->nChannels == 3)
        {
            Ret.s_Blue  = ((uchar*)(m_Frame->imageData + m_Frame->widthStep*Y))[X*3];
            Ret.s_Green = ((uchar*)(m_Frame->imageData + m_Frame->widthStep*Y))[X*3 + 1];
            Ret.s_Red   = ((uchar*)(m_Frame->imageData + m_Frame->widthStep*Y))[X*3 + 2];
        }
        return Ret;
    }

    void OpenCvFrame::SetPixel(const vipnt::CPoint& Pnt, PixelVal Col)
    {
        unsigned int X = Pnt.GetX();
        unsigned int Y = Pnt.GetY();
        if(m_Frame->nChannels == 1)
        {
            ((uchar*)(m_Frame->imageData + m_Frame->widthStep*Y))[X] = Col.s_Red;
        }
        else if(m_Frame->nChannels == 3)
        {
            ((uchar*)(m_Frame->imageData + m_Frame->widthStep*Y))[X*3] = Col.s_Blue;
            ((uchar*)(m_Frame->imageData + m_Frame->widthStep*Y))[X*3 + 1] = Col.s_Green;
            ((uchar*)(m_Frame->imageData + m_Frame->widthStep*Y))[X*3 + 2] = Col.s_Red;
        }
    }

    void OpenCvFrame::ShowImage(const std::string& WindowText)
    {
        cvNamedWindow(WindowText.c_str(),CV_WINDOW_AUTOSIZE);
        cvShowImage(WindowText.c_str(),m_Frame);
        cvvWaitKey(0);
        cvDestroyWindow(WindowText.c_str());
    }

    void OpenCvFrame::ShowImageMat(const std::string& WindowText)
    {
        cv::namedWindow( WindowText.c_str(), CV_WINDOW_AUTOSIZE );
        cv::imshow(WindowText.c_str(),m_Mat);
        cv::waitKey(1);
        //cvDestroyWindow(WindowText.c_str());
    }

    void OpenCvFrame::SaveImage(const std::string& WindowText)
    {
        cv::imwrite(WindowText.c_str(), cv::Mat(m_Frame));

    }

    void OpenCvFrame::SetText(const std::string& Text, const vipnt::CPoint& Pos)
    {
        CvFont font;
        CvPoint pt = cvPoint(Pos.GetX(),Pos.GetY());
        cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, 1.0, 1.0,0,1);
        cvPutText(m_Frame, Text.c_str(), pt, &font, CV_RGB(255,0,0));
    }


    void OpenCvFrame::InitFrame(unsigned int Height, unsigned int Width)
    {
        if(m_Created)
        {
            cvReleaseImage(&m_Frame);
        }
        m_Size.height = Width;
        m_Size.width = Height;

        m_Frame = cvCreateImage(m_Size,IPL_DEPTH_8U,3);
        m_Created = true;
    }
