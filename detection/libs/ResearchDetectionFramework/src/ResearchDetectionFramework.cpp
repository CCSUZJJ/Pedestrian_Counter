#include "ResearchDetectionFramework.h"
#include <iostream>
#include "tinyxml.h"     //Te verwijderen na vervangen tinyxml -> ticpp
                         //Probleem doordat LoadFile geen bool returnt bij ticpp zie r149
#include "ticpp.h"
#include "OpenCvUtilities.h"
#include "FrameDecoding.h"

#include "CvBoundingBox.h"  //gewoon om te testen
#include "CvPoint.h"
#include "CvLine.h"
#include <vector>
#include <math.h>

#include "DetectedObject.h"
#include "ForegroundSubtraction.h"
#include "BlobSegmentation.h"

namespace
{
    void GetNameAndPath(const string& Input, string& Path, string& VideoName)
    {
        size_t DotIndex     = Input.find_last_of(".");
        size_t SlashIndex   = Input.find_last_of("/\\");

        if ((DotIndex > 0) && (SlashIndex > 0))
        {
            Path        = Input.substr(0,SlashIndex+1);
            VideoName   = Input.substr(SlashIndex+1,DotIndex-SlashIndex);
        }
    }
}

ResearchDetectionFramework::ResearchDetectionFramework(const SFrameWorkConfig& Config):
   m_Configured(false)
,  m_VideoPlayer()
,  m_Algo()
,  m_ConfigFileName()
,  m_Config(Config)
{
    FILE * OutputFile;
    OutputFile = fopen ("SumOfErrors","w");
    fclose (OutputFile);
}

ResearchDetectionFramework::ResearchDetectionFramework() :
   m_Configured(false)
,  m_VideoPlayer()
,  m_Algo()
,  m_ConfigFileName()
,  m_Config()
{
    FILE * OutputFile;
    OutputFile = fopen ("SumOfErrors","w");
    fclose (OutputFile);
}

ResearchDetectionFramework::~ResearchDetectionFramework()
{
    ClearAlgoList();
}

void ResearchDetectionFramework::SetVideoName(const string& VideoFileName, unsigned int FrameRate, CvSize FrameSize)
{
    string Path = "";
    string VideoName = "";
    size_t DotIndex     = VideoFileName.find_last_of(".");
    size_t SlashIndex   = VideoFileName.find_last_of("/\\");
    if ((DotIndex > 0) && (SlashIndex > 0))
    {
        Path        = VideoFileName.substr(0,SlashIndex+1);
        VideoName   = VideoFileName.substr(SlashIndex+1,DotIndex-SlashIndex);
    }

    for(unsigned int Idx = 0; Idx < m_Algo.size(); ++Idx)
    {
        m_Algo[Idx]->SetVideoName(Path, VideoName, FrameRate, FrameSize, m_Config.s_CreateVideo);
    }
}

void ResearchDetectionFramework::ClearAlgoList()
{
    for(unsigned int Idx = 0; Idx < m_Algo.size(); ++Idx)
    {
        if(m_Algo[Idx])
        {
            delete m_Algo[Idx];
            m_Algo[Idx] = 0;
        }
    }
    m_Algo.clear();
}

