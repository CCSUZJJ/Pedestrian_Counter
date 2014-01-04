#include "CvDetectionFramework.h"
#include "DetectionTestSpecifications.h"
#include "tinyxml.h"
#include "OpenCvYFrameUtilities.h"
#include <opencv/highgui.h> // includes highGUI definitions
#include "FrameDecoding.h"

using namespace std;

// Static control (mouse-interaction and analysis)

CvDetectionFramework::CvDetectionFramework()
{
    Reset();
}

CvDetectionFramework::~CvDetectionFramework()
{
    Reset();
}

void CvDetectionFramework::Reset()
{
    FileNameVideo.clear();					// FullPath of the video [Mandatory parameter]
    FileNameConfiguration.clear();			// FullPath of the configurationfile [Mandatory parameter]
    FileNameEventOutput.clear();            //  FullPath of the event-log_file [Optional parameter]

    DevelopmentMode = false;
    PauzeDetectionFrameWork= false;

    FrameCounter = 0;
    FieldCounter = 0;
    FrameRate = 0;
    ValidFieldCounter = false;
}

bool CvDetectionFramework::RetrieveXmlAsString(const std::string& ConfigFile, std::string& Contents)
{
    bool RetValue = true;
    // open the config file
    ifstream inBestand(ConfigFile.c_str(), ios::in);
    if(!inBestand)
    {
        std::cout << "Can't open configuration-file ";
        std::cout << ConfigFile  << std::endl;
        RetValue = false;
    }
    else
    {
        ostringstream oss;
        oss << inBestand.rdbuf();
        Contents.assign(oss.str());
        inBestand.close();
    }
    return RetValue;
} // end of RetrieveXmlAsCharString


// Start detector in performance-evaluation-mode

int CvDetectionFramework::Start(const std::string& FileNameVideo,const std::string& FileNameConfiguration,const std::string& FileNameEventOutput)
{
    Reset();

    DevelopmentMode = false;
    this->FileNameVideo.clear();					// FullPath of the video [Mandatory parameter]
    this->FileNameConfiguration.clear();			// FullPath of the configurationfile [Mandatory parameter]
    this->FileNameEventOutput.clear();

    this->FileNameVideo = FileNameVideo;
    this->FileNameConfiguration = FileNameConfiguration;
    this->FileNameEventOutput = FileNameEventOutput;

    // Enjoy the ride!

    return ProcessFrames();
}

// Start detector in development-mode

int CvDetectionFramework::Start(const std::string& FileNameTestSpecifications)
{
    int ReturnValue = CvDetectionFrameworkExitCodes::NormalExit;
    Reset();
    if (FileNameTestSpecifications.size() == 0) return CvDetectionFrameworkExitCodes::EmptyFileNameTestEntries;

    std::cout << " Processing predefined testcases in " << FileNameTestSpecifications << std::endl;

    DetectionTestSpecifications TestSpecifications;
    if (TestSpecifications.GetSpecifications(FileNameTestSpecifications,std::cout))
    {
        // Log a nice message about the parsing

        std::cout << " " << TestSpecifications.GetNumberOfTestsToPerform();
        std::cout << " tests of ";
        std::cout << TestSpecifications.GetNumberOfDefinedTests();
        std::cout << " of defined tests"  << std::endl;

        // Start processing all testcases

        DevelopmentMode = true;
        std::vector<unsigned int>::const_iterator It = TestSpecifications.ReferenceIdsOfTestsToPerform.begin();
        for (;It != TestSpecifications.ReferenceIdsOfTestsToPerform.end();++It)
        {
            ReturnValue = 0;
            const DetectionTestEntry* TestOI = TestSpecifications.GetEntry(*It);
            if (TestOI != 0)
            {
                // Make and start detection framework

                std::cout << "TestCase = " << *It  << std::endl;

                FileNameVideo = TestOI->NameVideoFile;                      // FullPath of the video [Mandatory parameter]
                FileNameConfiguration = TestOI->NameConfigurationFile;		// FullPath of the configurationfile [Mandatory parameter]

                int ErrorExit = ProcessFrames();
                if (ErrorExit != 0) ReturnValue = ErrorExit;
            }
            else
            {
                std::cout << "SKIPPING TEST_ID = " << *It << " No specs"  << std::endl;
            }
        } // End loop test-cases
    }
    else
    {
        std::cout << "**** FATAL ERROR *** in processing testspecifications"  << std::endl;
        ReturnValue = CvDetectionFrameworkExitCodes::ErrorInParsingTestEntries;
    }

    return ReturnValue;
}

