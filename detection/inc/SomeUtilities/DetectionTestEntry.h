#ifndef DETECTIONTESTENTRY_H
#define DETECTIONTESTENTRY_H

#include <string>

class DetectionTestEntry
{
public:
    DetectionTestEntry(unsigned int ReferenceId);

    // TestEntry

    unsigned int ReferenceId;
    std::string NameVideoFile;
    std::string NameConfigurationFile;
};

#endif // DETECTIONTESTENTRY_H
