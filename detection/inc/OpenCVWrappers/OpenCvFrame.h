#ifndef OPENCV_FRAME_H
#define OPENCV_FRAME_H

#include <opencv/cv.h> // includes OpenCV definitions
#include <opencv/highgui.h> // includes highGUI definitions
#include "Point.h"

// Class for recording a video from image-frames (depth = 3 RGB-frames)

struct PixelVal
{
    PixelVal() : s_Red(), s_Green(), s_Blue()
    {}

    PixelVal(int R, int G, int B) : s_Red(R), s_Green(G), s_Blue(B)
    {}

    int s_Red;
    int s_Green;
    int s_Blue;
};

const PixelVal operator/(const PixelVal& lhs, const int& rhs);

class OpenCvFrame
{
public:
    OpenCvFrame(IplImage* Frame);
    OpenCvFrame(cv::Mat &Frame);
    OpenCvFrame();

    ~OpenCvFrame();

    void InitFrame(unsigned int Height, unsigned int Width);

    PixelVal GetPixel(unsigned X, unsigned Y) const;
    PixelVal GetPixel(const vipnt::CPoint& Pnt) const;

    void SetPixel(const vipnt::CPoint& Pnt, PixelVal Col);
    void ShowImage(const std::string& WindowText);
    void ShowImageMat(const std::string& WindowText);
    void SaveImage(const std::string& WindowText);
    void SetText(const std::string& Text);
    void SetText(const std::string& Text, const vipnt::CPoint& Pos);

    CvSize GetSize() { return m_Size; }
    IplImage* GetFrame() { return m_Frame; }
    cv::Mat&  GetFrameMat() { return m_Mat; }

    int GetHeight() { return m_Mat.rows; }
    int GetWidth()  { return m_Mat.cols;  }
    int GetNumChannels() {  return m_Mat.channels();}
    unsigned char* GetData() {  return m_Mat.data;}
    int GetStep() {  return m_Mat.step;}

private:
    IplImage* m_Frame;
    cv::Mat m_Mat;
    bool m_Created;
    CvSize m_Size;
};

#endif // OPENCV_FRAME_H
