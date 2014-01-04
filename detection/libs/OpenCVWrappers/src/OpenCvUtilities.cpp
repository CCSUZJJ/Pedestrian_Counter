#include "OpenCvUtilities.h"
using namespace std;

// Get a random-color
//CvScalar color = CV_RGB( rand()&255, rand()&255, rand()&255 );

// Returns true if the width and the height of both images match

bool CheckImagesSizes(const IplImage* Image1,const IplImage* Image2)
{
    if (Image1 == 0) return false;
    if (Image2 == 0) return false;

    if (Image2->width != Image1->width) return false;
    return (Image2->height == Image1->height);
}

// Returns true if the width, height, pixeldimensions and channel
// of both images match

bool CheckImageFootPrints(const IplImage* Image1,const IplImage* Image2)
{
    if (!CheckImagesSizes(Image1,Image2)) return false;

    if (Image1->widthStep != Image2->widthStep) return false;
    if (Image1->depth != Image2->depth) return false;
    return (Image1->nChannels == Image2->nChannels);
}

bool InROI(const CvRect& ROI,int x, int y)
{
    return ((x >=ROI.x)
            && (x < (ROI.x+ROI.width))
            && (y >=ROI.y)
            && (y < (ROI.y+ROI.height)));
}

bool IsTouchingImageBorders(const CvRect& ROI,unsigned long ImageWidth,unsigned long ImageHeight)
{
    if (ROI.x == 0) return true;
    if (ROI.y == 0) return true;
    if ((ROI.x + ROI.width) == (int)(ImageWidth)) return true;
    return (ROI.y + ROI.height) == (int)(ImageHeight);
}

CvPoint GetMinimalDistanceToImageBorders(const CvRect& ROI,unsigned long ImageWidth,unsigned long ImageHeight)
{
    CvPoint Distance;
    Distance.x = ROI.x;
    Distance.y = ROI.y;

    // Right distance ?

    int RightDistance = ImageWidth - (ROI.x + ROI.width);
    if (RightDistance <  Distance.x) Distance.x = RightDistance;

    // Bottom distance ?

    int BottomDistance = ImageHeight- (ROI.y + ROI.height);
    if (BottomDistance <  Distance.y) Distance.y = BottomDistance;

    // return distance

    return Distance;
}


void GetMinimumDistanceToImageBorder(const CvRect& ROI,unsigned long ImageWidth,unsigned long ImageHeight,int& XDistance,int& YDistance)
{
    XDistance = ROI.x;
    int DistanceOI = ImageWidth - (ROI.x + ROI.width);
    if (DistanceOI < XDistance) XDistance = DistanceOI;
    YDistance = ROI.y;
    DistanceOI = ImageHeight - (ROI.y + ROI.height);
    if (DistanceOI < YDistance) YDistance = DistanceOI;
}

void SmoothInputImage(IplImage* YFrame)
{
    CvSize FrameSize = cvGetSize(YFrame);
    unsigned char* YPointer = (unsigned char*) YFrame->imageData;
    for(int y = 0; y < FrameSize.height; ++y)
    {
        unsigned char* YData = YPointer;
        for(int x = 0; x < FrameSize.width; ++x)
        {
            int Val = 0;
            int Count = 0;
            for(int Y2 = std::max(0,y-2); Y2 < std::min(y+2,FrameSize.height); ++Y2)
            {
                for(int X2 = std::max(0,x-2); X2 < std::min(x+2,FrameSize.width); ++X2)
                {
                    Val += *(YData + Y2*FrameSize.width + X2) ;
                    ++Count;
                }
            }
            *(YData + y*FrameSize.width + x) = Val / Count;
        }
    }
}

void GetYFromRGB(const IplImage* RGBFrame,IplImage* YFrame)
{
    if (RGBFrame == 0) return;
    if (YFrame == 0) return;
    if (RGBFrame->depth != IPL_DEPTH_8U) return;    // Invalid image-format
    if (RGBFrame->nChannels != 3) return;           // Invalid image-format
    if (YFrame->depth != IPL_DEPTH_8U) return;      // Invalid image-format
    if (YFrame->nChannels != 1) return;             // Invalid image-format

    CvSize FrameSize = cvGetSize(RGBFrame);

    unsigned char* YPointer = (unsigned char*) YFrame->imageData;
    const unsigned char* RGBPointer = (const unsigned char*)RGBFrame->imageData;

    for(int y = 0; y < FrameSize.height; ++y)
    {
        unsigned char* YData = YPointer;
        const unsigned char* RGBData = RGBPointer;
        for(int x = 0; x < FrameSize.width; ++x)
        {
            // blue channel can now be accessed with ptr[0]
            // green channel can now be accessed with ptr[1]
            // red channel can now be accessed with ptr[2]

            // Y = 0.299 *  R + 0.587 * G + 0.114 * B
            // if divide by 1024 iso 1000
            // Y = (306 * R + 601 * G + 117 * B)/1024

            int Result = (306 * RGBData[2] + 601 * RGBData[1] + 117 * RGBData[0])>>10;
            if (Result < 0) Result = 0;
            if (Result > 255) Result = 255;

            *YData++ = (unsigned char)Result;
            RGBData += 3;
        }
        YPointer += YFrame->widthStep;
        RGBPointer += RGBFrame->widthStep;
    }

    //SmoothInputImage(YFrame);
}

void WriteTextOnImage(IplImage* ImageOI,const std::string& StringOI,const CvScalar Colour,const CvPoint& Location)
{
    if (ImageOI == 0) return;

    int fontFace = CV_FONT_HERSHEY_SCRIPT_SIMPLEX;
    double fontScale = 0.5;
    int thickness = 1;
    int baseline = 0;

    CvFont font;
    cvInitFont(&font, fontFace,fontScale, fontScale, 0, thickness, CV_AA);
    cv::Size TextSize = cv::getTextSize(StringOI, fontFace, fontScale, thickness, &baseline);

    CvPoint LocationOI = Location;

    if ((LocationOI.x+TextSize.width) > ImageOI->width) LocationOI.x = ImageOI->width - TextSize.width;
    if ((LocationOI.y-TextSize.height) < 0) LocationOI.y = TextSize.height;
    if (LocationOI.y>= ImageOI->height) LocationOI.y = ImageOI->height-1;
    cvPutText(ImageOI, StringOI.c_str(), LocationOI, &font, Colour);
}

void PlotProfile(const std::vector<unsigned long>& Profile,const char* PlotName)
{
    unsigned long SizeProfile = (unsigned long) Profile.size();
    if (SizeProfile == 0) return;  // can't plot ...
    unsigned long Maximum = *std::max_element(Profile.begin(), Profile.end());
    if (Maximum == 0) return;      // can't plot ...

    unsigned long BoxWidth = 200;
    if (SizeProfile > BoxWidth) BoxWidth = SizeProfile;
    unsigned long BoxHeight = 200;
    IplImage* PlotImage = cvCreateImage(cvSize(BoxWidth,BoxHeight), IPL_DEPTH_32F, 1);

    cvRectangle(PlotImage, cvPoint(0,0),cvPoint(BoxWidth,BoxHeight),CV_RGB(255,255,255),-1);
    unsigned long Offset = 0;
    unsigned long PreviousValuePixels = 0;
    for (std::vector<unsigned long>::const_iterator it = Profile.begin(); it != Profile.end();it++)
    {
        unsigned long Value = *it;
        unsigned long ValuePixels = cvRound(Value * BoxHeight /Maximum);
        if (Offset > 0)
        {
            unsigned long PlotOffset1 = Offset-1;
            unsigned long PlotOffset2 = Offset;
            if (BoxWidth > (2* SizeProfile))
            {
                PlotOffset1 = ((Offset-1) * BoxWidth)/ SizeProfile;
                PlotOffset2 = (Offset * BoxWidth)/ SizeProfile;
            }

            cvLine(PlotImage, cvPoint(PlotOffset1,BoxHeight-PreviousValuePixels), cvPoint(PlotOffset2,BoxHeight-ValuePixels), CV_RGB(0,0xff,0));
        }

        PreviousValuePixels = ValuePixels;
        Offset++;
    }

    cvShowImage(PlotName, PlotImage);
    cvReleaseImage(&PlotImage);
}

