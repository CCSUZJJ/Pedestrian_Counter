#ifndef CVMULTIFUNCTIONPLOTWITHTRACKBAR_H
#define CVMULTIFUNCTIONPLOTWITHTRACKBAR_H

#include <opencv/cv.h> // includes OpenCV definitions
#include <vector>
#include "CvFunctionPlot.h"
#include "OpenCvMouse.h"

class CvMultiFunctionPlotWithTrackbar
{
public:

    // Helper-class

    class FunctionPlotProperties
    {
    public:

        void Reset();
        std::vector<unsigned int> PlotData;
        CvRect ROIPlot;
        CvScalar PlotColour;
    };

    // Constructor/destructor

    CvMultiFunctionPlotWithTrackbar();
    virtual ~CvMultiFunctionPlotWithTrackbar();

    // Functions to plot

    std::vector<FunctionPlotProperties> DataToPlot;

    // Start and end plotting-margins

    CvSize CanvasPlotMargin;
    CvSize ROIDistance;

    // Window-control

    std::string NameWindow;
    std::string NameTrackBar;

    // Methods

    void Reset();

    // Call-sequence
    //  1. SetDataToPlot
    //  2. SetMargins

    //  3. ConfigurePlotLayout
    //  4. Plot

    void SetMargins(const CvSize& CanvasPlotMargin,const CvSize& ROIDistance);
    void SetDataToPlot(unsigned int Reference,const std::vector<unsigned int>& Data,const CvScalar& PlotColour,unsigned int PlotHeight);
    void ConfigurePlot(const CvScalar& BackGroundColour,const std::string& NameWindow);
    void Plot(bool AddMarker,int TrackBarPosition = 0);
    void AddTrackbar(const std::string& NameTrackBar);
    void AddMouseControl(const std::string& NameOfSnapshot);

    // Draw the functions to pot on the canvas

    void Draw();

    // Axis

    CvScalar AxisColour;

    // Marker (used by slider)

    CvScalar MarkerColour;
    unsigned int DistanceFromImageBorders;

    // Trackbar-callback

    void TrackBarCallBack(int TrackBarPosition);

    // Snapshot

     void LogSnapShot();

     // Additional functions

     const IplImage* GetCanvas() const;

private:

    CvSize SizePlotROI;

    // Trackbar-control

    int MinimumTrackBarPosition;
    int MaximumTrackBarPosition;

    // Marker-control

    void PlotMarker(unsigned int Position);

    // Axis

    void DrawAxis(const FunctionPlotProperties& PlotData);

    // Snapshotcontrol

    std::string NameOfSnapshot;

    // Canvas

    CvFunctionPlot PlotCanvas;
    CvSize SizeCanvas;
    CvScalar CanvasColour;

    // MouseControl

     OpenCvMouseControl MyMouse;

    // Prevent copy-constructor

    CvMultiFunctionPlotWithTrackbar(const CvMultiFunctionPlotWithTrackbar&);    //Prevent copy-constructor
};

#endif // CVMULTIFUNCTIONPLOTWITHTRACKBAR_H