void ResearchDetectionFramework::Configure(const std::string& FileNameVideo, const std::string& FileNameConfiguration,
                                           const std::string& GroundTruthFileName, const std::string& GroupName)
{
    std::cout << "Open Video: "  << FileNameVideo << std::endl;
    m_VideoPlayer.SetName(FileNameVideo);
    m_Configured = m_VideoPlayer.Init();
    if(!m_Configured)
    {
        FILE * OutputFile;
        OutputFile = fopen ("SumOfErrors","aw");
        if(OutputFile)
        {
            fprintf (OutputFile, "Error opening video: %-60.60s \n", FileNameVideo.c_str());
            fclose (OutputFile);
        }
        else
        {
            std::cout << " SHITTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT" << std::endl;
            perror ("The following error occurred");

        }
    }
    SetVideoName(FileNameVideo, m_VideoPlayer.GetFrameRate(), m_VideoPlayer.GetFrameSize());

    m_ConfigFileName = FileNameConfiguration;
    std::cout << "Send Config: "  << m_ConfigFileName << std::endl;
    bool PrevConfig = m_Configured;
    m_Configured = m_Configured && SendConfig(GroundTruthFileName, GroupName);

    if(PrevConfig && !m_Configured)
    {
        FILE * OutputFile;
        OutputFile = fopen ("SumOfErrors","aw");
        if(OutputFile)
        {
            fprintf (OutputFile, "Error sending config: %-60.60s \n", m_ConfigFileName.c_str());
            fclose (OutputFile);
        }
        else
        {
            std::cout << " SHITTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT" << std::endl;
            perror ("The following error occurred");

        }
    }

}

void ResearchDetectionFramework::AddAlgo(IDetAlgo* Algo)
{
    if(Algo) // ensure to add only valid pointers
    {
        m_Algo.push_back(Algo);
    }
}

bool ResearchDetectionFramework::SendConfig(const std::string& GroundTruthFileName, const std::string& GroupName)
{
    bool RetVal = false;
    std::cout << "Parse Config file: " << m_ConfigFileName << std::endl;
    TiXmlDocument XmlDoc;
    //ticpp::Document XmlDoc;
    m_Config.s_XmlDoc = &XmlDoc;
    m_Config.s_GroundTruthFile = GroundTruthFileName;
    m_Config.s_GroupName = GroupName;
    if (!m_ConfigFileName.empty() && XmlDoc.LoadFile(m_ConfigFileName.c_str())) //m.b.v. ticpp returnt LoadFile void ipv bool
    {
        RetVal = true;
        for (unsigned int Idx = 0; RetVal && Idx < m_Algo.size(); ++Idx)
        {

            RetVal = m_Algo[Idx]->Configure(m_Config); //! \todo check non zero Algo pointers!!!
        }
    }
    else
    {
        FILE * OutputFile;
        OutputFile = fopen ("SumOfErrors","aw");
        if(OutputFile)
        {
            fprintf (OutputFile, "ERROR: File: %-60.60s \n", m_ConfigFileName.c_str());
            fclose (OutputFile);
        }
        else
        {
            perror ("The following error occurred");
            std::cout << " SHITTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT" << std::endl;
        }


        std::cout << "ERROR: File " << m_ConfigFileName << " does not exist or is no proper xml" << std::endl;
    }

    return RetVal;
}