void PlotHistogramOnXAxis(const std::vector<unsigned int>& Histogram,int BinWidth,int BinHeight,const CvScalar CanvasColour,const CvScalar PlotColour,const char* PlotName)
{
    unsigned long SizeHistogram = (unsigned long) Histogram.size();
    if (SizeHistogram == 0) return;  // can't plot ...
    unsigned long Maximum = *std::max_element(Histogram.begin(), Histogram.end());
    if (Maximum == 0) return;      // can't plot ...

    // Determine Canvas
    // Width = (SizeHistogram+2) * BinWidth;
    // Height = BinHeight + (2 * 20)

    int OffsetHeight = 20;
    int CanvasWidth = (SizeHistogram +2) * BinWidth;
    int CanvasHeight = BinHeight + 2 * OffsetHeight;

    IplImage* PlotImage = cvCreateImage(cvSize(CanvasWidth,CanvasHeight), IPL_DEPTH_32F, 3);
    if (PlotImage != 0)
    {
        // Set canvas-colour

        cvSet(PlotImage,CanvasColour);

        // Draw the bins

        int BinNumber = 0;
        for (std::vector<unsigned int>::const_iterator it = Histogram.begin(); it != Histogram.end();it++)
        {
            unsigned long Value = *it;
            unsigned long ValuePixels = cvRound(Value * BinHeight /Maximum);

            CvPoint Top;
            CvPoint Bottom;

            Top.x = BinWidth + BinNumber*BinWidth;
            Bottom.x = Top.x + BinWidth;
            Bottom.y = CanvasHeight-OffsetHeight;
            Top.y = Bottom.y - ValuePixels;
            cvRectangle(PlotImage,Top,Bottom,PlotColour,CV_FILLED);
            BinNumber++;
        }

        cvShowImage(PlotName, PlotImage);
        cvReleaseImage(&PlotImage);
    }
}

void PlotHistogramOnYAxis(const std::vector<unsigned int>& Histogram,int BinWidth,int BinHeight,const CvScalar CanvasColour,const CvScalar PlotColour,const char* PlotName)
{
    unsigned long SizeHistogram = (unsigned long) Histogram.size();
    if (SizeHistogram == 0) return;  // can't plot ...
    unsigned long Maximum = *std::max_element(Histogram.begin(), Histogram.end());
    if (Maximum == 0) return;      // can't plot ...

    // Determine Canvas
    // Height = (SizeHistogram+2) * BinWidth;
    // Width = BinHeight + (2 * 20)

    int OffsetWidth = 20;
    int CanvasHeight = (SizeHistogram +2) * BinWidth;
    int CanvasWidth = BinHeight + 2 * OffsetWidth;

    IplImage* PlotImage = cvCreateImage(cvSize(CanvasWidth,CanvasHeight), IPL_DEPTH_32F, 3);
    if (PlotImage != 0)
    {
        // Set canvas-colour

        cvSet(PlotImage,CanvasColour);

        // Draw the bins

        int BinNumber = 0;
        for (std::vector<unsigned int>::const_iterator it = Histogram.begin(); it != Histogram.end();it++)
        {
            unsigned long Value = *it;
            unsigned long ValuePixels = cvRound(Value * BinHeight /Maximum);

            CvPoint Top;
            CvPoint Bottom;

            Top.x = OffsetWidth;
            Bottom.x = Top.x + ValuePixels;
            Top.y = BinWidth + BinNumber*BinWidth;
            Bottom.y = Top.y - BinWidth;
            cvRectangle(PlotImage,Top,Bottom,PlotColour,CV_FILLED);
            BinNumber++;
        }

        cvShowImage(PlotName, PlotImage);
        cvReleaseImage(&PlotImage);
    }
}

unsigned long GetGraySum(const IplImage* ImageOI)
{
    unsigned long Sum = 0;
    if (ImageOI == 0) return Sum;
    CvSize FrameSize = cvGetSize(ImageOI);

    const unsigned char* Row = (unsigned char*)ImageOI->imageData;
    for (int y=0;y < FrameSize.height;y++)
    {
        const unsigned char* Data = Row;
        for (int x=0;x < FrameSize.width; x++) Sum += *Data++;
        Row +=  ImageOI->widthStep;
    }
    return Sum;
}

unsigned long GetGraySum(const IplImage* ImageOI, const CvRect& ROI)
{
    unsigned long Sum = 0;
    if (ImageOI == 0) return Sum;

    const unsigned char* Row = (unsigned char*)ImageOI->imageData;
    Row += ROI.y * ImageOI->widthStep;
    Row += ROI.x;

    for (int y=0; y < ROI.height; y++)
    {
        const unsigned char* Data = Row;
        for (int x=0; x < ROI.width; x++) Sum += *Data++;
        Row += ImageOI->widthStep;
    }
    return Sum;
}

// Sum taken over the Columns

void GetColumnProfile(const IplImage* ImageOI,vector<unsigned long>& Profile)
{
    if (ImageOI == 0) return;
    CvSize FrameSize = cvGetSize(ImageOI);

    // Setup profile

    int SizeProfile = FrameSize.width;
    Profile.assign(SizeProfile, 0);

    const unsigned char* Row = (unsigned char*)ImageOI->imageData;
    for (int y=0;y < FrameSize.height;y++)
    {
        const unsigned char* Data = Row;
        for (int x=0;x < FrameSize.width; x++) Profile[x] += *Data++;
        Row +=  ImageOI->widthStep;
    }
}

// Sum taken over the Columns

void GetColumnProfile(const IplImage* ImageOI,const CvRect& ROI, vector<unsigned long>& Profile)
{
    if (ImageOI == 0) return;

    // Setup profile

    int SizeProfile = ROI.width;
    Profile.assign(SizeProfile, 0);

    const unsigned char* Row = (unsigned char*)ImageOI->imageData;
    Row += ROI.y * ImageOI->widthStep;
    Row += ROI.x;

    for (int y=0;y < ROI.height;y++)
    {
        const unsigned char* Data = Row;
        for (int x=0;x < ROI.width; x++) Profile[x] += *Data++;
        Row +=  ImageOI->widthStep;
    }
}

// Sum taken over the Rows

void GetRowProfile(const IplImage* ImageOI,const CvRect& ROI, vector<unsigned long>& Profile)
{
    if (ImageOI == 0) return;

    // Setup profile

    int SizeProfile = ROI.height;
    Profile.assign(SizeProfile, 0);

    const unsigned char* Row = (unsigned char*)ImageOI->imageData;
    Row += ROI.y * ImageOI->widthStep;
    Row += ROI.x;

    for (int y=0;y < ROI.height;y++)
    {
        const unsigned char* Data = Row;
        for (int x=0;x < ROI.width; x++) Profile[y] += *Data++;
        Row +=  ImageOI->widthStep;
    }
}

// Sum taken over the Rows

void GetRowProfile(const IplImage* ImageOI, vector<unsigned long>& Profile)
{
    if (ImageOI == 0) return;
    CvSize FrameSize = cvGetSize(ImageOI);

    // Setup profile

    int SizeProfile = FrameSize.height;
    Profile.assign(SizeProfile, 0);

    const unsigned char* Row = (unsigned char*)ImageOI->imageData;

    for (int y=0;y < FrameSize.height;y++)
    {
        const unsigned char* Data = Row;
        for (int x=0;x < FrameSize.width; x++) Profile[y] += *Data++;
        Row +=  ImageOI->widthStep;
    }
}


void DrawCross(IplImage* ImageOI,CvScalar Colour,unsigned long Size,unsigned long x, unsigned long y)
{
    if (ImageOI == 0) return;
    if (ImageOI->depth != IPL_DEPTH_8U) return;     // Invalid image-format
    if (ImageOI->nChannels != 3) return;			// Invalid image-format
    CvSize FrameSize = cvGetSize(ImageOI);

    if ((x >= (unsigned long)FrameSize.width) || (y >= (unsigned long)FrameSize.height)) return;
    cvSet2D(ImageOI,y,x,Colour);

    for (unsigned long i = 1; i <= Size;i++)
    {
        if (x >= i) cvSet2D(ImageOI,y,x-i,Colour);
        if ((x +i) < (unsigned long)FrameSize.width) cvSet2D(ImageOI,y,x+i,Colour);
        if (y >= i) cvSet2D(ImageOI,y-i,x,Colour);
        if ((y +i) < (unsigned long)FrameSize.height) cvSet2D(ImageOI,y+i,x,Colour);
    }
}

IplImage* GetROI(const IplImage* ImageOI,const CvRect& ROI)
{
    if (ImageOI == 0) return 0;
    CvSize FrameSize;

    // Determine frame-size (truncate if appropriate)

    if ((ROI.x+ROI.width) <= ImageOI->width) FrameSize.width = ROI.width;
    else FrameSize.width = ImageOI->width-ROI.x;
    if (FrameSize.width <= 0) return 0;

    if ((ROI.y+ROI.height) <= ImageOI->height) FrameSize.height = ROI.height;
    else FrameSize.height = ImageOI->height-ROI.y;
    if (FrameSize.height <= 0) return 0;

    // Create image

    IplImage* ImageROI = cvCreateImage(FrameSize,IPL_DEPTH_8U,1);

    const unsigned char* Row = (unsigned char*)ImageOI->imageData;
    unsigned char* ToRow = (unsigned char*)ImageROI->imageData;
    Row += ROI.y * ImageOI->widthStep;
    Row += ROI.x;

    for (int y=0;y < ROI.height;y++)
    {
        const unsigned char* Data = Row;
        unsigned char* To = ToRow;
        for (int x=0;x < ROI.width; x++) *To++ = *Data++;
        Row +=  ImageOI->widthStep;
        ToRow += ImageROI->widthStep;
    }

    return ImageROI;
}

