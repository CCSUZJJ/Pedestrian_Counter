#include "ticpp.h"
#include <deque>
#include <fstream>

namespace
{
    const std::string AllIntrusion_Path("F:/user/Documents/FLIR/Pedestrian_Counter/products/libs/Pedestrian_Counter/src/Tapes.xml");
}

struct STestCases
{
    STestCases() :
        s_VideoName(), s_GroundTruthName(), s_CaseName(), s_ConfigName()
    {}

    std::string s_VideoName;
    std::string s_GroundTruthName;
    std::string s_CaseName;
    std::string s_ConfigName;
};

struct STestGroup
{
    STestGroup() :
        s_TestCases(), s_GroupName()
    {}

    std::deque<STestCases> s_TestCases;
    std::string s_GroupName;
};

bool CheckConfigName(const std::string& ConfigName)
{
    std::ifstream File(ConfigName.c_str());
    return File.good();
}


std::deque<STestGroup> ReadAllTests()
{
    std::deque<STestGroup> GroupResults;

    std::string GlobalPath;
    ticpp::Document Dom(AllIntrusion_Path.c_str());
    ticpp::Element* Groups;

    try{
        Dom.LoadFile();
        //std::cout << "Dom.LoadFile() check" <<std::endl;
    }
    catch(ticpp::Exception& ex){
        std::cout << ex.what();
    }

    try{
        ticpp::Element* GlobalInfo = Dom.FirstChild("xml")->FirstChildElement("VideoPath");
        if(GlobalInfo)
        {
            GlobalInfo->GetAttribute("Path", &GlobalPath);  //GlobalPath = std::string(GlobalInfo->Attribute("Path"));
        }

        Groups = Dom.FirstChild("xml")->ToElement();
    }
    catch(ticpp::Exception& ex){
        std::cout << ex.what();
    }

    try{
        //Loop over all test groups
        ticpp::Iterator< ticpp::Element > CurrGroup( "TestCase" );
        for(CurrGroup = CurrGroup.begin(Groups); CurrGroup != CurrGroup.end(); CurrGroup++)
        //for (ticpp::Element* CurrGroup = Groups->FirstChildElement("TestCase"); CurrGroup; CurrGroup = CurrGroup->NextSiblingElement("TestCase"))
        {
            std::string AnnotationMap;
                CurrGroup->GetAttribute("AnnotationMap", &AnnotationMap);
            std::string VideoMap;
                CurrGroup->GetAttribute("VideoMap", &VideoMap);

            std::deque<STestCases> Results;

            //ticpp::Iterator< ticpp::Element > CurrVideo( "Test" );
            //for(CurrVideo = CurrVideo.begin(CurrGroup/*->FirstChildElement("Test")*/); CurrVideo != CurrVideo.end(); CurrVideo++)
            for(ticpp::Element* CurrVideo = CurrGroup->FirstChildElement("Test"); CurrVideo; CurrVideo = CurrVideo->NextSiblingElement("Test", false))
            {
                STestCases NewCase;
                std::string VideoName;
                    CurrVideo->GetAttribute("Video", &VideoName);
                std::string GroundTruthName;
                    CurrVideo->GetAttribute("Annotations", &GroundTruthName);
                std::string Name;
                    CurrVideo->GetAttribute("Name", &Name);
                std::string ConfigName;
                    CurrVideo->GetAttribute("Config", &ConfigName);

                NewCase.s_VideoName = GlobalPath + VideoMap + VideoName;
                NewCase.s_GroundTruthName = GlobalPath + AnnotationMap  + GroundTruthName;
                NewCase.s_CaseName = Name;
                NewCase.s_ConfigName = GlobalPath /*+ Name */+ ConfigName;
                if(!CheckConfigName(NewCase.s_ConfigName))
                {
                    NewCase.s_ConfigName = GlobalPath + Name + std::string("Config.xml");
                }
                Results.push_back(NewCase);
                //std::cout << NewCase.s_CaseName << std::endl;
            }

            STestGroup CurrGroupRes;
            CurrGroupRes.s_TestCases = Results;

            std::string GroupName = AnnotationMap;
            std::size_t Found = GroupName.find("/Annotations");
            if(Found != std::string::npos)
            {
                GroupName = GroupName.substr(0,Found);
            }
            CurrGroupRes.s_GroupName = GroupName;

            GroupResults.push_back(CurrGroupRes);
            //std::cout << CurrGroupRes.s_GroupName << std::endl;     //GROUPS LUKKEN ITT NEWCASES
        }
    }
    catch(ticpp::Exception& ex){
        std::cout << ex.what();
    }

    return GroupResults;
}