bool ResearchDetectionFramework::Run()
{

    bool RetVal = false;


    if(m_Configured){
        //Init runningAvgBackground
        //cv::Mat background = m_VideoPlayer.GetNextFrame();
        //cv::cvtColor(background, background, CV_BGR2GRAY);
        //m_VideoPlayer.Init();                               //Videoplayer back at frame 0
        //-------------------------------------------------------------------------------------------
        //Init Sigma Delta
        //cv::Mat background = cv::Mat(m_VideoPlayer.GetFrameSize(), CV_8UC1, cv::Scalar(128));
        //-------------------------------------------------------------------------------------------
        //Init Sigma Delta with Confidence Measure
        cv::Mat background = m_VideoPlayer.GetNextFrame();
        cv::cvtColor(background, background, CV_BGR2GRAY);
        m_VideoPlayer.Init();                               //Videoplayer back at frame 0
        bool toUpdate = false;
        cv::Mat variance = cv::Mat(m_VideoPlayer.GetFrameSize(), CV_8UC1, cv::Scalar(10));
        cv::Mat detectionCount = cv::Mat::zeros(m_VideoPlayer.GetFrameSize(), CV_8UC1);
        cv::Mat frameCount = cv::Mat::zeros(m_VideoPlayer.GetFrameSize(), CV_8UC1);
        cv::Mat confidence = cv::Mat(m_VideoPlayer.GetFrameSize(), CV_8UC1, cv::Scalar(10));
        //-------------------------------------------------------------------------------------------

        ForegroundSegmentation fgSegment = ForegroundSegmentation();
        BlobSegmentation blobSegment =  BlobSegmentation();

        while(true){
            cv::Mat newFrame = m_VideoPlayer.GetNextFrame();
            if(newFrame.data == 0){
                m_VideoPlayer.Reset();
                break;
            }
            cv::Mat currFrame;
            cv::cvtColor(newFrame, currFrame, CV_BGR2GRAY);
            cv::Mat foreground = cv::Mat(m_VideoPlayer.GetFrameSize(), CV_8UC1);

            //Foreground Segmentation
            //fgSegment.runningAvgBackground(currFrame, background, foreground);
            //fgSegment.sigmaDeltaBackground(currFrame, background, foreground);
            fgSegment.sigmaDeltaCMBackground(currFrame, background, foreground, frameCount, confidence, variance,
                                             detectionCount, toUpdate);

            //Morphological Operations: opening -> closing
            cv::Mat foregroundMorph;
            cv::Mat kernel = cv::Mat::ones(cv::Size(4,4), CV_8UC1);
            cv::morphologyEx(foreground, foregroundMorph, cv::MORPH_OPEN, kernel);
            cv::morphologyEx(foreground, foregroundMorph, cv::MORPH_CLOSE, kernel);

            //Blob Segmentation
            //std::vector<cv::Rect> boundRect = blobSegment.contourSegment(foregroundMorph);
            //std::vector<cv::Rect> boundRect = blobSegment.intensitySegment(foregroundMorph);
            std::vector<cv::Rect> boundRect = blobSegment.connectedComponentSegment(foregroundMorph);

            //Draw the Bounding Boxes
            for(int i = 0; i < boundRect.size(); i++){
                cv::Rect rect = boundRect[i];
                if(rect.area() > 300){
                    cv::rectangle(newFrame, rect, cv::Scalar(0,255,0), 1,8,0);
                }
            }

            //Display
            cv::namedWindow("Current Frame");
            //cv::namedWindow("Background");
            //cv::namedWindow("Foreground");
            //cv::namedWindow("Foreground Morphed");
            cv::imshow("Current Frame", newFrame);
            //cv::imshow("Background", background);
            //cv::imshow("Foreground", foreground);
            //cv::imshow("Foreground Morphed", foregroundMorph);
            cv::waitKey(1);
        }

    }
    return true;
}

void ResearchDetectionFramework::FinishPerformanceAnalyse()
{
    for(auto& Algo : m_Algo)
    {
        Algo->FinishPerformanceAnalyse();
    }
}

void ResearchDetectionFramework::FinishGroupPerformanceAnalyse()
{
    for(auto& Algo : m_Algo)
    {
        Algo->FinishGroupPerformanceAnalyse();
    }
}

unsigned long ResearchDetectionFramework::GetTraficonFieldCounter(const IplImage* GrayImage)
{
    unsigned long FieldCounter = 0;
    const int ColumnsForTraficonDecoder = 16;
    if ((GrayImage == 0) ||
        (GrayImage->height < ColumnsForTraficonDecoder))
    {
        return FieldCounter;
    }

    // create a sub-image (top of the image)
    CvRect ROI;
    ROI.x = 0;
    ROI.y = 0;
    ROI.width  = GrayImage->width;
    ROI.height = ColumnsForTraficonDecoder;

    IplImage* ImageOI = GetROI(GrayImage,ROI);
    if (ImageOI == 0) return FieldCounter;

   // FieldCounter = GetFrameCounter((unsigned char*)ImageOI->imageData,ROI.width, ROI.height);
    cvReleaseImage(&ImageOI);
    return FieldCounter;
}