void PlotInUpScaledImage(IplImage* PlotImage,CvScalar Colour,const CvPoint& PointOI,const CvSize& OrgFrameSize)
{
    if (PlotImage == 0) return;
    if (PlotImage->depth != IPL_DEPTH_8U) return;       // Invalid image-format
    if (PlotImage->nChannels != 3) return;              // Invalid image-format
    CvSize FrameSize = cvGetSize(PlotImage);
    if (FrameSize.width == 0) return;
    if (FrameSize.height == 0) return;


    if (FrameSize.width < OrgFrameSize.width) return;   // Can't upscale
    if (FrameSize.height < OrgFrameSize.height) return; // Can't upscale

    // Determine scaling-coeff

    int x = (PointOI.x * FrameSize.width)/OrgFrameSize.width;
    if (x > (FrameSize.width-1)) x = FrameSize.width-1;
    int y = (PointOI.y * FrameSize.height)/OrgFrameSize.height;
    if (y > (FrameSize.height-1)) y = FrameSize.height-1;

    // Plot

    cvSet2D(PlotImage,y,x,Colour);
}

void SetImage(IplImage* ImageOI,unsigned char Value)
{
    if (ImageOI == 0) return;
    if (ImageOI->depth != IPL_DEPTH_8U) return;	// Invalid image-format
    CvSize FrameSize = cvGetSize(ImageOI);

    unsigned char* Row1 = (unsigned char*)ImageOI->imageData;

    for (int y=0;y < FrameSize.height;y++)
    {
        unsigned char* Data1 = Row1;
        for (int x=0;x < FrameSize.width; x++) *Data1++ = Value;
        Row1 += ImageOI->widthStep;
    }
}

void SetPixel(IplImage* ImageOI,unsigned char Value,int x, int y)
{
    if (ImageOI == 0) return;
    if (ImageOI->depth != IPL_DEPTH_8U) return;	// Invalid image-format
    if (ImageOI->nChannels != 1) return;        // Invalid image-format

    CvSize FrameSize = cvGetSize(ImageOI);
    if ((x < 0) || (x >=  FrameSize.width)) return;
    if ((y < 0) || (y >=  FrameSize.height)) return;


    unsigned char* Data = (unsigned char*)ImageOI->imageData + y * ImageOI->widthStep + x;
    *Data = Value;
}

unsigned char GetPixel(const IplImage* ImageOI,int x, int y)
{
    if (ImageOI == 0) return 0;
    if (ImageOI->depth != IPL_DEPTH_8U) return 0;	// Invalid image-format
    if (ImageOI->nChannels != 1) return 0;        // Invalid image-format

    CvSize FrameSize = cvGetSize(ImageOI);
    if ((x < 0) || (x >=  FrameSize.width)) return 0;
    if ((y < 0) || (y >=  FrameSize.height)) return 0;

    unsigned char* Data = (unsigned char*)ImageOI->imageData + y * ImageOI->widthStep + x;
    return *Data;
}

bool CheckROI(const IplImage* ImageOI,const CvRect& ROI)
{
    if (ImageOI == 0) return false;
    if (ROI.width == 0) return false;
    if (ROI.height == 0) return false;

    if ((ROI.x < 0) || ((ROI.x + ROI.width) > ImageOI->width)) return false;
    return ((ROI.y >= 0) && ((ROI.y + ROI.height) <= ImageOI->height));
}

// The contrast is the squared stddev

void GetContrast(const IplImage* ImageOI,const CvRect& ROI,unsigned long& Contrast,unsigned long& Average)
{
    Contrast = 0;
    Average = 0;

    if (!CheckROI(ImageOI,ROI)) return;   // ROI-spec error
    if (ImageOI == 0) return;

    unsigned long Sum = 0;
    unsigned long SumSquared = 0;
    unsigned long NumberOfPixels = ROI.width * ROI.height;

    const unsigned char* Data = (const unsigned char*) ImageOI->imageData;
    Data += (ImageOI->widthStep * ROI.y) + ROI.x;

    // Std = (sum x  (sumx)/N)/N

    for (int i=0;i<ROI.height;i++)
    {
        const unsigned char* RowData = Data;
        for (int j=0;j<ROI.width;j++)
        {
            unsigned long Value = *RowData++;
            Sum += Value;
            SumSquared += Value*Value;
        }

        // Next row

        Data += ImageOI->widthStep;
    }

    // Contrast = stdev**2

    unsigned long RoundingFactor = NumberOfPixels/2;
    Contrast = ((SumSquared - (Sum * Sum)/NumberOfPixels) + RoundingFactor) / NumberOfPixels;
    Average = (Sum + RoundingFactor) / NumberOfPixels;
}


// Fast-corner-detection (fast9_detect) in a region_of_interest of the image. After
// finding the corners, the coordinates of the corners are transformed from ROI-coordinates
// into real image-coordinates

/*xy* GetCorners(const IplImage* ImageOI,const CvRect& ROI,unsigned char Threshold,unsigned long& NumberOfCorners)
{
    NumberOfCorners = 0;
    if (!CheckROI(ImageOI,ROI)) return 0;   // ROI-spec error
    if (ImageOI == 0) return 0;
    if (ImageOI->depth != IPL_DEPTH_8U) return 0;	// Invalid image-format

    const unsigned char* Data = (const unsigned char*) ImageOI->imageData;
    Data += (ImageOI->widthStep * ROI.y) + ROI.x;

    int NumberOfCornersFound = 0;
    xy* Corners = fast9_detect((const byte*)Data, ROI.width, ROI.height, ImageOI->widthStep, Threshold, &NumberOfCornersFound);

    // Adjust coordinates of the corners found to image-coordinates

    for (int i=0;i<NumberOfCornersFound;i++)
    {
        Corners[i].x += ROI.x;
        Corners[i].y += ROI.y;
    }

    NumberOfCorners = (unsigned long)NumberOfCornersFound;
    return Corners;
}

// Fast-corner-detection (fast9_detect)

xy* GetCorners(const IplImage* ImageOI,unsigned char Threshold,unsigned long& NumberOfCorners)
{
    NumberOfCorners = 0;
    if (ImageOI == 0) return 0;
    if (ImageOI->depth != IPL_DEPTH_8U) return 0;	// Invalid image-format

    const unsigned char* Data = (const unsigned char*) ImageOI->imageData;
    int NumberOfCornersFound = 0;
    xy* Corners = fast9_detect((const byte*)Data, ImageOI->width,ImageOI->height, ImageOI->widthStep, Threshold, &NumberOfCornersFound);
    NumberOfCorners = (unsigned long)NumberOfCornersFound;
    return Corners;
}
*/

// Converts a signed image into a 8U-image
// Make absolute -> threshold -> [0 ,255]

void Convert16STo8U(const IplImage* ImageOI,IplImage* Result,unsigned short Threshold)
{
    if (ImageOI == 0) return;
    if (ImageOI->depth != (int)IPL_DEPTH_16S) return;    // Invalid image-format
    if (ImageOI->nChannels != 1) return;			// Invalid image-format

    if (Result == 0) return;
    if (Result->depth != IPL_DEPTH_8U) return;      // Invalid image-format
    if (Result->nChannels != 1) return;             // Invalid image-format

    CvSize FrameSize = cvGetSize(ImageOI);
    if (FrameSize.width != Result->width) return;   // Size doesn't match
    if (FrameSize.height != Result->height) return;   // Size doesn't match

    const signed short* SourceImage = (signed short*) ImageOI->imageData;
    unsigned char* DestinationImage = (unsigned char*) Result->imageData;
    signed short PositiveThreshold = Threshold;
    signed short NegativeThreshold = -Threshold;


    for (int x=0;x<FrameSize.height;x++)
    {
        const signed short* DataIn = SourceImage;
        unsigned char* DataOut = DestinationImage;
        for (int y=0;y<FrameSize.width;y++)
        {
            signed short Value = *DataIn++;
            if (Value < 0) *DataOut++ = (Value < NegativeThreshold) ? 0xff : 0x0;
            else *DataOut++ = (Value > PositiveThreshold) ? 0xff : 0x0;
        }

        SourceImage += ImageOI->widthStep/sizeof(unsigned short);
        DestinationImage += Result->widthStep;
    }

}

