#include <iostream>

#include "ResearchDetectionFramework.h"
#include "PreDefinedParameters.h"
#include "PeopleCounter.h"
//#include "IntrusionWrapperAlgo.h"

int main(int , char *[])
{
    std::cout << "Start application" << std::endl;

    std::deque<STestGroup> TestGroups = ReadAllTests();

    ResearchDetectionFramework DetFramework;

    // Make sure you use the correct order
    //      1. Add the correct algo.
    //      2. Configure the framework
    //      3. Run the detection framework

    //DetFramework.AddAlgo(//IN TE VULLEN);

    for(auto const& Group : TestGroups)
    {
        for(auto const& Case : Group.s_TestCases)
        {
            DetFramework.Configure(Case.s_VideoName, Case.s_ConfigName, Case.s_GroundTruthName, Group.s_GroupName);
            DetFramework.Run();
        }

        DetFramework.FinishGroupPerformanceAnalyse();
    }

    DetFramework.FinishPerformanceAnalyse();

    return 0;
}

