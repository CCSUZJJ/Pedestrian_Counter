#include "ResearchDetectionFramework.h"
#include <iostream>
#include <fstream>
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
#include "gtTrack.h"
#include "Geometry.h"
#include "gtEvent.h"
#include "DetAlgoI.h"

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
                                           const std::string& GroundTruthFileName, const std::string& ResultFileName, const std::string& GroupName)
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
    m_Configured = m_Configured && SendConfig(GroundTruthFileName, ResultFileName, GroupName);

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

bool ResearchDetectionFramework::SendConfig(const std::string& GroundTruthFileName, const std:: string& ResultFileName
                                            ,const std::string& GroupName)
{
    bool RetVal = false;
    std::cout << "Parse Config file: " << m_ConfigFileName << std::endl;
    TiXmlDocument XmlDoc;
    //ticpp::Document XmlDoc;
    m_Config.s_XmlDoc = &XmlDoc;
    m_Config.s_GroundTruthFile = GroundTruthFileName;
    m_Config.s_ResultFile = ResultFileName;
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

        //Generate Ground Truth tracks
        ticpp::Document dom(m_Config.s_GroundTruthFile.c_str());
        try{
            dom.LoadFile();
        }
        catch(ticpp::Exception& ex){
            std::cout << ex.what();
        }

        ticpp::Element* frameNodes = dom.FirstChild("GroundTruth")->FirstChildElement("Frames");
        std::vector<gtTrack> gtTracks;
        std::vector<gtTrack> currSeqGtTracks;
        std::vector<int> existingIDs;

        ticpp::Iterator < ticpp::Element > frame("Frame");
        for(frame = frame.begin(frameNodes); frame != frame.end(); frame++){
            if(frame->FirstChildElement("BBox", 0) != NULL){    //BBoxes aanwezig, geen exception werpen als null
                int frameNr;
                frame->GetAttribute("Nr", &frameNr);

                ticpp::Element* currFrame = frame.Get();
                ticpp::Iterator< ticpp::Element > CurrBox ( "BBox" );
                for(CurrBox = CurrBox.begin(currFrame); CurrBox != CurrBox.end(); CurrBox++){
                    int x, y, w, h, id, catid;
                    CurrBox->GetAttribute("X", &x);
                    CurrBox->GetAttribute("Y", &y);
                    CurrBox->GetAttribute("W", &w);
                    CurrBox->GetAttribute("H", &h);
                    CurrBox->GetAttribute("ID", &id);
                    CurrBox->GetAttribute("CatID", &catid);
                    DetectedBlob newBlob;
                    newBlob.BBox = cv::Rect(x,y,w,h);
                    newBlob.frameNr = frameNr;

                    if(std::find(existingIDs.begin(),existingIDs.end(), id) != existingIDs.end()){
                        //id already exists in current sequence => find existing object, add box
                        std::vector<gtTrack>::iterator obj;
                        for(obj = currSeqGtTracks.begin(); obj != currSeqGtTracks.end(); obj++){
                            if(obj->getID() == id){
                                obj->addBlob(newBlob);
                            }
                        }
                    }
                    else{
                        //create new gtTrack, add id to existing ID's
                        gtTrack newTrack;
                        newTrack.setCatID(catid);
                        newTrack.setID(id);
                        newTrack.addBlob(newBlob);
                        existingIDs.push_back(id);
                        currSeqGtTracks.push_back(newTrack);
                    }
                }
            }
            else{   //end of current sequence of frames that contains BBoxes
                for(gtTrack track : currSeqGtTracks){
                    gtTracks.push_back(track);
                }
                if(!(currSeqGtTracks.empty() || existingIDs.empty())){ //empty the vectors
                    currSeqGtTracks.clear();
                    existingIDs.clear();
                }
            }
        }

        //Generate Ground Truth events
        std::vector<gtEvent> gtEvents;
        std::vector<gtTrack>::iterator gtIt;
        for(gtIt = gtTracks.begin(); gtIt != gtTracks.end(); gtIt++){
            gtEvent event;
            std::vector<DetectedBlob>::iterator blobIt;
            for(blobIt = gtIt->getBoxes().begin()+1; blobIt != gtIt->getBoxes().end()-1; blobIt++){
                cv::Rect box = blobIt->BBox;
                std::vector<DetectedBlob>::iterator tmpIt = blobIt+1;
                cv::Rect nextBox = tmpIt->BBox;

                cv::Point boxA = cv::Point(box.x,box.y+box.height);
                cv::Point boxB = cv::Point(box.x+box.width,box.y+box.height);
                cv::Point nxBoxA = cv::Point(nextBox.x, nextBox.y+nextBox.height);
                cv::Point nxBoxB = cv::Point(nextBox.x+nextBox.width, nextBox.y+nextBox.height);
                cv::Point line1A = cv::Point(lines[0][0], lines[0][1]);
                cv::Point line1B = cv::Point(lines[0][2], lines[0][3]);

                LineSegment line1 = LineSegment(line1A, line1B);
                LineSegment trajectLB = LineSegment(boxA, nxBoxA);
                LineSegment trajectRB = LineSegment(boxB, nxBoxB);

                //Check if Left-bottom crossed
                if(Geometry::doLinesIntersect(trajectLB, line1)){
                    bool sideBeforeBL = Geometry::whatSideOfLine(line1,boxA);
                    bool sideAfterBL = Geometry::whatSideOfLine(line1,nxBoxA);
                    gtIt->setBLCrossed(!gtIt->getBLCrossed());
                    gtIt->setBLPosToNeg(sideBeforeBL);
                    if(gtIt->getBLCrossed() && gtIt->getBRCrossed()){ // BR already crossed
                        if(sideBeforeBL == gtIt->getBRPosToNeg()){    // in same direction
                            event.setEnd(tmpIt->frameNr);
                            event.setEndRect(nextBox);
                            event.setPosToNeg(sideBeforeBL);
                            gtEvents.push_back(event);
                            std::cout<<"GTru event "<<event.getPosToNeg()<<" to "<<!event.getPosToNeg()
                                    <<" from frame "<<event.getStart()<<" to "<<event.getEnd()<<std::endl;
                        }
                    }
                    else if(gtIt->getBLCrossed() && !gtIt->getBRCrossed()){                                             //only BL crossed -> new event
                        event.setStart(blobIt->frameNr);
                        event.setStartRect(box);
                        event.setPosToNeg(sideBeforeBL);
                    }
                }
                //Check if Right-bottom crossed
                if(Geometry::doLinesIntersect(trajectRB, line1)){
                    bool sideBeforeBR = Geometry::whatSideOfLine(line1, boxB);
                    bool sideAfterBR = Geometry::whatSideOfLine(line1, nxBoxB);
                    gtIt->setBRCrossed(!gtIt->getBRCrossed());
                    gtIt->setBRPosToNeg(sideBeforeBR);
                    if(gtIt->getBLCrossed() && gtIt->getBRCrossed()){
                        if(sideBeforeBR == gtIt->getBLPosToNeg()){
                            event.setEnd(tmpIt->frameNr);
                            event.setEndRect(nextBox);
                            event.setPosToNeg(sideBeforeBR);
                            gtEvents.push_back(event);
                            std::cout<<"GTru event "<<event.getPosToNeg()<<" to "<<!event.getPosToNeg()
                                    <<" from frame "<<event.getStart()<<" to "<<event.getEnd()<<std::endl;
                        }
                    }
                    else if(gtIt->getBRCrossed() && !gtIt->getBLCrossed()){
                        event.setStart(blobIt->frameNr);
                        event.setStartRect(box);
                        event.setPosToNeg(sideBeforeBR);
                    }
                }
            }
        }

        //Init runningAvgBackground
        //cv::Mat background = m_VideoPlayer.GetNextFrame();
        //cv::cvtColor(background, background, CV_BGR2GRAY);
        //m_VideoPlayer.Init();                               //Videoplayer back at frame 0
        //-------------------------------------------------------------------------------------------
        //Init Median
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


        std::vector<AlgoEvent> algoEvents;
        int posToNegCnt =0;
        int negToPosCnt =0;

        int frameNumber = -1;       //init at -1 => first framenumber = 0
        int count = 0;            //to stop at certain frame
        while(true){
            count++;
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
            //fgSegment.medianBackground(currFrame, background, foreground);
            fgSegment.sigmaDeltaCMBackground(currFrame, background, foreground, frameCount, confidence, variance,
                                           detectionCount, toUpdate);

            //Morphological Operations: opening -> closing
            cv::Mat foregroundMorph;
            //Remove noise
            cv::Mat kernel = cv::Mat::ones(cv::Size(2,2), CV_8UC1);
            cv::morphologyEx(foreground, foregroundMorph, cv::MORPH_OPEN, kernel);
            //Connect objects
            cv::Mat kernel2 =  cv::Mat::ones(cv::Size(5,5), CV_8UC1);
            cv::morphologyEx(foregroundMorph, foregroundMorph, cv::MORPH_CLOSE, kernel2);
            cv::morphologyEx(foregroundMorph, foregroundMorph, cv::MORPH_CLOSE, kernel2);
            cv::morphologyEx(foregroundMorph, foregroundMorph, cv::MORPH_CLOSE, kernel2);
            //cv::Mat kernel2 =  cv::Mat::ones(cv::Size(20,20), CV_8UC1);
            //cv::morphologyEx(foregroundMorph, foregroundMorph, cv::MORPH_CLOSE, kernel2);

            //Blob Segmentation
            //std::vector<cv::Rect> boundRect = blobSegment.contourSegment(foregroundMorph);
            //std::vector<cv::Rect> boundRect = blobSegment.intensitySegment(foregroundMorph); //Unfinished
            std::vector<DetectedBlob> detectedBlobs = blobSegment.connectedComponentSegment(foregroundMorph, frameNumber);


            //Tracking
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
                        it->setBLCrossed(!it->getBLCrossed());  //not yet crossed <-> crossed
                        it->setBLPosToNeg(sideBeforeBL);        //remember direction
                        if(it->getBLCrossed() && it->getBRCrossed()){   //if BR already crossed
                            if(sideBeforeBL == it->getBRPosToNeg()){    //in same direction -> end the event
                                if((!it->getCounted(sideBeforeBL))      //not counted before in this direction
                                && (it->getConfidence() >= 25)      //has existed long enough
                                && (it->isPedestrian())){           //classified enough times
                                    AlgoEvent event = it->getEvent();
                                    event.setEnd(frameNumber);
                                    event.setEndRect(lastBox);
                                    event.setPosToNeg(sideBeforeBL);
                                    it->setEvent(event);
                                    it->setCounted(sideBeforeBL);
                                    algoEvents.push_back(event);
                                    if(sideBeforeBL == true){
                                        posToNegCnt++;
                                        std::cout<<"Algo event 1 to 0 from frame "<<event.getStart()<<" to "<<event.getEnd()<<std::endl;
                                    }
                                    else{
                                        negToPosCnt++;
                                        std::cout<<"Algo event 0 to 1 from frame "<<event.getStart()<<" to "<<event.getEnd()<<std::endl;
                                    }
                                }
                            }
                        }
                        else if(it->getBLCrossed() && !it->getBRCrossed()){ //only BL has crossed -> new event
                            AlgoEvent event;
                            event.setStart(frameNumber);
                            event.setStartRect(secondToLast);
                            event.setPosToNeg(sideBeforeBL);
                            it->setEvent(event);
                        }
                    }
                    //Check if Right-bottom crossed
                    if(Geometry::doLinesIntersect(trajectRB, line1)){
                        bool sideBeforeBR = Geometry::whatSideOfLine(line1,sLastB);
                        it->setBRCrossed(!it->getBRCrossed());
                        it->setBRPosToNeg(sideBeforeBR);
                        if(it->getBRCrossed() && it->getBLCrossed()){   //if BL already crossed
                            if(sideBeforeBR == it->getBLPosToNeg()){    //in same direction -> end the event
                                if(!it->getCounted(sideBeforeBR)    //not counted before
                                && (it->getConfidence() >= 25)      //has existed long enough
                                && (it->isPedestrian())){           //if not counted before in this direction
                                    AlgoEvent event = it->getEvent();
                                    event.setEnd(frameNumber);
                                    event.setEndRect(lastBox);
                                    event.setPosToNeg(sideBeforeBR);
                                    it->setEvent(event);
                                    it->setCounted(sideBeforeBR);
                                    algoEvents.push_back(event);
                                    if(sideBeforeBR == true){
                                        posToNegCnt++;
                                        std::cout<<"Algo event 1 to 0 from frame "<<event.getStart()<<" to "<<event.getEnd()<<std::endl;
                                    }
                                    else{
                                        negToPosCnt++;
                                        std::cout<<"Algo event 0 to 1 from frame "<<event.getStart()<<" to "<<event.getEnd()<<std::endl;
                                    }
                                }
                            }
                        }
                        else if(it->getBRCrossed() && !it->getBLCrossed()){ //only BR has crossed -> new event
                            AlgoEvent event;
                            event.setStart(frameNumber);
                            event.setStartRect(secondToLast);
                            event.setPosToNeg(sideBeforeBR);
                            it->setEvent(event);
                        }
                    }
                }
            }


            //Draw the Bounding Boxes
            for(it = currentTracks.begin(); it != currentTracks.end(); it++){
                if(it->getMatched() == true){ //Up to date && 40x classified as pedestrian
                    cv::Rect rect = it->getBoxes().back().BBox;
                    //if(rect.area() > 180){
                        cv::Scalar color = cv::Scalar(0,0,255);
                        if(it->isPedestrian())
                            color = cv::Scalar(0,255,0);
                        cv::rectangle(newFrame, rect, color);
                        std::stringstream ss;
                        //ss << it->getPedestrianCnt();
                        //cv::putText(newFrame,ss.str(),cv::Point(rect.x+4, rect.y+4),cv::FONT_HERSHEY_SIMPLEX,1,
                        //            it->getColor(),3,8,false);
                    //}
                }
            }
            //Draw Ground truth
