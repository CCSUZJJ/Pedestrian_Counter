#ifndef _DET_ALGOI_H_INCLUDED
#define _DET_ALGOI_H_INCLUDED
#include <iostream>
#include <string>
//#include "ticpp.h"

class OpenCvFrame;
class CvSize;
class TiXmlDocument;
//class ticpp::Document;

struct SFrameWorkConfig
{
    SFrameWorkConfig(): s_XmlDoc(), s_TestBatchFile(), s_GroundTruthFile(), s_ResultFile(),s_GroupName(),
                        s_ConfigFile(), s_Debug(false), s_CreateVideo(false), s_CommandLineMode(false)
    {}

    TiXmlDocument*  s_XmlDoc;
    //ticpp::Document* s_XmlDoc;
    std::string     s_TestBatchFile;
    std::string     s_GroundTruthFile;
    std::string     s_ResultFile;
    std::string     s_GroupName;
    std::string     s_ConfigFile;
    bool            s_Debug;
    bool            s_CreateVideo;
    bool            s_CommandLineMode;
};

class IDetAlgo
{
   public:
	IDetAlgo(){}
    virtual ~IDetAlgo() {}

	virtual bool ReadSample (OpenCvFrame& InputFrame) = 0; //!< This method should only copy the needed pixel values

    virtual bool Run        () = 0; //!< The Run method : performs all the calcuations

    virtual bool Configure(const SFrameWorkConfig& Config) = 0; //!< Configure the algorithm

    virtual void SetFieldCounter(unsigned int FieldCounter) = 0;

    virtual bool HasWork() = 0;

    virtual void ProcessPerformanceAnalyse() = 0;

    virtual void FinishGroupPerformanceAnalyse() = 0;

    virtual void FinishPerformanceAnalyse() = 0;

    virtual void SetVideoName(const std::string& Path, const std::string& VideoName,
                              unsigned int FrameRate, CvSize& FrameSize, bool CreateVideo = false) = 0;
};

#endif //_DET_ALGOI_H_INCLUDED

