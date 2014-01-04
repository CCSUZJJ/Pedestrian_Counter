#ifndef DETECTIONFRAMEWORK_H
#define DETECTIONFRAMEWORK_H

#include <vector>
#include <string>
#include <fstream>
#include <opencv/cv.h> // includes OpenCV definitions

namespace CvDetectionFrameworkExitCodes
{
const int NormalExit                            = 0;
const int InvalidVideoFilename                  = 100;
const int NoVideoCapture                        = 101;
const int NoFrameSize                           = 102;
const int NoConfiguration                       = 103;
const int EmptyFileNameTestEntries              = 104;
const int ErrorInParsingTestEntries             = 105;
}

class CvDetectionFramework
{
public:
    CvDetectionFramework();
    virtual ~CvDetectionFramework();

    // Functions
    
    void Reset();
    
    // Start detector in performance-evaluation-mode
    
    int Start(const std::string& FileNameVideo,const std::string& FileNameConfiguration,const std::string& FileNameEventOutput);
    
    // Start detector in development-mode

    int Start(const std::string& FileNameTestSpecifications);

    // SetUp-parameters
    
    std::string FileNameVideo;					// FullPath of the video [Mandatory parameter]
    std::string FileNameConfiguration;			// FullPath of the configurationfile [Mandatory parameter]
    
    // If the FileNameEventOutput ain't specified, then the logging of the events will be disabled
    
    std::string FileNameEventOutput;            //  FullPath of the event-log_file [Optional parameter]
    
    // ControlMode
    // DevelopmentMode = true : development
    //                 = false : offline-detector (performance-evaluation)
    
    bool DevelopmentMode;

    // ImageFlow-control

    bool PauzeDetectionFrameWork;

    // Image and video-statistics

    unsigned int FrameCounter;
    unsigned int FieldCounter;
    unsigned int FrameRate;
    bool ValidFieldCounter;

    // +--------------------+
    // | DETECTOR-INTERFACE |
    // +--------------------+
    //
    // Calling sequence
    //     Configure --> {Run} over all the frames --> ExitDetection

    virtual void Configure(const std::string& XMLConfiguration) = 0;
    virtual void Run(const IplImage* ImageOI) = 0;
    virtual void ExitDetection() = 0;

    // Helper-functions

    void GetShortFileNameOfVideo(std::string& ShortFileName,bool ExtensionRemoved=false) const;

private:

    // The main-processing loop
    //  Fetch frame from video -> run detector, aso...

    int ProcessFrames();
    unsigned int GetTraficonFieldCounter(const IplImage* GrayImage);
    bool RetrieveXmlAsString(const std::string& ConfigFile, std::string& Contents);
};

#endif // DETECTIONFRAMEWORK_H
