#pragma once

#include <string>
#include <vector>
#include "OpenCvVideo.h"
#include "OpenCvFrame.h"
#include "DetAlgoI.h"


struct IntrusionEvent;

using namespace std;

class ResearchDetectionFramework
{
public:
    ResearchDetectionFramework();
    ResearchDetectionFramework(const SFrameWorkConfig& Config);
    ~ResearchDetectionFramework();

    void AddAlgo(IDetAlgo* Algo);

    void Configure(const std::string& FileNameVideo, const std::string& FileNameConfiguration,
                   const std::string& GroundTruthFileName, const std::string& GroupName);

    bool Run();

    void FinishPerformanceAnalyse();
    void FinishGroupPerformanceAnalyse();


private:
    bool m_Configured;
    OpenCvVideo m_VideoPlayer;
    std::vector<IDetAlgo*> m_Algo;
    std::string m_ConfigFileName;
    SFrameWorkConfig m_Config;

    bool SendConfig(const std::string& GroundTruthFileName, const std::string& GroupName);
    void SetVideoName(const std::string& VideoFileName, unsigned int FrameRate, CvSize FrameSize);
    unsigned long GetTraficonFieldCounter(const IplImage* GrayImage);

    void ClearAlgoList();
 };
