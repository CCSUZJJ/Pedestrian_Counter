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
  /*  if(m_Configured)
    {
        IplImage* Frame = 0;
        while(true)
        {
            cv::Mat CurrFrame = m_VideoPlayer.GetNextFrame();
            if(CurrFrame.data == 0)
            {
                m_VideoPlayer.Reset();
                break;		// no frames
            }
            OpenCvFrame UpdateFrame(CurrFrame);

            unsigned long FieldCounter = GetTraficonFieldCounter(Frame);
            for (unsigned int Idx = 0; Idx < m_Algo.size(); ++Idx)
            {
                m_Algo[Idx]->ReadSample(UpdateFrame);
                m_Algo[Idx]->SetFieldCounter(FieldCounter);
            }

            //----------------------------------------------
            // Run

//            timeval start, end;
//            long mtime, seconds, useconds;

//            gettimeofday(&start, NULL);

            bool HasWork = true;
            while (HasWork)
            {
                HasWork = false;
                for (unsigned int Idx = 0; Idx < m_Algo.size(); ++Idx)
                {
                    m_Algo[Idx]->Run();
                    HasWork = HasWork || m_Algo[Idx]->HasWork();
                }
            }

//            gettimeofday(&end, NULL);

//            seconds = end.tv_sec - start.tv_sec;
//            useconds = end.tv_usec - start.tv_usec;

//            mtime = ((seconds)*1000 + useconds/1000.0) + 0.5;

//            std::cout << "Elapsed time: " << mtime << " ms." << std::endl;
        }

        // Now perform the Matching to receive Performance results
        for(auto& Algo : m_Algo)
        {
            Algo->ProcessPerformanceAnalyse();
        }

        RetVal = true;
    }
    return RetVal; */

    //TESTING
    int Point1X;
    int Point1Y;
    int Point2X;
    int Point2Y;
    ticpp::Document XmlDoc(m_ConfigFileName.c_str());
    try{
        XmlDoc.LoadFile();
    }
    catch(ticpp::Exception& ex){
        std::cout << ex.what();
    }

    try{
        ticpp::Element* Point1 = XmlDoc.FirstChild("Configuration")->FirstChildElement("IntrusionDetection")
                ->FirstChildElement("IntrusionDetection")->FirstChildElement("Zones")->FirstChildElement("Zone")
                ->FirstChildElement("Shape")->FirstChildElement("Point");
        ticpp::Element* Point2 = Point1->NextSiblingElement("Point");
        Point1->GetAttribute("X", &Point1X);
        Point1->GetAttribute("Y", &Point1Y);
        Point2->GetAttribute("X", &Point2X);
        Point2->GetAttribute("Y", &Point2Y);
    }
    catch(ticpp::Exception& ex){
        std::cout << ex.what();
    }

    if(m_Configured){
        int frameNo = 0;
        ticpp::Document Dom(m_Config.s_GroundTruthFile.c_str());
        Dom.LoadFile();
        ticpp::Element* FrameNodes = Dom.FirstChild("GroundTruth")->FirstChildElement("Frames");
        ticpp::Element* CurrFrameNode = FrameNodes->FirstChildElement("Frame");

        int totalFrames = 0;
        ticpp::Iterator< ticpp::Element > Frame ("Frame");
        for(Frame = Frame.begin(FrameNodes); Frame != Frame.end(); Frame++){
            totalFrames++;
        }

        while(frameNo < totalFrames-1) //TotalFrames -1 omdat 1ste FrameNr = 0
        {
            cv::namedWindow("Video");
            cv::Mat CurrFrame = m_VideoPlayer.GetNextFrame();
            if(CurrFrame.data == 0)
            {
                m_VideoPlayer.Reset();
                break;		// no frames
            }

            cv::Point Point1 = cv::Point(Point1X, Point1Y);
            cv::Point Point2 = cv::Point(Point2X, Point2Y);
            cv::line(CurrFrame,Point1, Point2, cv::Scalar(0,0,255), 2, 8);

            ticpp::Iterator< ticpp::Element > CurrBox ( "BBox" );
            for(CurrBox = CurrBox.begin(CurrFrameNode); CurrBox != CurrBox.end(); CurrBox++){
                int x, y, w, h;
                CurrBox->GetAttribute("X", &x);
                CurrBox->GetAttribute("Y", &y);
                CurrBox->GetAttribute("W", &w);
                CurrBox->GetAttribute("H", &h);

                cv::Rect rec = cv::Rect(x, y, w, h);
                cv::rectangle(CurrFrame,rec,cv::Scalar(0,255,0),2,8);
            }

            cv::imshow("Video", CurrFrame);
            cv::waitKey(30);
            OpenCvFrame UpdateFrame(CurrFrame);

            frameNo++;
            CurrFrameNode = CurrFrameNode->NextSiblingElement("Frame");
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