//            std::vector<gtTrack>::iterator gtIt;
//            for(gtIt = gtTracks.begin(); gtIt != gtTracks.end(); gtIt++){
//                std::vector<DetectedBlob> dbs = gtIt->getBoxes();
//                std::vector<DetectedBlob>::iterator dbIt;
//                for(dbIt = dbs.begin(); dbIt != dbs.end(); dbIt++){
//                    if(dbIt->frameNr == frameNumber){
//                        cv::rectangle(newFrame,dbIt->BBox,cv::Scalar(0,255,0));
//                    }
//                }
//            }

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
            //cv::namedWindow("Foreground Morphed");
            //cv::namedWindow("Variance");
            //cv::namedWindow("Confidence Measurement");
            cv::imshow("Current Frame", newFrame);
            //cv::imshow("Background", background);
            //cv::imshow("Foreground", foreground);
            //cv::imshow("Foreground Morphed", foregroundMorph);
            //cv::imshow("Variance", variance);
            //cv::imshow("Confidence Measurement", confidence);
            //if(count<=1){
                cv::waitKey(1);
            //} else {
            //    cv::waitKey();
            //}
        }

        //Write Algo Events to XML
        ticpp::Document result;
        ticpp::Declaration dec = ticpp::Declaration("1.0", "utf-8", "");
        ticpp::Declaration* decp = &dec;
        result.LinkEndChild(decp);

        ticpp::Element results("Results");
        ticpp::Element gTruEvents("GroundTruthEvents");
        std::vector<gtEvent>::iterator gIt;
        for(gIt = gtEvents.begin(); gIt != gtEvents.end(); gIt++){
            ticpp::Element gEvent("GroundTruthEvent");
            gEvent.SetAttribute("Start", gIt->getStart());
            gEvent.SetAttribute("End", gIt->getEnd());
            gEvent.SetAttribute("Direction", gIt->getPosToNeg());
            gTruEvents.InsertEndChild(gEvent);
        }
        results.InsertEndChild(gTruEvents);
        ticpp::Element algEvents("AlgorithmEvents");
        std::vector<AlgoEvent>::iterator aIt;
        for(aIt = algoEvents.begin(); aIt != algoEvents.end(); aIt++){
            ticpp::Element algEvent("AlgorithmEvent");
            algEvent.SetAttribute("Start", aIt->getStart());
            algEvent.SetAttribute("End", aIt->getEnd());
            algEvent.SetAttribute("Direction", aIt->getPosToNeg());
            algEvents.InsertEndChild(algEvent);
        }
        results.InsertEndChild(algEvents);

        result.InsertEndChild(results);

        std::ofstream file;
        file.open(m_Config.s_ResultFile);
        file << result;
        file.close();
    }
    return true;
}

