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

#include "ForegroundSubtraction.h"
#include "BlobSegmentation.h"
#include "Tracking.h"
#include "DetectedBlob.h"
//#include "DetectedObject.h"
#include "Track.h"
#include "Geometry.h"

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
        //Populate the lines vector used to count objects crossing a line
        std::vector< std::vector<int> > lines;
        ticpp::Document XmlDoc(m_ConfigFileName.c_str());
        try{
            XmlDoc.LoadFile();
        }
        catch(ticpp::Exception& ex){
            std::cout << ex.what();
        }

        try{
            ticpp::Element* parent = XmlDoc.FirstChild("Configuration")->FirstChildElement("IntrusionDetection")
                    ->FirstChildElement("IntrusionDetection")->FirstChildElement("Zones");
            ticpp::Iterator<ticpp::Node> iterator;
            for(iterator = iterator.begin(parent); iterator != iterator.end(); iterator++){
                ticpp::Element* point1 = iterator->FirstChildElement("Shape")->FirstChildElement("Point");
                ticpp::Element* point2 = point1->NextSiblingElement("Point");
                int point1x;
                int point1y;
                int point2x;
                int point2y;
                point1->GetAttribute("X", &point1x);
                point1->GetAttribute("Y", &point1y);
                point2->GetAttribute("X", &point2x);
                point2->GetAttribute("Y", &point2y);
                std::vector<int> line;
                line.push_back(point1x);
                line.push_back(point1y);
                line.push_back(point2x);
                line.push_back(point2y);
                lines.push_back(line);
            }
        }
        catch(ticpp::Exception& ex){
            std::cout << ex.what();
        }

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
        Tracking tracker = Tracking();

        std::vector<Track> finishedTracks;
        std::vector<Track> currentTracks;

        int posToNegCnt =0;
        int negToPosCnt =0;

        int frameNumber = -1;       //init at -1 => first framenumber = 0
        //int count = 0;            //to stop at certain frame
        while(true){
            //count++;
            cv::Mat newFrame = m_VideoPlayer.GetNextFrame();
            if(newFrame.data == 0){
                m_VideoPlayer.Reset();
                break;
            }
            frameNumber++;

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
            //Remove noise
            cv::Mat kernel = cv::Mat::ones(cv::Size(2,2), CV_8UC1);
            cv::morphologyEx(foreground, foregroundMorph, cv::MORPH_OPEN, kernel);
            //Connect objects
            cv::Mat kernel2 =  cv::Mat::ones(cv::Size(20,20), CV_8UC1);
            cv::morphologyEx(foregroundMorph, foregroundMorph, cv::MORPH_CLOSE, kernel2);


            //Blob Segmentation
            //std::vector<cv::Rect> boundRect = blobSegment.contourSegment(foregroundMorph);
            //std::vector<cv::Rect> boundRect = blobSegment.intensitySegment(foregroundMorph); //Unfinished
            std::vector<cv::Rect> boundRect = blobSegment.connectedComponentSegment(foregroundMorph);
            //Create objects from the bounding boxes used for tracking
            int nextLabel = 1;
            std::vector<DetectedBlob> detectedBlobs;
            std::vector<cv::Rect>::iterator rectIt;
            for(rectIt = boundRect.begin(); rectIt != boundRect.end(); rectIt++){
                if(rectIt->area() > 180){
                    DetectedBlob blob = DetectedBlob();
                    blob.frameNr = frameNumber;
                    blob.BBox = (*rectIt);
                    blob.label = nextLabel;
                    nextLabel++;
                    detectedBlobs.push_back(blob);
                }
            }

            //Tracking
            //tracker.bestMatchTracking(presentObjects, detectedBlobs); //Check for best match in prev.
                                                    //If select eachother => match
                                                    //No match backward => new object
                                                    //No match forward => end of object
            tracker.simpleTracking(finishedTracks, currentTracks, detectedBlobs, frameNumber);

            //Check if Pedestrian crossed
            std::vector<Track>::iterator it;
            for(it = currentTracks.begin(); it != currentTracks.end(); it++){
                if(static_cast<int>(it->getBoxes().size()) > 1){
                    cv::Rect lastBox = it->getBoxes().back().BBox;
                    cv::Rect secondToLast = (*(it->getBoxes().rbegin()+1)).BBox;
                    cv::Point lastA = cv::Point(lastBox.x, lastBox.y+lastBox.height);
                    cv::Point lastB = cv::Point(lastBox.x+lastBox.width, lastBox.y+lastBox.height);
                    cv::Point sLastA = cv::Point(secondToLast.x, secondToLast.y+secondToLast.height);
                    cv::Point sLastB = cv::Point(secondToLast.x+secondToLast.width,
                                                           secondToLast.y+secondToLast.height);
                    cv::Point line1A = cv::Point(lines[0][0], lines[0][1]);
                    cv::Point line1B = cv::Point(lines[0][2], lines[0][3]);

                    LineSegment line1 = LineSegment(line1A, line1B);
                    LineSegment trajectLB = LineSegment(sLastA, lastA);
                    LineSegment trajectRB = LineSegment(sLastB, lastB);
                    //Check if Left-bottom crossed
                    if(Geometry::doLinesIntersect(trajectLB, line1)){
                        bool sideBeforeBL = Geometry::whatSideOfLine(line1,sLastA);
                        bool sideAfterBL = Geometry::whatSideOfLine(line1,lastA);
                        it->setBLCrossed(!it->getBLCrossed());
                        it->setBLPosToNeg(sideBeforeBL);
                        if(it->getBLCrossed()){
                            if(it->getBRCrossed() && it->getBRPosToNeg()==sideBeforeBL){ //BR previously crossed in
                                                                                         //same direction
                                if(sideBeforeBL == true){
                                    posToNegCnt++;
                                }
                                else{
                                    negToPosCnt++;
                                }
                            }
                        }
                    }
                    //Check if Right-bottom crossed
                    if(Geometry::doLinesIntersect(trajectRB, line1)){
                        bool sideBeforeBR = Geometry::whatSideOfLine(line1, sLastB);
                        bool sideAfterBR = Geometry::whatSideOfLine(line1, lastB);
                        it->setBRCrossed(!it->getBRCrossed());
                        it->setBRPosToNeg(sideBeforeBR);
                        if(it->getBRCrossed()){
                            if(it->getBLCrossed() && it->getBLPosToNeg() == sideBeforeBR){ //BL previously crossed in
                                                                                           //same direction
                                if(sideBeforeBR == true){
                                    posToNegCnt++;
                                }
                                else{
                                    negToPosCnt++;
                                }
                            }
                        }
                    }
                }
            }


            //Draw the Bounding Boxes
            for(it = currentTracks.begin(); it != currentTracks.end(); it++){
                if(it->getMatched() == true){ //Up to date
                    cv::Rect rect = it->getBoxes().back().BBox;
                    //if(rect.area() > 180){
                        cv::rectangle(newFrame, rect, it->getColor());
                    //}
                }
            }

            //Draw the configured counting lines
            std::vector< std::vector<int> >::iterator lineIt;
            for(lineIt = lines.begin(); lineIt != lines.end(); lineIt++){
                int p1x = (*lineIt)[0];
                int p1y = (*lineIt)[1];
                int p2x = (*lineIt)[2];
                int p2y = (*lineIt)[3];
                cv::Point point1 = cv::Point(p1x, p1y);
                cv::Point point2 = cv::Point(p2x, p2y);
                cv::line(newFrame, point1, point2, cv::Scalar(0,0,255),2,8,0);
            }

            //Display counters
            std::stringstream ss;
            ss << posToNegCnt;
            cv::putText(newFrame, ss.str(), cv::Point(30,newFrame.rows-30),cv::FONT_HERSHEY_SIMPLEX,1,
                        cv::Scalar(0,255,0),3,8,false);
            ss.str("");
            ss << negToPosCnt;
            cv::putText(newFrame, ss.str(), cv::Point(100,newFrame.rows-30),cv::FONT_HERSHEY_SIMPLEX,1,
                        cv::Scalar(0,255,0),3,8,false);

            //Generate windows
            cv::namedWindow("Current Frame");
            //cv::namedWindow("Background");
            //cv::namedWindow("Foreground");
            //cv::namedWindow("Variance");
            //cv::namedWindow("Confidence Measurement");
            //cv::namedWindow("Foreground Morphed");
            cv::imshow("Current Frame", newFrame);
            //cv::imshow("Background", background);
            //cv::imshow("Foreground", foreground);
            //cv::imshow("Variance", variance);
            //cv::imshow("Confidence Measurement", confidence);
            //cv::imshow("Foreground Morphed", foregroundMorph);
            //if(count<=392){
                cv::waitKey(1);
            //} else {
              //  cv::waitKey();
            //}
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



