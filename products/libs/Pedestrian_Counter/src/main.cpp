#include <iostream>

#include "ResearchDetectionFramework.h"
#include "PreDefinedParameters.h"
#include "PeopleCounter.h"
//#include "IntrusionWrapperAlgo.h"
#include <time.h>

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

    std::vector<std::string> GroupResultNames;
    for(auto const& Group : TestGroups)
    {
        GroupResultNames.push_back(Group.s_GroupResultName);
        std::vector<std::string> TestCaseResultNames;
        for(auto const& Case : Group.s_TestCases)
        {
            TestCaseResultNames.push_back(Case.s_ResultName);
            DetFramework.Configure(Case.s_VideoName, Case.s_ConfigName, Case.s_GroundTruthName, Case.s_ResultName, Group.s_GroupName);

            time_t start;
            time_t end;
            time(&start);
            DetFramework.Run();
            time(&end);
            double runtime = difftime(end, start);
            std::cout << "runtime is " << runtime << " seconds"<<std::endl;
        }

        DetFramework.FinishGroupPerformanceAnalyse(Group.s_GroupResultName, TestCaseResultNames);
    }

    std::string FinalResultName = "F:/user/Documents/FLIR_Data/Results/FinalResult.xml";
    DetFramework.FinishPerformanceAnalyse(GroupResultNames, FinalResultName);

    return 0;
}

