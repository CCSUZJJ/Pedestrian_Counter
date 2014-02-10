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
        //Init toUpdate
        bool toUpdate = false;
        //Init Background
        cv::Mat background;
        cv::cvtColor(m_VideoPlayer.GetNextFrame(),background, CV_BGR2GRAY);
        //Init Variance: Vini = Vmin       Vt = [10,200]
        cv::Mat variance = cv::Mat(background.size(), CV_8UC1, cv::Scalar(10));
        //Init Detection Counter & Frame Counter
        cv::Mat detectionCount = cv::Mat::zeros(background.size(), CV_8UC1);
        cv::Mat frameCount = cv::Mat::zeros(background.size(), CV_8UC1);
        //Init Confidence Measure: Cini = Cmin      Ct = [10,125]
        cv::Mat confidence = cv::Mat(background.size(), CV_8UC1, cv::Scalar(10));

        //Extra Inits
        //Init Foreground
        cv::Mat foreground(background.size(), CV_8UC1);
        //Init difference background model, currentframe
        cv::Mat difference(background.size(), CV_8UC1);

        while(true){
            cv::Mat currFrame = m_VideoPlayer.GetNextFrame();
            if(currFrame.data == 0){
                m_VideoPlayer.Reset();
                break;
            }
            cv::Mat currFrameGray;
            cv::cvtColor(currFrame, currFrameGray, CV_BGR2GRAY);
            //Increment Frame Counter
            frameCount += 1;

            //For each pixel
            for(int i = 0; i < frameCount.rows; i++){
                for(int j = 0; j < frameCount.cols; j++){
                    if(frameCount.at<uchar>(i,j) < confidence.at<uchar>(i,j)){  //Current confidence period not expired
                        if(frameCount.at<uchar>(i,j) % 10){         //Refresh period expires
                            if(variance.at<uchar>(i,j) <= 38){      //Low variance: Vt <= Vth: reliable info
                                if((detectionCount.at<uchar>(i,j) / frameCount.at<uchar>(i,j)) <= 0.8){ //No heavy traffic
                                    toUpdate = true;                //Refresh period updating mode
                                }
                            }
                        }
                    }
                    else{                                                       //Current confidence period expired
                        if(variance.at<uchar>(i,j) <= 38){          //Low variance
                            confidence.at<uchar>(i,j) +=            //Confidence updating as function of detection ratio
                                    round(11 * exp(-4 * (detectionCount.at<uchar>(i,j) / frameCount.at<uchar>(i,j))) -1);
                            if(confidence.at<uchar>(i,j) == 10){    //Confidence = Cmin
                                toUpdate = true;                    //Force updating
                            }
                        }
                        else{                                       //High variance: no reliable info
                            toUpdate = true;                        //Confidence period updating mode
                                                                    //to avoid background model deadlock
                        }
                        detectionCount = cv::Scalar(0);             //Reset Detection Counter
                        frameCount = cv::Scalar(0);                 //Reset Frame Counter
                    }
                }
            }

            if(toUpdate == true){                                   //Updating recommended
                //Update Background Model
                for(int i = 0; i < background.rows; i++){
                    for(int j = 0; j < background.cols; j++){
                        if(currFrameGray.at<uchar>(i,j) > background.at<uchar>(i,j))
                            background.at<uchar>(i,j)++;
                        else if(currFrameGray.at<uchar>(i,j) < background.at<uchar>(i,j))
                            background.at<uchar>(i,j)--;
                    }
                }
                cv::absdiff(currFrameGray, background, difference); //Compute difference
                //Update Variance
                for(int i = 0; i < variance.rows; i++){
                    for(int j = 0; j < variance.cols; j++){
                        if(variance.at<uchar>(i,j) > 10 + 4*difference.at<uchar>(i,j))
                            variance.at<uchar>(i,j)--;
                        if(variance.at<uchar>(i,j) < 10 + 4*difference.at<uchar>(i,j))
                            variance.at<uchar>(i,j)++;
                    }
                }
                //Compute Foreground
                for(int i = 0; i < difference.rows; i++){
                    for(int j = 0; j < difference.cols; j++){
                        if(difference.at<uchar>(i,j) > variance.at<uchar>(i,j))
                            foreground.at<uchar>(i,j) = 255;
                        else
                            foreground.at<uchar>(i,j) = 0;
                    }
                }
            }
            else{                                                   //Do not update, just detect
                cv::absdiff(currFrameGray, background, difference); //Compute difference
                //Compute Foreground
                for(int i = 0; i < difference.rows; i++){
                    for(int j = 0; j < difference.cols; j++){
                        if(difference.at<uchar>(i,j) > variance.at<uchar>(i,j))
                            foreground.at<uchar>(i,j) = 255;
                        else
                            foreground.at<uchar>(i,j) = 0;
                    }
                }
            }
            //Update Detection Counter
            for(int i = 0; i < detectionCount.rows; i++){
                for(int j = 0; j < detectionCount.cols; j++){
                    if(foreground.at<uchar>(i,j) == 255)
                        detectionCount.at<uchar>(i,j)++;
                }
            }

            //Show output
            cv::namedWindow("Original");                //Original
            cv::imshow("Original", currFrameGray);
            cv::namedWindow("Background");              //Background
            cv::imshow("Background", background);
            cv::namedWindow("Foreground");              //Foreground
            cv::imshow("Foreground", foreground);
            cv::namedWindow("Variance");                //Variance
            cv::imshow("Variance", variance);
            cv::waitKey(25);
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