int CvDetectionFramework::ProcessFrames()
{
    int ReturnValue = CvDetectionFrameworkExitCodes::NormalExit;

    // Reset some control-parameters (if not already reset)

    PauzeDetectionFrameWork=false;

    // Image and video-statistics

    FrameCounter = 0;
    FieldCounter = 0;
    FrameRate = 0;
    ValidFieldCounter = true;

    const int PlayDelay = 5;        // msec
    const char Escape = 27;         // ascii-code

    // Try to open the video

    if (FileNameVideo.size() == 0) return CvDetectionFrameworkExitCodes::InvalidVideoFilename;
    CvCapture* Capture = cvCreateFileCapture(FileNameVideo.c_str());
    if (Capture == 0)
    {
        std::cout << "Error opening video " << FileNameVideo.c_str() << std::endl;
        return CvDetectionFrameworkExitCodes::NoVideoCapture;
    }

    // Get the frame and video-properties

    CvSize FrameSize = cvSize(  (int)cvGetCaptureProperty( Capture, CV_CAP_PROP_FRAME_WIDTH),
                                (int)cvGetCaptureProperty( Capture, CV_CAP_PROP_FRAME_HEIGHT));

    if ((FrameSize.width == 0) || (FrameSize.height == 0))
    {
        std::cout << "Framesize = 0" << std::endl;
        std::cout << "*** This is often the case when using the traficon-opencv-binaries !!"  << std::endl;
        std::cout << " Try using the native installed opencv-binaries on your computer"  << std::endl;


        cvReleaseCapture( &Capture);
        return CvDetectionFrameworkExitCodes::NoFrameSize;
    }

    FrameRate = (unsigned int)cvGetCaptureProperty( Capture, CV_CAP_PROP_FPS );
    if (DevelopmentMode) std::cout << "Framerate = " << FrameRate << std::endl;
    if (FrameRate < 25) FrameRate = 25; // It looks as opencv has problems with low framerates

    // Get the configuration

    string ConfigurationString;
    if (RetrieveXmlAsString(FileNameConfiguration,ConfigurationString))
    {
        // Configure the algo

        Configure(ConfigurationString);

        // Process-loop

        bool SingleStepMode = false;
        bool ContinueProcessing = true;
        bool HoldLoop = false;

        IplImage* Frame;
        IplImage* YFrame = cvCreateImage(FrameSize,IPL_DEPTH_8U,1);

        while(ContinueProcessing)
        {
            // Mouse interactions-control

            bool CurentSingleStepMode = SingleStepMode;
            while (HoldLoop)
            {
                char c = cvWaitKey(0);
                HoldLoop = false;
                if( c == Escape ) ContinueProcessing = false;
                SingleStepMode = ((c == 's') || (c == 'S'));
                CurentSingleStepMode = SingleStepMode;
            }

            // Cancelled ?

            if (!ContinueProcessing) break;

            Frame = cvQueryFrame( Capture );
            if(Frame == 0) break;		// no frames

            // Convert to Y-frames

            GetYFromRGB(Frame,YFrame);

            // get TraficonFieldCounter (if any)

            if (ValidFieldCounter)
            {
                FieldCounter = GetTraficonFieldCounter(YFrame);

                // Check fieldcounter-value on the first frame

                if (FrameCounter == 0)
                {
                    // Upon the first-frame, we expected a reasonable fieldcounter

                    if (FieldCounter > 100)
                    {
                        // Expected fieldcounter is normally 2
                        // don't relay on fieldcounter !

                        FieldCounter = 0;
                        ValidFieldCounter = false;

                        std::cout << "Abandon fieldcounter";
                        std::cout << " FieldCounter = " << FieldCounter;
                        std::cout << " FrameCounter = " << FrameCounter;
                        std::cout << std::endl;
                    }
                }
            }

            // DETECT !!

            Run(Frame);
            FrameCounter++;

            // Keycontrol
            //  Esc : stop-video
            //  s : stop (any key will continue, but s -> single-step-mode)

            if (DevelopmentMode || PauzeDetectionFrameWork)
            {
                if (PauzeDetectionFrameWork)
                {
                    PauzeDetectionFrameWork = false;
                    SingleStepMode = true;
                }

                if (SingleStepMode)
                {
                    char c = cvWaitKey(0);
                    if (c == Escape ) break;
                    SingleStepMode = ((c == 's') || (c == 'S'));
                }
                else
                {
                    char c = cvWaitKey(PlayDelay);
                    if (CurentSingleStepMode != SingleStepMode) HoldLoop = true;

                    if (c == Escape ) break;
                    if ((c == 's') || (c == 'S'))
                    {
                        // Stopped, wait for any key to continue

                        c = cvWaitKey(0);
                        if( c == Escape ) break;
                        SingleStepMode = ((c == 's') || (c == 'S'));
                    }
                }
            } // end wait-development
        } // end process-loop

        cvReleaseImage(&YFrame);
    }
    else
    {
        // Error -> can't read configuration-string

        ReturnValue = CvDetectionFrameworkExitCodes::NoConfiguration;
    }

    // Exit-step => clean up all stuff

    ExitDetection();
    cvReleaseCapture(&Capture);
    return ReturnValue;
}

