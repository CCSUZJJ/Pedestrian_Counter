#include "DetectionTestSpecifications.h"
#include "tinyxml.h"

DetectionTestSpecifications::DetectionTestSpecifications()
{
}

void DetectionTestSpecifications::Reset()
{
    ReferenceIdsOfTestsToPerform.clear();
    TestEntries.clear();
    NumberOfDefinedTestEntries = 0;
}

const DetectionTestEntry* DetectionTestSpecifications::GetEntry(unsigned int ReferenceId) const
{
    std::vector<DetectionTestEntry>::const_iterator It = TestEntries.begin();
    for (;It != TestEntries.end();++It)
    {
        if (It->ReferenceId == ReferenceId) return &(*It);
    }

    // Entry not found

    return 0;
}

// Parse <xxx FileName = "..."></xxx>

void DetectionTestSpecifications::GetFileName(const TiXmlNode* NodeOI,const char* NodeName,std::string& FileName) const
{
    if (NodeOI == 0) return;
    const TiXmlNode* FileNameNode = NodeOI->FirstChild(NodeName);
    if (FileNameNode == 0) return;      //Parse-error
    const TiXmlElement* Element = FileNameNode->ToElement();
    if (Element == 0) return;
    if (Element->Attribute("FileName") == 0) return;
    FileName = std::string(Element->Attribute("FileName"));
}

// <xml>
// <TestCases>
//  <Test ReferenceId = "xxx" />
//  <Test ReferenceId = "xxx" />
// </TestCases>

//	<TestCase <ReferenceId = "xxx">
//		<Video FileName = "..."></Video>
//		<Config FileName = ".."></Config>
//	</TestCase>
//</xml>

bool DetectionTestSpecifications::GetSpecifications(const std::string& FileName,std::ostream& LogFile)
{
    Reset();        // Reset before getting a new one
    if (FileName.empty()) return false;	// No filename-specified

    // Load xml-file to parse

    TiXmlDocument XmlFile;
    if (!XmlFile.LoadFile(FileName.c_str()))
    {
        LogFile << "ERROR in xml-parser " << FileName << std::endl;
        LogFile << XmlFile.ErrorDesc();
        LogFile << " @ r=" << XmlFile.ErrorRow() << " c=" << XmlFile.ErrorCol() << std::endl;
        return false;       // Fatal error ...
    }

    // Xml-file loaded
    // <xml>

    const TiXmlNode* MainNode = 0;
    MainNode = XmlFile.FirstChild("xml");

    if (MainNode != 0)
    {
        const TiXmlNode* TestNode = MainNode->FirstChild("TestCases");
        if (TestNode != 0)
        {
            // Parse the testcases
            // <TestCases>
            //  <Test ReferenceId = "xxx" />
            //  <Test ReferenceId = "xxx" />
            // </TestCases>

            const TiXmlNode* TestReferenceNode = TestNode->FirstChild("Test");
            while (TestReferenceNode != 0)
            {
                const TiXmlElement* TestReferenceElement = TestReferenceNode->ToElement();
                if (TestReferenceElement != 0)
                {
                    int ValueOI = 0;
                    if (TestReferenceElement->QueryIntAttribute("ReferenceId", &ValueOI) == TIXML_SUCCESS)
                    {
                        ReferenceIdsOfTestsToPerform.push_back((unsigned int)ValueOI);
                    }
                }

                TestReferenceNode = TestNode->IterateChildren("Test",TestReferenceNode);
            } // loop for test-references

            // Stop parsing if no testreferences have been specified

            if (ReferenceIdsOfTestsToPerform.size() > 0)
            {
                // Tests found => parse the test-specifications
                // Only save the specified tests (save memory!)

                const TiXmlNode* TestNode = MainNode->FirstChild("TestCase");
                while (TestNode != 0)
                {
                    //	<TestCase <ReferenceId = "xxx">
                    //		<Video FileName = "..."></Video>
                    //		<Config FileName = ".."></Config>
                    //	</TestCase>

                    const TiXmlElement* TestCaseElement = TestNode->ToElement();
                    if (TestCaseElement != 0)
                    {
                        int ValueOI = 0;
                        if (TestCaseElement->QueryIntAttribute("ReferenceId", &ValueOI) == TIXML_SUCCESS)
                        {
                            // ReferenceId found

                            unsigned int ReferenceId = (unsigned int) ValueOI;
                            NumberOfDefinedTestEntries++;

                            // EntryOI ?

                            bool SaveEntry = false;
                            unsigned int NumberOfTestsToPerform = (unsigned int)ReferenceIdsOfTestsToPerform.size();
                            for (unsigned int i=0;i < NumberOfTestsToPerform;++i)
                            {
                                if (ReferenceIdsOfTestsToPerform[i] == ReferenceId)
                                {
                                    SaveEntry = true;
                                    break;
                                }
                            } // End searchloop entry to save

                            // Save entry?

                            if (SaveEntry)
                            {
                                const DetectionTestEntry* AlreadyDefinedEntry = GetEntry(ReferenceId);
                                if (AlreadyDefinedEntry == 0)
                                {
                                    // ok, entry isn't defined yet

                                    DetectionTestEntry EntryOI(ReferenceId);
                                    GetFileName(TestNode,"Video",EntryOI.NameVideoFile);
                                    if (EntryOI.NameVideoFile.size() > 0)
                                    {
                                        GetFileName(TestNode,"Config",EntryOI.NameConfigurationFile);
                                        if (EntryOI.NameConfigurationFile.size() > 0)
                                        {
                                            TestEntries.push_back(EntryOI);
                                        }
                                    }
                                }
                                else
                                {
                                    LogFile << "WARNING -> skipping double defined entry =" << ReferenceId << std::endl;
                                }
                             } // end save-entry
                        }
                    }

                    TestNode = MainNode->IterateChildren("TestCase",TestNode);
                } // end loop TestCases
            }
            else
            {
                LogFile << "WARNING -> aborting processing, no tests specified" << std::endl;
            }
        } // end TestCases
        else
        {
            LogFile << "ERROR in specs " << FileName << std::endl;
            LogFile << "No TestCases-tag" << std::endl;
            return false;       // Fatal error ...
        }
    } // End xml-node found
    else
    {
        // no xml-tags found (fatal error)

        LogFile << "ERROR in specs " << FileName << std::endl;
        LogFile << "No xml-tags" << std::endl;
        return false;       // Fatal error ...
    }

    // Normal exit

    return true;
}