void ResearchDetectionFramework::FinishPerformanceAnalyse(const std::vector<std::string>& GroupResultNames, const string &FinalResultName)
{
    /*for(auto& Algo : m_Algo)
    {
        Algo->FinishPerformanceAnalyse();
    }*/

    int gtEvents = 0;
    int AlgoEvents = 0;
    int TP = 0;
    int FP = 0;
    int FN = 0;
    double sensitivity;
    double precision;

    for(auto GroupResultName : GroupResultNames){
        ticpp::Document doc(GroupResultName.c_str());
        doc.LoadFile();
        ticpp::Element* GroupResult = doc.FirstChildElement("GroupResult");
        int grpGTE, grpALG, grpTP, grpFP, grpFN;
        GroupResult->GetAttribute("AlgorithmEvents", &grpALG);
        GroupResult->GetAttribute("GroundTruthEvents", &grpGTE);
        GroupResult->GetAttribute("TruePositive", &grpTP);
        GroupResult->GetAttribute("FalsePositive", &grpFP);
        GroupResult->GetAttribute("FalseNegative", &grpFN);
        gtEvents += grpGTE;
        AlgoEvents += grpALG;
        TP += grpTP;
        FP += grpFP;
        FN += grpFN;
    }

    sensitivity = 100.0*TP/(TP+FN);
    precision = 100.0*TP/(TP+FP);

    ticpp::Document result;
    ticpp::Declaration dec = ticpp::Declaration("1.0", "utf-8", "");
    ticpp::Declaration* decp = &dec;
    result.LinkEndChild(decp);

    ticpp::Element FinalResult("FinalResult");
    FinalResult.SetAttribute("AlgorithmEvents", AlgoEvents);
    FinalResult.SetAttribute("GroundTruthEvents", gtEvents);
    FinalResult.SetAttribute("TruePositive", TP);
    FinalResult.SetAttribute("FalsePositive", FP);
    FinalResult.SetAttribute("FalseNegative", FN);
    FinalResult.SetAttribute("Sensitivity", sensitivity);
    FinalResult.SetAttribute("Precision", precision);

    result.InsertEndChild(FinalResult);

    std::ofstream file;
    file.open(FinalResultName.c_str());
    file << result;
    file.close();
}