unsigned int CvDetectionFramework::GetTraficonFieldCounter(const IplImage* GrayImage)
{
    unsigned long FieldCounter = 0;
    if (GrayImage == 0) return FieldCounter;
    const int ColumnsForTraficonDecoder = 16;
    if (GrayImage->height < ColumnsForTraficonDecoder) return FieldCounter;

    // create a sub-image (top of the image)

    CvRect ROI;
    ROI.x = 0;
    ROI.y = 0;
    ROI.width = GrayImage->width;
    ROI.height = ColumnsForTraficonDecoder;

    IplImage* ImageOI = GetROI(GrayImage,ROI);
    if (ImageOI == 0) return FieldCounter;

    FieldCounter = GetFrameCounter((unsigned char*)ImageOI->imageData,ROI.width, ROI.height);
    cvReleaseImage(&ImageOI);
    return FieldCounter;
}

// Helper-functions

void CvDetectionFramework::GetShortFileNameOfVideo(std::string& ShortFileName,bool ExtensionRemoved) const
{
    ShortFileName.clear();
    size_t SizeName = FileNameVideo.size();
    if (SizeName < 2) return;

    size_t DotIndex = FileNameVideo.find_last_of(".");
    if (DotIndex < 2) return;

    size_t SlashIndex = FileNameVideo.find_last_of("/\\");
    if (SlashIndex > 0)
    {
        if (ExtensionRemoved && (DotIndex > SlashIndex))
        {
            ShortFileName = FileNameVideo.substr(SlashIndex+1,DotIndex-SlashIndex-1);
        }
        else
        {
            ShortFileName = FileNameVideo.substr(SlashIndex+1,SizeName-1);
        }
    }
    else
    {
        if (ExtensionRemoved)
        {
            ShortFileName = FileNameVideo.substr(0,DotIndex-1);
        }
        else
        {
            ShortFileName = FileNameVideo;
        }
    }
}