IplImage* GetColouredImage(IplImage* ImageOI,const CvScalar Colour)
{
    if (ImageOI == 0) return 0;
    if (ImageOI->depth != IPL_DEPTH_8U) return 0;	// Invalid image-format
    CvSize FrameSize = cvGetSize(ImageOI);

    IplImage* ImageColoured = cvCreateImage(FrameSize,IPL_DEPTH_8U,3);
    cvZero(ImageColoured);

    const unsigned char* Row = (unsigned char*)ImageOI->imageData;

    for (int y=0;y < FrameSize.height;y++)
    {
        const unsigned char* Data = Row;
        for (int x=0;x < FrameSize.width; x++)
        {
            if (*Data++ != 0)
            {
                cvSet2D(ImageColoured,y,x,CV_RGB(*Data,*Data,*Data));
            }
        }

        Row +=  ImageOI->widthStep;
    }

    return ImageColoured;
}

// Extend a given ROI with a certain specified Width and height on both sides. The ROI is always
// extended, but the ROI will be shifted such that the ROI fits into the image-boundaries

void ExtendROI(const CvRect& ROIToExtend, const CvSize& FrameSize, unsigned long ExtensionWidth, unsigned long ExtensionHeight, CvRect& ROIExtended)
{
    // Extend width

    int XStart = ROIToExtend.x;
    int XEnd = XStart + ROIToExtend.width - 1;

    if (XStart >= (int)ExtensionWidth) XStart -= ExtensionWidth;
    else
    {
        XEnd += ExtensionWidth - XStart;
        XStart = 0;
    }

    if ((XEnd + static_cast<int>(ExtensionWidth)) < FrameSize.width) XEnd += ExtensionWidth;
    else
    {
        int ExtensionRemain = XEnd + ExtensionWidth - FrameSize.width + 1;
        XEnd = FrameSize.width-1;
        if (XStart > ExtensionRemain) XStart -= ExtensionRemain;
        else XStart = 0;
    }

    // Extend Height

    int YStart = ROIToExtend.y;
    int YEnd = YStart+ROIToExtend.height-1;

    if (YStart >= (int)ExtensionHeight) YStart -= ExtensionHeight;
    else
    {
        YEnd += ExtensionHeight - YStart;
        YStart = 0;
    }

    if ((YEnd + static_cast<int>(ExtensionHeight)) < FrameSize.height) YEnd += ExtensionHeight;
    else
    {
        int ExtensionRemain = YEnd + ExtensionHeight - FrameSize.height + 1;
        YEnd = FrameSize.height-1;
        if (YStart > ExtensionRemain) YStart -= ExtensionRemain;
        else YStart = 0;
    }

    // Set ROI-extended

    ROIExtended.x = XStart;
    ROIExtended.width = XEnd - XStart + 1;
    ROIExtended.y = YStart;
    ROIExtended.height = YEnd - YStart + 1;
}


// Get the cityblockdistance between two blocks. If the blocks do overlap, then
// the cityblockdistance = 0, else the shortest cityblockdistance is returned
// of the sides of the blocks that are closest to each other
//
//   +---+
//   |   |
//   |   |<-------->+---+
//   +---+          |   |
//                  |   |
//                  +---+
//
//   +---+
//   |   |
//   |   |
//   +---+
//       ^
//       |
//       *--------->+---+
//                  |   |
//                  |   |
//                  +---+
//

unsigned long GetInterBlockCityBlockDistance(const CvRect& ROI1,const CvRect& ROI2)
{
    // Try to figure out the relative location of the boundingboxes
    // to each other

    const int XLeft1 = ROI1.x;
    const int XRight1 = ROI1.x + ROI1.width - 1;
    const int YTop1 = ROI1.y;
    const int YBottom1 = ROI1.y + ROI1.height - 1;

    const int XLeft2 = ROI2.x;
    const int XRight2 = ROI2.x + ROI2.width - 1;
    const int YTop2 = ROI2.y;
    const int YBottom2 = ROI2.y + ROI2.height - 1;
    unsigned long Distance = 0;

    if (XLeft1 >= XRight2)
    {
        // Box2 is to the left of box1

        Distance = XLeft1 - XRight2;
    } else if (XLeft2 >= XRight1)
    {
        // Box2 is to the right of box1

        Distance = XLeft2 - XRight1;
    }

    if (YTop1 >= YBottom2) Distance += (YTop1 - YBottom2);
    else if (YTop2 >= YBottom1) Distance += (YTop2 - YBottom1);
    return Distance;
}


// Get the cityblockdistance between two blocks. If the blocks do overlap, then
// the cityblockdistance = 0, else the shortest cityblockdistance is returned
// of the sides of the blocks that are closest to each other
//
//   +---+
//   |   |
//   |   |<-------->+---+
//   +---+          |   |
//                  |   |
//                  +---+
//
//   +---+
//   |   |
//   |   |
//   +---+
//       ^
//       |
//       *--------->+---+
//                  |   |
//                  |   |
//                  +---+
//

CvPoint GetInterBlockCityBlockDistances(const CvRect& ROI1,const CvRect& ROI2)
{
    CvPoint Distances;
    Distances.x = 0;
    Distances.y = 0;

    // Try to figure out the relative location of the boundingboxes
    // to each other

    const int XLeft1 = ROI1.x;
    const int XRight1 = ROI1.x + ROI1.width - 1;
    const int YTop1 = ROI1.y;
    const int YBottom1 = ROI1.y + ROI1.height - 1;

    const int XLeft2 = ROI2.x;
    const int XRight2 = ROI2.x + ROI2.width - 1;
    const int YTop2 = ROI2.y;
    const int YBottom2 = ROI2.y + ROI2.height - 1;

    if (XLeft1 >= XRight2) Distances.x = XLeft1 - XRight2; // Box2 is to the left of box1
    else if (XLeft2 >= XRight1) Distances.x = XLeft2 - XRight1; // Box2 is to the right of box1

    if (YTop1 >= YBottom2) Distances.y = (YTop1 - YBottom2);
    else if (YTop2 >= YBottom1) Distances.y += (YTop2 - YBottom1);
    return Distances;
}


//       o
//       ^
//       |
//       *--------->+---+
//                  |   |
//                  |   |
//                  +---+
//
//       +------------------+
//       |    ^             |
//       |    |             |
//       |<---o             |
//       |                  |
//       |                  |
//       |                  |
//       +------------------+

CvPoint GetCityBlockDistances(const CvRect& ROI,const CvPoint& PointOI)
{
    CvPoint Distances;
    Distances.x = 0;
    Distances.y = 0;

    const int XRight = ROI.x + ROI.width - 1;
    const int YBottom = ROI.y + ROI.height - 1;

    if (ROI.x >= PointOI.x) Distances.x = ROI.x - PointOI.x;
    else if (XRight <= PointOI.x) Distances.x = PointOI.x - XRight;
    else Distances.x = std::min(PointOI.x-ROI.x,XRight-PointOI.x);

    if (ROI.y >= PointOI.y) Distances.y = ROI.y - PointOI.y;
    else if (YBottom <= PointOI.y) Distances.y = PointOI.y - YBottom;
    else Distances.y = std::min(PointOI.y-ROI.y,YBottom-PointOI.y);

    return Distances;
}


// Clip the ROI such that the ROI is in the given frame-size of an image

void ClipROI(const CvSize& FrameSize,CvRect& ROI)
{
    if ((ROI.width <= 0)
            || (ROI.height <= 0)
            || (ROI.x >= FrameSize.width)
            || (ROI.y >= FrameSize.height))
    {
        // Invalid ROI !

        ROI.x = 0;
        ROI.y = 0;
        ROI.width = 0;
        ROI.height = 0;
    }
    else
    {
        int XLeft = ROI.x;
        int XRight = ROI.x + ROI.width-1;
        int YTop = ROI.y;
        int YBottom = ROI.y + ROI.height-1;

        if (XLeft < 0) XLeft = 0;
        if (XRight < 0) XRight = 0;
        if (YTop < 0) YTop = 0;
        if (YBottom < 0) YBottom = 0;

        if (XRight >= FrameSize.width) XRight = FrameSize.width-1;
        if (YBottom >= FrameSize.height) YBottom = FrameSize.height-1;

        ROI.x = XLeft;
        ROI.width = XRight - XLeft + 1;
        ROI.y = YTop;
        ROI.height = YBottom - YTop + 1;
    }
}

// Calculate the intersection of ROI1 & ROI2. In case the ROI's don't overlap,
// then an invalid ROI is returned (all attributes set to zero)

void GetIntersection(const CvRect& ROI1,const CvRect& ROI2,CvRect& ROI)
{
    const int XLeft1 = ROI1.x;
    const int XRight1 = ROI1.x + ROI1.width - 1;
    const int YTop1 = ROI1.y;
    const int YBottom1 = ROI1.y + ROI1.height - 1;

    const int XLeft2 = ROI2.x;
    const int XRight2 = ROI2.x + ROI2.width - 1;
    const int YTop2 = ROI2.y;
    const int YBottom2 = ROI2.y + ROI2.height - 1;

    if ((XLeft1 <= XRight2)
            && (XLeft2 <= XRight1)
            && (YTop1 <= YBottom2)
            && (YTop2 <= YBottom1))
    {
        // Intersecting !

        int XLeft = (XLeft1 > XLeft2) ? XLeft1 : XLeft2;
        int XRight = (XRight1 < XRight2) ? XRight1 : XRight2;

        int YTop = (YTop1 > YTop2) ? YTop1 : YTop2;
        int YBottom = (YBottom1 < YBottom2) ? YBottom1 : YBottom2;

        ROI.x = XLeft;
        ROI.width = XRight - XLeft + 1;
        ROI.y = YTop;
        ROI.height = YBottom - YTop + 1;
    }
    else
    {
        // Not intersecting

        ROI.x = 0;
        ROI.y = 0;
        ROI.width = 0;
        ROI.height = 0;
    }
}

