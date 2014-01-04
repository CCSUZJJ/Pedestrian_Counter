#ifndef DETECTIONTESTSPECIFICATIONS_H
#define DETECTIONTESTSPECIFICATIONS_H

#include "DetectionTestEntry.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "tinyxml.h"

class DetectionTestSpecifications
{
public:
    DetectionTestSpecifications();

    // Functions

    void Reset();
    bool GetSpecifications(const std::string& FileName,std::ostream& LogFile);
    const DetectionTestEntry* GetEntry(unsigned int ReferenceId) const;

    unsigned int GetNumberOfTestsToPerform() const { return (unsigned int)ReferenceIdsOfTestsToPerform.size(); }
    unsigned int GetNumberOfDefinedTests() const { return NumberOfDefinedTestEntries; }

    // Tests to perform

    std::vector<unsigned int> ReferenceIdsOfTestsToPerform;

private:

    // TestEntries (only entries of interest are kept!)

    unsigned int NumberOfDefinedTestEntries;
    std::vector<DetectionTestEntry> TestEntries;

    // Helper-functions

    void GetFileName(const TiXmlNode* NodeOI,const char* NodeName,std::string& FileName) const;
};

#endif // DETECTIONTESTSPECIFICATIONS_H