void ResearchDetectionFramework::FinishGroupPerformanceAnalyse(const string &GroupResultName, std::vector<std::string>& TestCaseResultNames)
{
    /*for(auto& Algo : m_Algo)
    {
        Algo->FinishGroupPerformanceAnalyse();
    }*/

    //Calculate sensitivity and precision for each group
    std::vector<AlgoEvent> algoEvents;
    std::vector<gtEvent> gtEvents;
    int TP = 0;
    int FP = 0;
    int FN = 0;
    double sensitivity;
    double precision;

    for(auto TestCaseResultName : TestCaseResultNames){
        ticpp::Document doc(TestCaseResultName.c_str());
        doc.LoadFile();
        ticpp::Element* parent = doc.FirstChild("Results")->FirstChildElement("GroundTruthEvents");
        ticpp::Iterator< ticpp::Element > iterator("GroundTruthEvent");
        for(iterator = iterator.begin(parent); iterator != iterator.end(); iterator++){
            ticpp::Element* xmlEvent = iterator.Get();
            gtEvent event;
            int start, end;
            xmlEvent->GetAttribute("Start", &start);
            xmlEvent->GetAttribute("End", &end);
            event.setStart(start);
            event.setEnd(end);
            gtEvents.push_back(event);
        }
        parent = doc.FirstChild("Results")->FirstChildElement("AlgorithmEvents");
        ticpp::Iterator< ticpp::Element > iterator2("AlgorithmEvent");
        for(iterator2 = iterator2.begin(parent); iterator2 != iterator2.end(); iterator2++){
            ticpp::Element* xmlEvent = iterator2.Get();
            AlgoEvent event;
            int start, end;
            xmlEvent->GetAttribute("Start", &start);
            xmlEvent->GetAttribute("End", &end);
            event.setStart(start);
            event.setEnd(end);
            event.setFoundInGT(false);
            algoEvents.push_back(event);
        }
    }

    std::vector<AlgoEvent>::iterator alge;
    std::vector<gtEvent>::iterator gte;
    for(gte = gtEvents.begin(); gte != gtEvents.end(); gte++){
        for(alge = algoEvents.begin(); alge != algoEvents.end(); alge++){
            if(gte->isMatch(*alge)){          //Find matches in the Algorithm Events
                alge->setFoundInGT(true);    //Find match => True Positive
                gte->setFoundMatch(true);
                TP++;
                break;
            }
        }
        if(!(gte->getFoundMatch())){           //No match => False Negative
            FN++;
        }
    }

    for(alge = algoEvents.begin(); alge != algoEvents.end(); alge++){ //All unmatched algorithm events are False Positive
        if(!(alge->getFoundInGT())){
            FP++;
        }
    }

    sensitivity = 100.0*TP/(TP+FN);
    precision = 100.0*TP/(TP+FP);

    ticpp::Document result;
    ticpp::Declaration dec = ticpp::Declaration("1.0", "utf-8", "");
    ticpp::Declaration* decp = &dec;
    result.LinkEndChild(decp);

    ticpp::Element GroupResult("GroupResult");
    GroupResult.SetAttribute("AlgorithmEvents", static_cast<unsigned int>(algoEvents.size()));
    GroupResult.SetAttribute("GroundTruthEvents", static_cast<unsigned int>(gtEvents.size()));
    GroupResult.SetAttribute("TruePositive", TP);
    GroupResult.SetAttribute("FalsePositive", FP);
    GroupResult.SetAttribute("FalseNegative", FN);
    GroupResult.SetAttribute("Sensitivity", sensitivity);
    GroupResult.SetAttribute("Precision", precision);

    result.InsertEndChild(GroupResult);

    std::ofstream file;
    file.open(GroupResultName.c_str());
    file << result;
    file.close();
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