// Get the minimum ROI where both ROI's are part of

void GetMinimum(CvRect& ROI1,const CvRect& ROI2,CvRect& ROI)
{
    const int XLeft1 = ROI1.x;
    const int XRight1 = ROI1.x + ROI1.width - 1;
    const int YTop1 = ROI1.y;
    const int YBottom1 = ROI1.y + ROI1.height - 1;

    const int XLeft2 = ROI2.x;
    const int XRight2 = ROI2.x + ROI2.width - 1;
    const int YTop2 = ROI2.y;
    const int YBottom2 = ROI2.y + ROI2.height - 1;

    int XLeft = (XLeft1 < XLeft2) ? XLeft1 : XLeft2;
    int XRight = (XRight1 > XRight2) ? XRight1 : XRight2;

    int YTop = (YTop1 < YTop2) ? YTop1 : YTop2;
    int YBottom = (YBottom1 > YBottom2) ? YBottom1 : YBottom2;

    ROI.x = XLeft;
    ROI.width = XRight - XLeft + 1;
    ROI.y = YTop;
    ROI.height = YBottom - YTop + 1;
}

bool AreEqual(const CvRect& ROI1,const CvRect& ROI2)
{
    return ((ROI1.x == ROI2.x)
            && (ROI1.y == ROI2.y)
            && (ROI1.width == ROI2.width)
            && (ROI1.height == ROI2.height));
}

CvPoint GetCenter(const CvRect& ROI)
{
    CvPoint PointOI;
    PointOI.x = ROI.x + (ROI.width+1)/2;
    PointOI.y = ROI.y + (ROI.height+1)/2;
    return PointOI;
}

bool IsIncluded(const CvRect& ROI,const CvPoint& PointOI)
{
    return ((PointOI.x >= ROI.x)
            && (PointOI.x <= (ROI.x+ROI.width-1))
            && (PointOI.y >= ROI.y)
            && (PointOI.y <= (ROI.y+ROI.height-1)));
}


int GetSquaredDistance(const CvPoint& PointOI1,const CvPoint& PointOI2)
{
    int XDistance = PointOI1.x - PointOI2.x;
    int YDistance = PointOI1.y - PointOI2.y;
    return (unsigned long)(XDistance*XDistance + YDistance*YDistance);
}

void SetROI(const CvPoint& PointOI1,const CvPoint& PointOI2,CvRect& ROI)
{
    ROI.x = std::min(PointOI1.x,PointOI2.x);
    ROI.y = std::min(PointOI1.y,PointOI2.y);

    if (PointOI1.x > ROI.x)  ROI.width =  PointOI1.x - ROI.x + 1;
    else ROI.width =  PointOI2.x - ROI.x + 1;

    if (PointOI1.y > ROI.y)  ROI.height =  PointOI1.y - ROI.y + 1;
    else ROI.height =  PointOI2.y - ROI.y + 1;

}

// Check whether the target-image does exit or not. It the target-image
// exits, then check if it has the same size, if not, release the image
// and create a new one

void CreateImage(IplImage** ImageOI,const CvSize& FrameSize)
{
    if (ImageOI == 0) return;
    if (*ImageOI != 0)
    {
        if (((*ImageOI)->depth != IPL_DEPTH_8U)
                || ((*ImageOI)->nChannels != 1)
                || ((*ImageOI)->width != FrameSize.width)
                || ((*ImageOI)->height != FrameSize.height))
        {
            cvReleaseImage(ImageOI);
            *ImageOI = 0;
        }
    }

    if (*ImageOI == 0) *ImageOI = cvCreateImage(FrameSize,IPL_DEPTH_8U ,1);
}

void JoinImagesHorizontallyAligned(const IplImage* LeftImage,const IplImage* RightImage,IplImage** ImageOI)
{
    if (LeftImage == 0) return;
    if (RightImage == 0) return;
    if (ImageOI == 0) return;
    if (!CheckImageFootPrints(LeftImage,RightImage)) return;
    if (LeftImage->depth != IPL_DEPTH_8U) return;	// Invalid image-format
    if (LeftImage->nChannels != 1) return;			// Invalid image-format
    if (LeftImage->width == 0) return;				// Invalid image
    if (LeftImage->height == 0) return;			// Invalid image

    // Create image

    CvSize FrameSize = cvGetSize(LeftImage);
    CvSize TargetFrameSize;
    TargetFrameSize.width = FrameSize.width*2;
    TargetFrameSize.height = FrameSize.height;
    CreateImage(ImageOI,TargetFrameSize);
    if (*ImageOI == 0) return;	// No target-image available
    unsigned char* Destination = (unsigned char*)(*ImageOI)->imageData;
    const unsigned char* LeftSource =  (unsigned char*)LeftImage->imageData;
    const unsigned char* RightSource = (unsigned char*)RightImage->imageData;

    for (int y=0;y < FrameSize.height;y++)
    {
        unsigned char* DestinationData = Destination;

        const unsigned char* SourceData = LeftSource;
        for (int x=0;x < FrameSize.width; x++)*DestinationData++ = *SourceData++;

        SourceData = RightSource;
        for (int x=0;x < FrameSize.width; x++)*DestinationData++ = *SourceData++;

        LeftSource += LeftImage->widthStep;
        RightSource += RightImage->widthStep;
        Destination += (*ImageOI)->widthStep;
    }
}


IplImage* GetColouredImage(IplImage* ImageOI,const std::vector<CvScalar>& Colours)
{
    if (ImageOI == 0) return 0;
    if (ImageOI->depth != IPL_DEPTH_8U) return 0;	// Invalid image-format
    if (ImageOI->nChannels != 1) return 0;			// Invalid image-format
    if (Colours.size() < 256) return 0;             // Not all values can be mapped

    CvSize FrameSize = cvGetSize(ImageOI);

    IplImage* ImageColoured = cvCreateImage(FrameSize,IPL_DEPTH_8U,3);
    if (ImageColoured == 0) return 0;
    const unsigned char* Row = (unsigned char*)ImageOI->imageData;

    for (int y=0;y < FrameSize.height;y++)
    {
        const unsigned char* Data = Row;
        for (int x=0;x < FrameSize.width; x++) cvSet2D(ImageColoured,y,x,Colours[*Data++]);
        Row +=  ImageOI->widthStep;
    }

    return ImageColoured;
}




IplImage* GetColouredBinaryImage(IplImage* ImageOI,const CvScalar FalseColour,const CvScalar TrueColour)
{
    if (ImageOI == 0) return 0;
    if (ImageOI->depth != IPL_DEPTH_8U) return 0;	// Invalid image-format
    CvSize FrameSize = cvGetSize(ImageOI);

    IplImage* ImageColoured = cvCreateImage(FrameSize,IPL_DEPTH_8U,3);
    if (ImageColoured == 0) return 0;
    const unsigned char* Row = (unsigned char*)ImageOI->imageData;

    for (int y=0;y < FrameSize.height;y++)
    {
        const unsigned char* Data = Row;
        for (int x=0;x < FrameSize.width; x++)
        {
            if (*Data++ != 0) cvSet2D(ImageColoured,y,x,TrueColour);
            else cvSet2D(ImageColoured,y,x,FalseColour);
        }

        Row +=  ImageOI->widthStep;
    }

    return ImageColoured;
}

IplImage* GetFrom16SColouredBinaryImage(IplImage* ImageOI,unsigned short Threshold,const CvScalar FalseColour,const CvScalar TrueColour)
{
    if (ImageOI == 0) return 0;
    if (ImageOI->depth != (int)IPL_DEPTH_16S) return 0;    // Invalid image-format
    if (ImageOI->nChannels != 1) return 0;			// Invalid image-format

    CvSize FrameSize = cvGetSize(ImageOI);

    IplImage* ImageColoured = cvCreateImage(FrameSize,IPL_DEPTH_8U,3);
    const signed short* Row = (signed short*)ImageOI->imageData;
    signed short PositiveThreshold = Threshold;
    signed short NegativeThreshold = -Threshold;

    for (int y=0;y < FrameSize.height;y++)
    {
        const signed short* Data = Row;
        for (int x=0;x < FrameSize.width; x++)
        {
            signed short Value = *Data++;
            if (Value < 0)
            {
                if (Value < NegativeThreshold) cvSet2D(ImageColoured,y,x,TrueColour);
                else cvSet2D(ImageColoured,y,x,FalseColour);
            }
            else
            {
                if (Value > PositiveThreshold) cvSet2D(ImageColoured,y,x,TrueColour);
                else cvSet2D(ImageColoured,y,x,FalseColour);
            }

        }

        Row +=  ImageOI->widthStep/sizeof(unsigned short);
    }

    return ImageColoured;
}


