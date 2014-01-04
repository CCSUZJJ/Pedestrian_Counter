#ifndef OPENCVUTILITIES_H
#define OPENCVUTILITIES_H

#include <opencv/cv.h> // includes OpenCV definitions
#include <opencv/highgui.h> // includes highGUI definitions
#include <opencv/cvaux.h>
#include <opencv/cxcore.h>
#include <string>
//#include "fast.h"
#include "OfflineFrame.h"
#include "CvHistogramPlot.h"

void SmoothInputImage(IplImage* YFrame);

void GetYFromRGB(const IplImage* RGBFrame,IplImage* YFrame);
void DrawCross(IplImage* ImageOI,CvScalar Colour,unsigned long Size,unsigned long x, unsigned long y);
void PlotProfile(const std::vector<unsigned long>& Profile,const char* PlotName);
void GetColumnProfile(const IplImage* ImageOI, std::vector<unsigned long>& Profile);
void GetRowProfile(const IplImage* ImageOI, std::vector<unsigned long>& Profile);
void GetColumnProfile(const IplImage* ImageOI,const CvRect& ROI, std::vector<unsigned long>& Profile);
void GetRowProfile(const IplImage* ImageOI,const CvRect& ROI, std::vector<unsigned long>& Profile);
IplImage* GetROI(const IplImage* ImageOI,const CvRect& ROI);
void WriteTextOnImage(IplImage* ImageOI,const std::string& StringOI,const CvScalar Colour,const CvPoint& Location);
void PlotInUpScaledImage(IplImage* PlotImage,CvScalar color,const CvPoint& PointOI,const CvSize& OrgFrameSize);
void SetImage(IplImage* ImageOI,unsigned char Value);
unsigned char GetPixel(const IplImage* ImageOI,int x, int y);
void SetPixel(IplImage* ImageOI,unsigned char Value,int x, int y);
IplImage* GetColouredImage(IplImage* ImageOI,const CvScalar Colour);
IplImage* GetColouredImage(IplImage* ImageOI,const std::vector<CvScalar>& Colours);
void SetImageBorders(IplImage* ImageOI,unsigned long BorderWidth,unsigned long BorderHeight,unsigned char Value);

unsigned long GetGraySum(const IplImage* ImageOI,const CvRect& ROI);


void Convert16STo8U(const IplImage* ImageOI,IplImage* Result,unsigned short Threshold);
void JoinImagesHorizontallyAligned(const IplImage* LeftImage,const IplImage* RightImage,IplImage** ImageOI);
IplImage* GetColouredBinaryImage(IplImage* ImageOI,const CvScalar FalseColour,const CvScalar TrueColour);
IplImage* GetFrom16SColouredBinaryImage(IplImage* ImageOI,unsigned short Threshold,const CvScalar FalseColour,const CvScalar TrueColour);
void ReduceResolution(IplImage* ImageOI,unsigned long Resolution);
IplImage* GetBinaryImage(const IplImage* ImageOI,unsigned char TrueValue);
void AdjustBinaryTrueValues(IplImage* ImageOI,unsigned char TrueValue);
void PlotHistogramOnXAxis(const std::vector<unsigned int>& Histogram,int BinWidth,int BinHeight,const CvScalar CanvasColour,const CvScalar PlotColour,const char* PlotName);
void PlotHistogramOnYAxis(const std::vector<unsigned int>& Histogram,int BinWidth,int BinHeight,const CvScalar CanvasColour,const CvScalar PlotColour,const char* PlotName);
IplImage* GetBikeCornersDifferenceImage(const IplImage* OldImage,const IplImage* NewImage,unsigned char ValueOI);
IplImage* GetSobelVerticalEdges(const IplImage* Image);
IplImage* GetScharrVerticalEdges(const IplImage* Image);
void GetAndImage(const IplImage* Image1,IplImage* Image2);
IplImage* GetTrueImagePixels(const IplImage* ImageOI,const IplImage* BinaryImage,unsigned char FalseValue);
IplImage* CreateImageForViewing16UImage(const IplImage* ImageOI);
void GetTrueImagePixels(const IplImage* ImageOI,const IplImage* BinaryImage,unsigned char FalseValue,IplImage** ResultImage);

    // Check

bool CheckROI(const IplImage* ImageOI,const CvRect& ROI);
void ExtendROI(const CvRect& ROIToExtend,const CvSize& FrameSize,unsigned long ExtensionWidth,unsigned long ExtensionHeight,CvRect& ROIExtended);
bool AreImagesEqual(const IplImage* Image1,const IplImage* Image2);

    // Functions to evaluate image-properties

void GetContrast(const IplImage* ImageOI,const CvRect& ROI,unsigned long& Contrast,unsigned long& Average);
//xy* GetCorners(const IplImage* ImageOI,const CvRect& ROI,unsigned char Threshold,unsigned long& NumberOfCorners);
//xy* GetCorners(const IplImage* ImageOI,unsigned char Threshold,unsigned long& NumberOfCorners);
bool InROI(const CvRect& ROI,int x, int y);

    // Functions on ROI's

unsigned long GetInterBlockCityBlockDistance(const CvRect& ROI1,const CvRect& ROI2);
CvPoint GetInterBlockCityBlockDistances(const CvRect& ROI1,const CvRect& ROI2);
CvPoint GetCityBlockDistances(const CvRect& ROI,const CvPoint& PointOI);

void ClipROI(const CvSize& FrameSize,CvRect& ROI);
void GetIntersection(const CvRect& ROI1,const CvRect& ROI2,CvRect& ROI);
void GetMinimum(CvRect& ROI1,const CvRect& ROI2,CvRect& ROI);
bool AreEqual(const CvRect& ROI1,const CvRect& ROI2);
CvPoint GetCenter(const CvRect& ROI);
bool IsIncluded(const CvRect& ROI,const CvPoint& PointOI);
int GetSquaredDistance(const CvPoint& PointOI1,const CvPoint& PointOI2);
void SetROI(const CvPoint& PointOI1,const CvPoint& PointOI2,CvRect& ROI);
bool IsTouchingImageBorders(const CvRect& ROI,unsigned long ImageWidth,unsigned long ImageHeight);
CvPoint GetMinimalDistanceToImageBorders(const CvRect& ROI,unsigned long ImageWidth,unsigned long ImageHeight);
void GetMinimumDistanceToImageBorder(const CvRect& ROI,unsigned long ImageWidth,unsigned long ImageHeight,int& XDistance,int& YDistance);

void SetImage(const vipnt::OfflineFrame* SourceImage,IplImage* DestinationImage);
void SetImage(const unsigned char* SourceImage,IplImage* DestinationImage);
void SetImage(const int* SourceImage,IplImage* DestinationImage);
void SetImage(const bool* SourceImage,IplImage* DestinationImage);
void SetImageRescaled(const vipnt::OfflineFrame* SourceImage,IplImage* DestinationImage,int MaximumValue);

IplImage* Get33CensusTransFormedImage(const IplImage* Image);
IplImage* GetBdcGradientImage(const IplImage* Image);

#endif // OPENCVUTILITIES_H