// Mask bits in the image
// Resolution ; 1 .. 7
// Resolution 1 : image & 0x80      1 bit
// Resolution 2 : image & 0xc0      2 bits
// Resolution 3 : image & 0xe0      3 bits
// Resolution 4 : image & 0xf0      4 bits
// Resolution 5 : image & 0xf8      5 bits
// Resolution 6 : image & 0xfc      6 bits
// Resolution 7 : image & 0xfe      7 bits

void ReduceResolution(IplImage* ImageOI,unsigned long Resolution)
{
    if (ImageOI == 0) return;
    if (ImageOI->depth != IPL_DEPTH_8U) return;     // Invalid image-format
    if (ImageOI->nChannels != 1) return;			// Invalid image-format
    if ((Resolution < 1) || (Resolution > 7)) return;
    CvSize FrameSize = cvGetSize(ImageOI);

    unsigned char ResolutionMask = 0xff;
    switch (Resolution)
    {
    case 1 : ResolutionMask= 0x80;
        break;

    case 2 : ResolutionMask= 0xc0;
        break;

    case 3 : ResolutionMask= 0xe0;
        break;

    case 4 : ResolutionMask= 0xf0;
        break;

    case 5 : ResolutionMask= 0xf8;
        break;

    case 6 : ResolutionMask= 0xfc;
        break;

    case 7 : ResolutionMask= 0xfe;
        break;

    default : ResolutionMask= 0xff;
        break;
    }

    unsigned char* ImageData = (unsigned char*)ImageOI->imageData;
    for (int y=0;y < FrameSize.height;y++)
    {
        unsigned char* Data = ImageData;
        for (int x=0;x < FrameSize.width; x++)
        {
            unsigned char Value = *Data;
            *Data++ = Value & ResolutionMask;
        }

        ImageData += ImageOI->widthStep;
    }
}

IplImage* GetBinaryImage(const IplImage* ImageOI,unsigned char TrueValue)
{
    if (ImageOI == 0) return 0;
    if (ImageOI->depth != (int)IPL_DEPTH_8U) return 0;    // Invalid image-format
    if (ImageOI->nChannels != 1) return 0;			// Invalid image-format

    CvSize FrameSize = cvGetSize(ImageOI);

    IplImage* BinaryImage = cvCreateImage(FrameSize,IPL_DEPTH_8U,1);
    const unsigned char* RowSource = (unsigned char*)ImageOI->imageData;
    unsigned char* RowTarget = (unsigned char*)BinaryImage->imageData;

    for (int y=0;y < FrameSize.height;y++)
    {
        const unsigned char* Source = RowSource;
        unsigned char* Target = RowTarget;
        for (int x=0;x < FrameSize.width; x++)
        {
            *Target++ = (*Source++ == 0) ? 0 : TrueValue;
        }

        RowSource +=  ImageOI->widthStep;
        RowTarget +=  BinaryImage->widthStep;
    }

    return BinaryImage;
}


void AdjustBinaryTrueValues(IplImage* ImageOI,unsigned char TrueValue)
{
    if (ImageOI == 0) return;
    if (ImageOI->depth != (int)IPL_DEPTH_8U) return;    // Invalid image-format
    if (ImageOI->nChannels != 1) return;			// Invalid image-format

    CvSize FrameSize = cvGetSize(ImageOI);
    unsigned char* ImageData = (unsigned char*)ImageOI->imageData;

    for (int y=0;y < FrameSize.height;y++)
    {
        unsigned char* RowData = ImageData;
        for (int x=0;x < FrameSize.width; x++)
        {
            if (*RowData != 0) *RowData = TrueValue;
            RowData++;
        }

        ImageData +=  ImageOI->widthStep;
    }
}

void GetGreyLevelHistogram(const IplImage* Image,std::vector<unsigned long>& Histogram)
{
    Histogram.resize(256,0);
    if (Image == 0) return;			// No image provided
    if (Image->depth != IPL_DEPTH_8U) return;	// Invalid image-format

    CvSize FrameSize = cvGetSize(Image);

    const unsigned char* RowData = (unsigned char*)Image->imageData;
    for (int Row=0;Row < FrameSize.height;Row++)
    {
        const unsigned char* Data = RowData;
        for (int Column=0;Column < FrameSize.width; Column++) Histogram[*Data++]++;
        RowData += Image->widthStep;
    }	// end for-loop rows
}


bool AreImagesEqual(const IplImage* Image1,const IplImage* Image2)
{
    if (!CheckImageFootPrints(Image1,Image2)) return false;
    if (Image1->depth != (int)IPL_DEPTH_8U) return false;       // Invalid image-format
    if (Image1->nChannels != 1) return false;                   // Invalid image-format

    CvSize FrameSize = cvGetSize(Image1);
    const unsigned char* ImageData1 = (unsigned char*)Image1->imageData;
    const unsigned char* ImageData2 = (unsigned char*)Image2->imageData;

    for (int y=0;y < FrameSize.height;y++)
    {
        const unsigned char* RowData1 = ImageData1;
        const unsigned char* RowData2 = ImageData2;
        for (int x=0;x < FrameSize.width; x++)
        {
            if (*RowData1++ != *RowData2++) return false;
        }

        ImageData1 +=  Image1->widthStep;
        ImageData2 +=  Image2->widthStep;
    }
    return true;
}


IplImage* GetBikeCornersDifferenceImage(const IplImage* OldImage,const IplImage* NewImage,unsigned char ValueOI)
{
    if (!CheckImageFootPrints(OldImage,NewImage)) return 0;
    if (OldImage->depth != (int)IPL_DEPTH_8U) return 0;       // Invalid image-format
    if (OldImage->nChannels != 1) return 0;                   // Invalid image-format

    CvSize FrameSize = cvGetSize(OldImage);
    IplImage* ResultImage = cvCreateImage(FrameSize,IPL_DEPTH_8U,1);
    if (ResultImage == 0) return 0;

    const unsigned char* ImageData1 = (unsigned char*)OldImage->imageData;
    const unsigned char* ImageData2 = (unsigned char*)NewImage->imageData;
    unsigned char* ImageResultData = (unsigned char*)ResultImage->imageData;

    for (int y=0;y < FrameSize.height;y++)
    {
        const unsigned char* RowData1 = ImageData1;
        const unsigned char* RowData2 = ImageData2;
        unsigned char* RowResult = ImageResultData;

        for (int x=0;x < FrameSize.width; x++)
        {
            if ((*RowData2 == ValueOI)
                    && (*RowData1 != ValueOI)) *RowResult++ = ValueOI;
            else *RowResult++ = 0;

            RowData1++;
            RowData2++;

        }

        ImageData1 += OldImage->widthStep;
        ImageData2 += NewImage->widthStep;
        ImageResultData += ResultImage->widthStep;
    }
    return ResultImage;
}

void SetImageBorders(IplImage* ImageOI,unsigned long BorderWidth,unsigned long BorderHeight,unsigned char Value)
{
    if (ImageOI == 0) return;
    if (ImageOI->depth != IPL_DEPTH_8U) return;	// Invalid image-format
    if (ImageOI->width == 0) return;	// Invalid image
    if (ImageOI->height == 0) return;	// Invalid image
    CvSize FrameSize = cvGetSize(ImageOI);

    // Set Top/BottomBorder (if BorderHeight > 0)

    if (BorderHeight > 0)
    {
        // Set top and bottom-border

        unsigned long Height = std::min(BorderHeight,(unsigned long)FrameSize.height/2);
        unsigned char* TopRow = (unsigned char*)ImageOI->imageData;
        unsigned char* BottomRow = (unsigned char*)ImageOI->imageData+(ImageOI->widthStep*(FrameSize.height-1));

        for (int y=0;y < (int)Height;y++)
        {
            unsigned char* Data1 = TopRow;
            unsigned char* Data2 = BottomRow;

            for (int x=0;x < FrameSize.width; x++)
            {
                *Data1++ = Value;
                *Data2++ = Value;
            }

            TopRow += ImageOI->widthStep;
            BottomRow -= ImageOI->widthStep;
        }
    }

    // Set Left/RightBorder (if BorderWidth > 0)

    if (BorderWidth > 0)
    {
        // Set top and bottom-border

        unsigned long Width = std::min(BorderWidth,(unsigned long)FrameSize.width/2);
        unsigned char* LeftColumn = (unsigned char*)ImageOI->imageData;
        unsigned char* RightColumn = (unsigned char*)ImageOI->imageData+FrameSize.width-Width;
        for (int y=0;y < FrameSize.height;y++)
        {
            unsigned char* Data1 = LeftColumn;
            unsigned char* Data2 = RightColumn;

            for (int x=0;x < (int) Width; x++)
            {
                *Data1++ = Value;
                *Data2++ = Value;
            }

            LeftColumn += ImageOI->widthStep;
            RightColumn += ImageOI->widthStep;
        }
    }
}

// Sobel Operator
// [-1 0 1]
// [-2 0 2]
// [-1 0 1]


IplImage* GetSobelVerticalEdges(const IplImage* Image)
{
    if (Image->depth != (int)IPL_DEPTH_8U) return 0;       // Invalid image-format
    if (Image->nChannels != 1) return 0;                   // Invalid image-format

    CvSize FrameSize = cvGetSize(Image);
    IplImage* ResultImage = cvCreateImage(FrameSize,IPL_DEPTH_8U,1);
    if (ResultImage == 0) return 0;
    SetImageBorders(ResultImage,1,1,0);

    const unsigned char* ImageData = (unsigned char*)Image->imageData;
    unsigned char* ImageResultData = (unsigned char*)ResultImage->imageData;


    for (int y=2;y < FrameSize.height;y++)
    {
        const unsigned char* Row0Data = ImageData;
        const unsigned char* Row1Data = Row0Data + Image->widthStep;

        const unsigned char* Row2Data = Row1Data + Image->widthStep;
        unsigned char* RowResult = ImageResultData+1;

        int x00 = *Row0Data++;
        int x01 = *Row0Data++;
        int x10 = *Row1Data++;
        int x11 = *Row1Data++;
        int x20 = *Row2Data++;
        int x21 = *Row2Data++;

        for (int x=2;x < FrameSize.width; x++)
        {

            int x02 = *Row0Data++;
            int x12 = *Row1Data++;
            int x22 = *Row2Data++;

            int Result = x02 - x00 - 2*x10 + 2*x12 - x20 + x22;
            if (Result < 0) Result = -Result;
            if (Result > 255) Result = 255;

            *RowResult++ = Result;

            x00 = x01;
            x01 = x02;
            x10 = x11;
            x11 = x12;
            x20 = x21;
            x21 = x22;
        }

        ImageData += Image->widthStep;
        ImageResultData += ResultImage->widthStep;
    }

    return ResultImage;
}

//The Scharr aperture is
// {-3}{0}{3}
// {-10}{0}{10}
// {-3}{0}{3}


IplImage* GetScharrVerticalEdges(const IplImage* Image)
{
    if (Image->depth != (int)IPL_DEPTH_8U) return 0;       // Invalid image-format
    if (Image->nChannels != 1) return 0;                   // Invalid image-format

    CvSize FrameSize = cvGetSize(Image);
    IplImage* ResultImage = cvCreateImage(FrameSize,IPL_DEPTH_8U,1);
    if (ResultImage == 0) return 0;
    SetImageBorders(ResultImage,1,1,0);

    const unsigned char* ImageData = (unsigned char*)Image->imageData;
    unsigned char* ImageResultData = (unsigned char*)ResultImage->imageData;


    for (int y=2;y < FrameSize.height;y++)
    {
        const unsigned char* Row0Data = ImageData;
        const unsigned char* Row1Data = Row0Data + Image->widthStep;
        const unsigned char* Row2Data = Row1Data + Image->widthStep;
        unsigned char* RowResult = ImageResultData+1;

        int x00 = *Row0Data++;
        int x01 = *Row0Data++;
        int x10 = *Row1Data++;
        int x11 = *Row1Data++;
        int x20 = *Row2Data++;
        int x21 = *Row2Data++;

        for (int x=2;x < FrameSize.width; x++)
        {
            int x02 = *Row0Data++;
            int x12 = *Row1Data++;
            int x22 = *Row2Data++;

            int Result = 3*x02 - 3*x00 - 10*x10 + 10*x12 - 3* x20 + 3 * x22;
            if (Result < 0) Result = -Result;

            if (Result > 255) Result = 255;

            *RowResult++ = Result;
            x00 = x01;
            x01 = x02;
            x10 = x11;
            x11 = x12;
            x20 = x21;
            x21 = x22;
        }

        ImageData += Image->widthStep;
        ImageResultData += ResultImage->widthStep;
    }

    return ResultImage;
}


void GetAndImage(const IplImage* Image1,IplImage* Image2)
{
    if (!CheckImageFootPrints(Image1,Image2)) return;
    if (Image1->depth != (int)IPL_DEPTH_8U) return;       // Invalid image-format
    if (Image1->nChannels != 1) return;                   // Invalid image-format

    CvSize FrameSize = cvGetSize(Image1);

    const unsigned char* ImageData1 = (unsigned char*)Image1->imageData;
    unsigned char* ImageData2 = (unsigned char*)Image2->imageData;

    for (int y=0;y < FrameSize.height;y++)
    {
        const unsigned char* RowData1 = ImageData1;
        unsigned char* RowData2 = ImageData2;

        for (int x=0;x < FrameSize.width; x++)
        {
            *RowData2++ &= *RowData1++;
        }

        ImageData1 += Image1->widthStep;
        ImageData2 += Image2->widthStep;

    }
}


IplImage* GetTrueImagePixels(const IplImage* ImageOI,const IplImage* BinaryImage,unsigned char FalseValue)
{
    if (!CheckImageFootPrints(ImageOI,BinaryImage)) return 0;
    if (ImageOI->depth != (int)IPL_DEPTH_8U) return 0;       // Invalid image-format
    if (ImageOI->nChannels != 1) return 0;                   // Invalid image-format

    CvSize FrameSize = cvGetSize(ImageOI);
    IplImage* ResultImage = cvCreateImage(FrameSize,IPL_DEPTH_8U,1);
    if (ResultImage == 0) return 0;

    const unsigned char* ImageData = (unsigned char*)ImageOI->imageData;
    const unsigned char* BinaryData = (unsigned char*)BinaryImage->imageData;
    unsigned char* ResultData = (unsigned char*)ResultImage->imageData;

    for (int y=0;y < FrameSize.height;y++)
    {
        const unsigned char* RowData = ImageData;
        const unsigned char* RowBinary = BinaryData;
        unsigned char* RowResult = ResultData;

        for (int x=0;x < FrameSize.width; x++)
        {
            if (*RowBinary++ != 0) *RowResult++ = *RowData++;
            else
            {
                *RowResult++ = FalseValue;
                RowData++;
            }
        }

        ImageData += ImageOI->widthStep;
        BinaryData += BinaryImage->widthStep;
        ResultData += ResultImage->widthStep;
    }
    return ResultImage;
}



void GetTrueImagePixels(const IplImage* ImageOI,const IplImage* BinaryImage,unsigned char FalseValue,IplImage** ResultImage)
{
    if (!CheckImageFootPrints(ImageOI,BinaryImage)) return;
    if (ImageOI->depth != (int)IPL_DEPTH_8U) return;       // Invalid image-format
    if (ImageOI->nChannels != 1) return;                   // Invalid image-format
    if (ResultImage == 0) return;
    CvSize FrameSize = cvGetSize(ImageOI);

    if (*ResultImage == 0) *ResultImage = cvCreateImage(FrameSize,IPL_DEPTH_8U,1);
    else
    {
        if (!CheckImageFootPrints(ImageOI,*ResultImage)
                || ((*ResultImage)->depth != (int)IPL_DEPTH_8U))
        {
            cvReleaseImage(ResultImage);
            *ResultImage = cvCreateImage(FrameSize,IPL_DEPTH_8U,1);
        }
    }

    IplImage* DestinationImage = *ResultImage;


    const unsigned char* ImageData = (unsigned char*)ImageOI->imageData;
    const unsigned char* BinaryData = (unsigned char*)BinaryImage->imageData;
    unsigned char* DestinationData = (unsigned char*)DestinationImage->imageData;

    for (int y=0;y < FrameSize.height;y++)
    {
        const unsigned char* RowData = ImageData;
        const unsigned char* RowBinary = BinaryData;
        unsigned char* RowDestination = DestinationData;

        for (int x=0;x < FrameSize.width; x++)
        {
            if (*RowBinary++ != 0) *RowDestination++ = *RowData++;
            else
            {
                *RowDestination++ = FalseValue;
                RowData++;
            }
        }

        ImageData += ImageOI->widthStep;
        BinaryData += BinaryImage->widthStep;
        DestinationData += DestinationImage->widthStep;
    }
}


IplImage* CreateImageForViewing16UImage(const IplImage* ImageOI)
{
    if (ImageOI->depth != (int)IPL_DEPTH_16U) return 0;      // Invalid image-format
    if (ImageOI->nChannels != 1) return 0;                   // Invalid image-format

    CvSize FrameSize = cvGetSize(ImageOI);
    IplImage* ResultImage = cvCreateImage(FrameSize,IPL_DEPTH_8U,1);
    if (ResultImage == 0) return 0;

    const unsigned short* ImageData = (unsigned short*)ImageOI->imageData;
    unsigned char* ResultData = (unsigned char*)ResultImage->imageData;

    for (int y=0;y < FrameSize.height;y++)
    {
        const unsigned short* RowData = ImageData;
        unsigned char* RowResult = ResultData;

        for (int x=0;x < FrameSize.width; x++)
        {
            unsigned short Value = *RowData++;
            if (Value > 255) Value = 255;
            *RowResult++ = Value;
        }

        ImageData += ImageOI->widthStep/sizeof(unsigned short);
        ResultData += ResultImage->widthStep;
    }

    return ResultImage;
}

void SetImage(const vipnt::OfflineFrame* SourceImage, IplImage* DestinationImage)
{
    if (SourceImage == 0) return;
    if (DestinationImage == 0) return;

    // Set frame

    unsigned int Width = SourceImage->GetWidth();
    unsigned int Height = SourceImage->GetHeight();
    if (Width != (unsigned int)DestinationImage->width) return;
    if (Height != (unsigned int)DestinationImage->height) return;
    unsigned int WidthStep = (unsigned int)DestinationImage->widthStep;

    const unsigned char* SourceData = SourceImage->GetData();
    unsigned char* DestinationData = (unsigned char*)DestinationImage->imageData;

    for (unsigned int y=0;y < Height;y++)
    {
        unsigned char* RowDestination = DestinationData;

        for (unsigned int x=0; x < Width; x++) *RowDestination++ = *SourceData++;
        DestinationData += WidthStep;
    }
}


void SetImage(const bool* SourceImage, IplImage* DestinationImage)
{
    if (SourceImage == 0) return;
    if (DestinationImage == 0) return;

    // Set frame

    unsigned int Width = DestinationImage->width;
    unsigned int Height = DestinationImage->height;
    unsigned int WidthStep = (unsigned int)DestinationImage->widthStep;

    const bool* SourceData = SourceImage;
    unsigned char* DestinationData = (unsigned char*)DestinationImage->imageData;

    for (unsigned int y=0;y < Height;y++)
    {
        unsigned char* RowDestination = DestinationData;

        for (unsigned int x=0; x < Width; x++) *RowDestination++ = (*SourceData++) ? 0xff : 0x00;
        DestinationData += WidthStep;
    }
}


void SetImage(const unsigned char* SourceImage, IplImage* DestinationImage)
{
    if (SourceImage == 0) return;
    if (DestinationImage == 0) return;

    // Set frame

    unsigned int Width = DestinationImage->width;
    unsigned int Height = DestinationImage->height;
    unsigned int WidthStep = (unsigned int)DestinationImage->widthStep;

    const unsigned char* SourceData = SourceImage;
    unsigned char* DestinationData = (unsigned char*)DestinationImage->imageData;

    for (unsigned int y=0;y < Height;y++)
    {
        unsigned char* RowDestination = DestinationData;

        for (unsigned int x=0; x < Width; x++) *RowDestination++ = *SourceData++;
        DestinationData += WidthStep;
    }
}

void SetImage(const int* SourceImage, IplImage* DestinationImage)
{
    if (SourceImage == 0) return;
    if (DestinationImage == 0) return;

    // Set frame

    unsigned int Width = DestinationImage->width;
    unsigned int Height = DestinationImage->height;
    unsigned int WidthStep = (unsigned int)DestinationImage->widthStep;

    const int* SourceData = SourceImage;
    unsigned char* DestinationData = (unsigned char*)DestinationImage->imageData;

    for (unsigned int y=0; y < Height; y++)
    {
        unsigned char* RowDestination = DestinationData;

        for (unsigned int x=0; x < Width; x++) *RowDestination++ = *SourceData++;
        DestinationData += WidthStep;
    }
}

void SetImageRescaled(const vipnt::OfflineFrame* SourceImage, IplImage* DestinationImage, int MaximumValue)
{
    if (SourceImage == 0) return;
    if (DestinationImage == 0) return;

    // MaximumValue == 0

    if (MaximumValue == 0)
    {
        // Set all to zero (avoid a zero-divide @ rescaling

        cvZero(DestinationImage);
        return;
    }

    // Set frame

    unsigned int Width = SourceImage->GetWidth();
    unsigned int Height = SourceImage->GetHeight();
    if (Width != (unsigned int)DestinationImage->width) return;
    if (Height != (unsigned int)DestinationImage->height) return;
    unsigned int WidthStep = (unsigned int)DestinationImage->widthStep;

    const unsigned char* SourceData = SourceImage->GetData();
    unsigned char* DestinationData = (unsigned char*)DestinationImage->imageData;

    for (unsigned int y=0; y < Height; y++)
    {
        unsigned char* RowDestination = DestinationData;

        for (unsigned int x=0; x < Width; x++)
        {
            unsigned char Value = *SourceData++;
            if (Value > MaximumValue) Value = MaximumValue;
            Value = (Value*255)/MaximumValue;
            *RowDestination++ = Value;
        }

        DestinationData += WidthStep;
    }
}

IplImage* Get33CensusTransFormedImage(const IplImage* Image)
{
    if (Image == 0) return 0;
    if (Image->depth != IPL_DEPTH_8U) return 0;	// Invalid image-format
    if (Image->width == 0) return 0;	// Invalid image
    if (Image->height == 0) return 0;	// Invalid image

    // Create image

    CvSize FrameSize = cvGetSize(Image);
    IplImage* ImageOI = cvCreateImage(FrameSize,IPL_DEPTH_8U ,1);

    // Borderpixels aren't defined

    SetImageBorders(ImageOI,1,1,0x00);

    // Census-tranform

    const unsigned int SourceWidthStep = Image->widthStep;
    unsigned char* RowResult = (unsigned char*)ImageOI->imageData+ImageOI->widthStep+1;
    const unsigned char* RowData = (unsigned char*)Image->imageData+SourceWidthStep+1;

    for (int y=1;y < (FrameSize.height-1);y++)
    {
        const unsigned char* Data1 = RowData;
        unsigned char* DataResult = RowResult;

        for (int x=1;x < (Image->width-1); x++)
        {
            unsigned char Reference = *Data1;
            unsigned char Result = 0;

            // QuickComparation

            if (*(Data1-1-SourceWidthStep) >= Reference) Result |= 0x80;
            if (*(Data1-SourceWidthStep) >= Reference) Result |= 0x40;
            if (*(Data1+1-SourceWidthStep) >= Reference) Result |= 0x20;
            if (*(Data1-1) >= Reference) Result |= 0x10;
            if (*(Data1+1) >= Reference) Result |= 0x08;
            if (*(Data1-1+SourceWidthStep) >= Reference) Result |= 0x04;
            if (*(Data1+SourceWidthStep) >= Reference) Result |= 0x02;
            if (*(Data1+1+SourceWidthStep) >= Reference) Result |= 0x01;

            *DataResult++ = Result;
            Data1++;
        }

        RowData += SourceWidthStep;
        RowResult += ImageOI->widthStep;
    }

    return ImageOI;
}

// Calculate absolute gradients
//    max(| (i,j-1) - (i,j) | , | (i-1,j) - (i,j) |)

IplImage* GetBdcGradientImage(const IplImage* Image)
{
    if (Image == 0) return 0;
    if (Image->depth != IPL_DEPTH_8U) return 0;	// Invalid image-format
    if (Image->width == 0) return 0;	// Invalid image
    if (Image->height == 0) return 0;	// Invalid image

    // Create image

    CvSize FrameSize = cvGetSize(Image);
    IplImage* ImageOI = cvCreateImage(FrameSize,IPL_DEPTH_8U ,1);

    // Borderpixels aren't defined

    SetImageBorders(ImageOI,1,1,0x00);

    const unsigned int SourceWidthStep = Image->widthStep;
    unsigned char* RowResult = (unsigned char*)ImageOI->imageData+ImageOI->widthStep+1;
    const unsigned char* RowData = (unsigned char*)Image->imageData+SourceWidthStep+1;

    for (int y=1;y < (FrameSize.height-1);y++)
    {
        const unsigned char* Data1 = RowData;
        unsigned char* DataResult = RowResult;

        for (int x=1;x < (Image->width-1); x++)
        {
            unsigned char Reference = *Data1;
            unsigned char Value = *(Data1-SourceWidthStep);
            unsigned char Result1 = (Value > Reference) ? Value - Reference : Reference - Value;
            Value = *(Data1-1);
            unsigned char Result2 = (Value > Reference) ? Value - Reference : Reference - Value;
            *DataResult++ = (Result1 > Result2)  ? Result1 : Result2;
            Data1++;
        }

        RowData += SourceWidthStep;
        RowResult += ImageOI->widthStep;
    }

    return ImageOI;
}
